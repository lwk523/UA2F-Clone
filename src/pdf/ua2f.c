#include "config.h"
#include "queue.h"
#include "nfqueue.h"
#include "handler.h"
#include "statistics.h"
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static struct nfqueue_context nfctx;
static struct queue q;
static struct statistics stats;
static pthread_t *workers;
static int num_workers;

static void signal_handler(int sig) {
    // Stop the queue so that worker threads can exit
    queue_stop(&q);
}

int main(int argc, char **argv) {
    // Load configuration
    config_load();

    // Initialize statistics
    stats_init(&stats);

    // Initialize queue with fixed size (1024 packets)
    if (queue_init(&q, 1024) != 0) {
        fprintf(stderr, "Failed to initialize queue\n");
        return 1;
    }

    // Initialize NFQUEUE (use queue number 10010)
    if (nfqueue_init(&nfctx, &q, 10010) != 0) {
        fprintf(stderr, "Failed to initialize NFQUEUE\n");
        return 1;
    }

    // Spawn worker threads based on configuration
    num_workers = config.min_threads;
    if (num_workers <= 0) {
        num_workers = 1;
    }
    workers = (pthread_t *)malloc(sizeof(pthread_t) * num_workers);
    if (!workers) {
        fprintf(stderr, "Failed to allocate worker thread array\n");
        return 1;
    }
    for (int i = 0; i < num_workers; i++) {
        pthread_create(&workers[i], NULL, worker_thread, &nfctx);
    }

    // Set up signal handlers for graceful shutdown
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Run NFQUEUE event loop (blocks until error or stopped)
    nfqueue_run(&nfctx);

    // Wait for worker threads to finish
    for (int i = 0; i < num_workers; i++) {
        pthread_join(workers[i], NULL);
    }
    free(workers);

    // Clean up NFQUEUE and queue
    nfqueue_close(&nfctx);
    queue_destroy(&q);

    // Print statistics summary
    printf("Processed: %lu\n", stats.processed);
    printf("Modified: %lu\n", stats.modified);
    printf("Skipped: %lu\n", stats.skipped);

    return 0;
}
