#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "../engine/engine.h"
#include "runner.h"

static struct Stats *partial_sums;
static pthread_barrier_t barrier;


struct Arg {
    unsigned id;
    struct Runner *r;
};

void *sub_runner(void *arg) {
    struct Runner *r = ((struct Arg*)arg)->r;
    unsigned id = ((struct Arg*)arg)->id;
    unsigned *iter_steps = r->steps;
    unsigned *iter_capacities = r->capacities;
    double *iter_arrival_rate = r->arrival_rate;
    double *iter_service_rates = r->service_rates;
    double *iter_price = r->price;
    double *iter_costs = r->costs;
    struct Engine engine;
    unsigned n_iter = r->n_iter / (r->n_thread - 1);
    n_iter += id <= r->n_iter % (r->n_thread - 1) != 0;
    for (unsigned i = 0; i < r->n_param;
         ++i,
         ++iter_arrival_rate,
         ++iter_price,
         iter_capacities += *iter_steps,
         iter_service_rates += *iter_steps,
         iter_costs += *iter_steps,
         ++iter_steps) {
        for (unsigned j = 0, k = r->seed; j < n_iter; ++j, ++k) {
            Engine_initialize(&engine,
                              r->time_end,
                              k,
                              *iter_steps,
                              iter_capacities,
                              *iter_arrival_rate,
                              iter_service_rates,
                              *iter_price,
                              iter_costs, 0, 0);
            Engine_run(&engine);
            partial_sums[id].num_arrived += engine.stats.num_arrived;
            partial_sums[id].num_departed += engine.stats.num_departed;
            partial_sums[id].TIS += engine.stats.TIS;
            partial_sums[id].profit += engine.stats.profit;
            Engine_finalize(&engine);
        }
        pthread_barrier_wait(&barrier);
        pthread_barrier_wait(&barrier);
    }
    return 0;
}

int Runner_run(struct Runner *r) {
    if (!r) {
        return 1;
    }
    pthread_barrier_init(&barrier, 0, r->n_thread);
    pthread_t *pool = malloc(sizeof(pthread_t) * (r->n_thread - 1));
    struct Arg *args = malloc(sizeof(struct Arg) * (r->n_thread - 1));
    partial_sums = calloc((r->n_thread - 1), sizeof(struct Stats));
    pthread_t *iter_pool = pool;
    struct Arg *iter_args = args;
    for (unsigned i = 0; i < r->n_thread - 1; ++i, ++iter_pool, ++iter_args) {
        iter_args->id = i;
        iter_args->r = r;
        pthread_create(iter_pool, 0, sub_runner, iter_args);
    }
    struct Stats stats;
    for (unsigned i = 0; i < r->n_param; ++i) {
        pthread_barrier_wait(&barrier);
        memset(&stats, 0, sizeof(struct Stats));
        struct Stats *iter_partial_sums = partial_sums;
        for (unsigned i = 0; i < r->n_thread - 1; ++i, ++iter_partial_sums) {
            stats.num_arrived += iter_partial_sums->num_arrived;
            stats.num_departed += iter_partial_sums->num_departed;
            stats.TIS += iter_partial_sums->TIS;
            stats.profit += iter_partial_sums->profit;
        }
        memset(partial_sums, 0, sizeof(struct Stats) * (r->n_thread - 1));
        stats.num_arrived /= r->n_iter;
        stats.num_departed /= r->n_iter;
        stats.TIS /= r->n_iter;
        stats.profit /= r->n_iter;
        if (r->fout) {
            fprintf(r->fout, "Setting: %u\n", i);
            fprintf(r->fout, "num_arrived: %u\n", stats.num_arrived);
            fprintf(r->fout, "num_departed: %u\n", stats.num_departed);
            fprintf(r->fout, "TIS: %lf\n", stats.TIS);
            fprintf(r->fout, "profit: %lf\n", stats.profit);
        }
        pthread_barrier_wait(&barrier);
    }
    iter_pool = pool;
    for (unsigned i = 0; i < r->n_thread - 1; ++i, ++iter_pool) {
        pthread_join(*iter_pool, 0);
    }
    free(pool);
    free(args);
    free(partial_sums);
    pthread_barrier_destroy(&barrier);
    return 0;
}
