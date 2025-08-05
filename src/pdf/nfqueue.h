#ifndef UA2F_NFQUEUE_H
#define UA2F_NFQUEUE_H

#include <stdint.h>
#include <pthread.h>
#include <libnetfilter_queue/libnetfilter_queue.h>

#include "queue.h"

struct nfqueue_context {
    struct nfq_handle *handle;
    struct nfq_q_handle *qh;
    int fd;
    pthread_mutex_t verdict_mutex;
    struct queue *queue;
};

int nfqueue_init(struct nfqueue_context *ctx, struct queue *q, uint16_t queue_num);
int nfqueue_run(struct nfqueue_context *ctx);
void nfqueue_close(struct nfqueue_context *ctx);
int nfqueue_send_verdict(struct nfqueue_context *ctx, uint32_t id, uint32_t verdict);

#endif // UA2F_NFQUEUE_H
