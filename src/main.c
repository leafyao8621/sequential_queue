#include <stdio.h>
#include "runner/runner.h"
#include "engine/engine.h"

int main(void) {
    unsigned steps[2] = {3, 2};
    unsigned capacities[5] = {4, 2, 3, 3, 2};
    double arrival_rate[2] = {2, 1};
    double service_rates[5] = {2, 3, 1, 3, 2};
    double price[2] = {5, 6};
    double costs[5] = {100, 200, 300, 300, 200};
    struct Runner r = {
        2,
        100,
        12,
        10000,
        10,
        steps,
        capacities,
        arrival_rate,
        service_rates,
        price,
        costs,
        stdout
    };
    Runner_run(&r);
}
