#include "handler.h"
#include "config.h"
#include "utils.h"
#include "statistics.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/tcp.h>

/* Simplified packet handler: currently just forwards packet */
void handle_packet(struct nfqueue_context *nfctx, struct packet *pkt) {
    // In a full implementation, parse IPv4/IPv6 headers and modify the HTTP
    // User-Agent header. For now, immediately accept the packet.
    nfqueue_send_verdict(nfctx, pkt->id, NF_ACCEPT);
}

/* Worker thread loop */
void *worker_thread(void *arg) {
    struct nfqueue_context *nfctx = (struct nfqueue_context *)arg;
    struct packet pkt;
    while (1) {
        if (queue_pop(nfctx->queue, &pkt) != 0) {
            // queue has been stopped
            break;
        }
        handle_packet(nfctx, &pkt);
        // free packet payload
        free(pkt.payload);
    }
    return NULL;
}
