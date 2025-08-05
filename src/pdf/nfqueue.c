#include "nfqueue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <libnetfilter_queue/libnetfilter_queue.h>

// Callback for NFQUEUE, called when a packet is received
static int cb(struct nfq_q_handle *qh, struct nfgenmsg *nfmsg, struct nfq_data *nfa, void *data) {
    struct nfqueue_context *ctx = (struct nfqueue_context *)data;
    struct nfqnl_msg_packet_hdr *ph;
    uint32_t id = 0;
    unsigned char *payload;
    int ret;

    ph = nfq_get_msg_packet_hdr(nfa);
    if (ph) {
        id = ntohl(ph->packet_id);
    }

    ret = nfq_get_payload(nfa, &payload);
    if (ret >= 0) {
        struct packet pkt;
        pkt.id = id;
        pkt.len = (size_t)ret;
        pkt.payload = (unsigned char *)malloc(pkt.len);
        if (!pkt.payload) {
            // drop packet if allocation fails
            nfq_set_verdict(qh, id, NF_DROP, 0, NULL);
            return 0;
        }
        memcpy(pkt.payload, payload, pkt.len);
        queue_push(ctx->queue, &pkt);
    }
    // temporarily set hold verdict; worker will send verdict later
    return 0;
}

int nfqueue_init(struct nfqueue_context *ctx, struct queue *q, uint16_t queue_num) {
    ctx->queue = q;
    ctx->handle = nfq_open();
    if (!ctx->handle) {
        perror("nfq_open");
        return -1;
    }
    // Unbind existing handlers for both IPv4 and IPv6
    nfq_unbind_pf(ctx->handle, AF_INET);
    nfq_unbind_pf(ctx->handle, AF_INET6);
    // Bind handlers
    if (nfq_bind_pf(ctx->handle, AF_INET) < 0 || nfq_bind_pf(ctx->handle, AF_INET6) < 0) {
        perror("nfq_bind_pf");
        nfq_close(ctx->handle);
        return -1;
    }
    ctx->qh = nfq_create_queue(ctx->handle, queue_num, &cb, ctx);
    if (!ctx->qh) {
        perror("nfq_create_queue");
        nfq_close(ctx->handle);
        return -1;
    }
    // Copy entire packet
    if (nfq_set_mode(ctx->qh, NFQNL_COPY_PACKET, 0xffff) < 0) {
        perror("nfq_set_mode");
        nfq_destroy_queue(ctx->qh);
        nfq_close(ctx->handle);
        return -1;
    }
    ctx->fd = nfq_fd(ctx->handle);
    pthread_mutex_init(&ctx->verdict_mutex, NULL);
    return 0;
}

int nfqueue_run(struct nfqueue_context *ctx) {
    char buf[4096];
    int rv;
    while ((rv = recv(ctx->fd, buf, sizeof(buf), 0)) >= 0) {
        nfq_handle_packet(ctx->handle, buf, rv);
    }
    return 0;
}

void nfqueue_close(struct nfqueue_context *ctx) {
    if (ctx->qh) {
        nfq_destroy_queue(ctx->qh);
        ctx->qh = NULL;
    }
    if (ctx->handle) {
        nfq_close(ctx->handle);
        ctx->handle = NULL;
    }
    pthread_mutex_destroy(&ctx->verdict_mutex);
}

int nfqueue_send_verdict(struct nfqueue_context *ctx, uint32_t id, uint32_t verdict) {
    int ret;
    pthread_mutex_lock(&ctx->verdict_mutex);
    ret = nfq_set_verdict(ctx->qh, id, verdict, 0, NULL);
    pthread_mutex_unlock(&ctx->verdict_mutex);
    return ret;
}
