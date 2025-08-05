#include "statistics.h"

void stats_init(struct statistics *stats) {
    atomic_init(&stats->processed, 0);
    atomic_init(&stats->modified, 0);
    atomic_init(&stats->skipped, 0);
}

void stats_inc_processed(struct statistics *stats) {
    atomic_fetch_add_explicit(&stats->processed, 1, memory_order_relaxed);
}

void stats_inc_modified(struct statistics *stats) {
    atomic_fetch_add_explicit(&stats->modified, 1, memory_order_relaxed);
}

void stats_inc_skipped(struct statistics *stats) {
    atomic_fetch_add_explicit(&stats->skipped, 1, memory_order_relaxed);
}
