#ifndef _SYSTEM_H_
#define _SYSTEM_H_

struct Queue {
    unsigned size;
    struct QueueNode *begin;
    struct QueueNode *end;
};

struct Entity {
    unsigned id;
    double arrival_time;
};

struct SystemStation {
    unsigned capacity;
    unsigned available;
    struct Queue input;
    struct Entity **current;
};

struct System {
    unsigned steps;
    struct SystemStation *stations;
    double arrival_rate;
    double *service_rates;
    double price;
    double *costs;
};

struct Stats {
    unsigned num_arrived;
    unsigned num_departed;
    double TIS;
    double profit;
};

int System_initialize(struct System *s,
                      unsigned steps,
                      unsigned *capacities,
                      double arrival_rate,
                      double *service_rates,
                      double price,
                      double *costs);
int System_arrive(struct System *s, unsigned id, double ts);
int System_start_working(struct System *s,
                         unsigned station_idx, unsigned *out_idx);
int System_transfer(struct System *s, unsigned from, unsigned idx);
int System_depart(struct System *s, unsigned idx,
                  double ts, double *tis, double *profit);
int System_finalize(struct System *s);
int System_log(struct System *s, FILE *fout);

#endif
