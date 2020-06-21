#ifndef _ENGINE_H_
#define _ENGINE_H_

#include <stdio.h>
#include "../model/system.h"
#include "../util/mt19937.h"

struct PriorityQueue {
    unsigned size;
    struct PriorityQueueNode *root;
};

struct Engine {
    double time_now;
    double time_end;
    struct MT19937 gen;
    struct PriorityQueue events;
    struct System model;
    struct Stats stats;
    char verbose;
    FILE *fout;
};

struct Event {
    union {
        struct {
            unsigned id;
        } arrive;
        struct {
            unsigned station_idx;
        } start_working;
        struct {
            unsigned from;
            unsigned idx;
        } transfer;
        struct {
            unsigned idx;
        } depart;
    } data;
    int (*handler)(struct Event*, struct Engine*);
    int (*logger)(struct Event*, FILE*);
};



int Engine_initialize(struct Engine *e,
                      double time_end,
                      unsigned seed,
                      unsigned steps,
                      unsigned *capacities,
                      double arrival_rate,
                      double *service_rates,
                      double price,
                      double *costs,
                      char verbose,
                      FILE *fout);
int Engine_step(struct Engine *e, char *cont);
int Engine_run(struct Engine *e);
int Engine_log_stats(struct Engine *e);
int Engine_finalize(struct Engine *e);

#endif
