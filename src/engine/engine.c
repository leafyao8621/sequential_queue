#include <stdio.h>
#include <math.h>
#include "engine.h"
#include "priority_queue.h"
#include "engine_events.h"

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
                      FILE *fout) {
    if (!e) {
        return 1;
    }
    e->time_now = 0;
    e->time_end = time_end;
    mt19937_initialize(&e->gen, seed);
    System_initialize(&e->model,
                      steps,
                      capacities,
                      arrival_rate,
                      service_rates,
                      price,
                      costs);
    PriorityQueue_initialize(&e->events);
    struct Event first;
    Event_arrive(&first, 0);
    double ts = -log(mt19937_gen(&e->gen) / (double)0xffffffffu) /
                arrival_rate;
    PriorityQueue_push(&e->events, ts, first);
    e->stats.num_arrived = 0;
    e->stats.num_departed = 0;
    e->stats.profit = 0;
    e->stats.TIS = 0;
    e->verbose = verbose;
    e->fout = fout;
    return 0;
}

int Engine_step(struct Engine *e, char *cont) {
    if (!e || !cont) {
        return 1;
    }
    double ts = 0;
    struct Event event;
    if (PriorityQueue_top(&e->events, &ts, &event)) {
        return 2;
    }
    if (ts > e->time_end) {
        *cont = 0;
        return 0;
    }
    *cont = 1;
    e->time_now = ts;
    if (e->verbose) {
        fprintf(e->fout, "ts: %lf\n", ts);
        event.logger(&event, e->fout);
    }
    if (event.handler(&event, e)) {
        return 3;
    }
    if (PriorityQueue_pop(&e->events)) {
        return 4;
    }
    return 0;
}

int Engine_run(struct Engine *e) {
    if (!e) {
        return 1;
    }
    char cont = 1;
    for (; cont;) {
        if (Engine_step(e, &cont)) {
            return 2;
        }
        if (e->verbose) {
            System_log(&e->model, e->fout);
        }
    }
    e->stats.TIS /= e->stats.num_departed;
    for (unsigned i = 0; i < e->model.steps; ++i) {
        e->stats.profit -= e->model.stations[i].capacity * e->model.costs[i];
    }
    return 0;
}

int Engine_log_stats(struct Engine *e) {
    if (!e || !e->fout) {
        return 1;
    }
    fprintf(e->fout, "num_arrived: %d\n", e->stats.num_arrived);
    fprintf(e->fout, "num_departed: %d\n", e->stats.num_departed);
    fprintf(e->fout, "TIS: %lf\n", e->stats.TIS);
    fprintf(e->fout, "profit: %lf\n", e->stats.profit);
    return 0;
}

int Engine_finalize(struct Engine *e) {
    if (!e) {
        return 1;
    }
    PriorityQueue_finalize(&e->events);
    System_finalize(&e->model);
    return 0;
}
