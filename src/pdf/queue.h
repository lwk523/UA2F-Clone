#ifndef UA2F_QUEUE_H
#define UA2F_QUEUE_H

#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Forward declaration to avoid including libnetfilter header here
struct nfq_q_handle;

// Packet container holding NFQUEUE id and payload
struct packet {
    uint32_t id;
    unsigned char *payload;
    size_t payload_len;
    struct nfq_q_handle *qh;
};

// Thread-safe circular queue for packets
struct queue {
    struct packet *buf;
    size_t head;
    size_t tail;
    size_t size;
    bool stopped;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
};

int queue_init(struct queue *q, size_t size);
void queue_destroy(struct queue *q);
void queue_stop(struct queue *q);
int queue_push(struct queue *q, const struct packet *pkt);
int queue_pop(struct queue *q, struct packet *pkt);

#endif // UA2F_QUEUE_H
