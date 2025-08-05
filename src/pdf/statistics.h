#ifndef UA2F_STATISTICS_H
#define UA2F_STATISTICS_H

#include <stdatomic.h>

struct statistics {
    atomic_ulong processed;
    atomic_ulong modified;
    atomic_ulong skipped;
};

void stats_init(struct statistics *stats);
void stats_inc_processed(struct statistics *stats);
void stats_inc_modified(struct statistics *stats);
void stats_inc_skipped(struct statistics *stats);

#endif // UA2F_STATISTICS_H
