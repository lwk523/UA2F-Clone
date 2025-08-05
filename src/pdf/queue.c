#include "queue.h"

#include <stdlib.h>
#include <string.h>

int queue_init(struct queue *q, size_t size) {
    if (!q || size == 0) {
        return -1;
    }
    q->buf = (struct packet *)calloc(size, sizeof(struct packet));
    if (!q->buf) {
        return -1;
    }
    q->head = 0;
    q->tail = 0;
    q->size = size;
    q->stopped = false;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->not_empty, NULL);
    pthread_cond_init(&q->not_full, NULL);
    return 0;
}

void queue_destroy(struct queue *q) {
    if (!q) return;
    free(q->buf);
    q->buf = NULL;
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->not_empty);
    pthread_cond_destroy(&q->not_full);
}

void queue_stop(struct queue *q) {
    pthread_mutex_lock(&q->mutex);
    q->stopped = true;
    pthread_cond_broadcast(&q->not_empty);
    pthread_cond_broadcast(&q->not_full);
    pthread_mutex_unlock(&q->mutex);
}

static bool queue_is_full(const struct queue *q) {
    return ((q->tail + 1) % q->size) == q->head;
}

static bool queue_is_empty(const struct queue *q) {
    return q->head == q->tail;
}

int queue_push(struct queue *q, const struct packet *pkt) {
    pthread_mutex_lock(&q->mutex);
    while (!q->stopped && queue_is_full(q)) {
        pthread_cond_wait(&q->not_full, &q->mutex);
    }
    if (q->stopped) {
        pthread_mutex_unlock(&q->mutex);
        return -1;
    }
    // Copy packet into queue
    q->buf[q->tail] = *pkt;
    q->tail = (q->tail + 1) % q->size;
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->mutex);
    return 0;
}

int queue_pop(struct queue *q, struct packet *pkt) {
    pthread_mutex_lock(&q->mutex);
    while (!q->stopped && queue_is_empty(q)) {
        pthread_cond_wait(&q->not_empty, &q->mutex);
    }
    if (queue_is_empty(q) && q->stopped) {
        pthread_mutex_unlock(&q->mutex);
        return -1;
    }
    *pkt = q->buf[q->head];
    q->head = (q->head + 1) % q->size;
    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->mutex);
    return 0;
}
