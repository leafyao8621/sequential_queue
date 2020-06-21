#ifndef _RUNNER_H_
#define _RUNNER_H_

#include <stdio.h>

struct Runner {
    unsigned n_param;
    unsigned n_iter;
    unsigned n_thread;
    double time_end;
    unsigned seed;
    unsigned *steps;
    unsigned *capacities;
    double *arrival_rate;
    double *service_rates;
    double *price;
    double *costs;
    FILE *fout;
};

int Runner_run(struct Runner *r);

#endif
