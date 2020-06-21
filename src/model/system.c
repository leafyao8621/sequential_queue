#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "system.h"
#include "queue.h"

int System_initialize(struct System *s,
                      unsigned steps,
                      unsigned *capacities,
                      double arrival_rate,
                      double *service_rates,
                      double price,
                      double *costs) {
    if (!s || !capacities || !service_rates || !costs) {
        return 1;
    }
    s->steps = steps;
    s->stations = calloc(steps, sizeof(struct SystemStation));
    struct SystemStation *iter = s->stations;
    unsigned *iter_cap = capacities;
    unsigned cap = 0;
    for (unsigned i = 0; i < steps; ++i, ++iter, ++iter_cap) {
        cap = *iter_cap;
        iter->capacity = cap;
        iter->available = cap;
        iter->current = calloc(cap, sizeof(struct Entity*));
        Queue_initialize(&iter->input);
    }
    s->arrival_rate = arrival_rate;
    s->service_rates = malloc(steps * sizeof(double));
    memcpy(s->service_rates, service_rates, steps * sizeof(double));
    s->price = price;
    s->costs = malloc(steps * sizeof(double));
    memcpy(s->costs, costs, steps * sizeof(double));
    return 0;
}

int System_arrive(struct System *s, unsigned id, double ts) {
    if (!s) {
        return 1;
    }
    struct Entity *ent = malloc(sizeof(struct Entity));
    if (!ent) {
        return 2;
    }
    ent->id = id;
    ent->arrival_time = ts;
    if (Queue_push(&s->stations->input, ent)) {
        return 3;
    }
    return 0;
}

int System_start_working(struct System *s,
                         unsigned station_idx, unsigned *out_idx) {
    if (!s) {
        return 1;
    }
    struct SystemStation *station = s->stations + station_idx;
    if (!station->available) {
        return 2;
    }
    struct Entity *ent = 0;
    if (Queue_top(&station->input, &ent)) {
        return 3;
    }
    Queue_pop(&station->input);
    station->available--;
    struct Entity **slot = station->current;
    unsigned i = 0;
    for (; i < station->capacity && *slot; ++i, ++slot);
    *slot = ent;
    *out_idx = i;
    return 0;
}

int System_transfer(struct System *s,
                    unsigned from, unsigned idx) {
    if (!s) {
        return 1;
    }
    if (from >= s->steps - 1 || idx >= s->stations[from].capacity) {
        return 2;
    }
    struct Entity *ent = s->stations[from].current[idx];
    if (!ent) {
        return 3;
    }
    if (Queue_push(&s->stations[from + 1].input, ent)) {
        return 4;
    }
    s->stations[from].current[idx] = 0;
    s->stations[from].available++;
    return 0;
}

int System_depart(struct System *s, unsigned idx,
                  double ts, double *tis, double *profit) {
    if (!s) {
        return 1;
    }
    if (idx >= s->stations[s->steps - 1].capacity) {
        return 2;
    }
    struct Entity *ent = s->stations[s->steps - 1].current[idx];
    if (!ent) {
        return 3;
    }
    *profit = s->price;
    *tis = ts - ent->arrival_time;
    s->stations[s->steps - 1].current[idx] = 0;
    s->stations[s->steps - 1].available++;
    free(ent);
    return 0;
}

int System_finalize(struct System *s) {
    if (!s) {
        return 1;
    }
    struct SystemStation *iter = s->stations;
    for (unsigned i = 0; i < s->steps; ++i,
                                       Queue_finalize(&((iter++)->input), 1)) {
        struct Entity **iter_ent = iter->current;
        for (unsigned j = 0; j < iter->capacity; ++j, ++iter_ent) {
            if (*iter_ent) {
                free(*iter_ent);
            }
        }
        free(iter->current);
    }
    free(s->stations);
    free(s->service_rates);
    free(s->costs);
    return 0;
}

int System_log(struct System *s, FILE *fout) {
    if (!s || !fout) {
        return 1;
    }
    struct SystemStation *iter = s->stations;
    for (unsigned i = 0; i < s->steps; ++i, ++iter) {
        fprintf(fout, "Station %u:\nAvailable: %u\nCurrent:\n",
                i, iter->available);
        struct Entity **iter_ent = iter->current;
        for (unsigned j = 0; j < iter->capacity; ++j, ++iter_ent) {
            if (*iter_ent) {
                fprintf(fout, "id: %u ts: %lf\n",
                        (*iter_ent)->id, (*iter_ent)->arrival_time);
            } else {
                fprintf(fout, "%s\n", "NULL");
            }
        }
        Queue_log(&iter->input, fout);
    }
    return 0;
}
