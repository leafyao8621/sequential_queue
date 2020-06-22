#include <stdio.h>
#include "runner/runner.h"
#include "engine/engine.h"

int main(void) {
    unsigned steps[3] = {1, 3, 2};
    unsigned capacities[6] = {1, 4, 2, 3, 3, 2};
    double arrival_rate[3] = {2, 2, 1};
    double service_rates[6] = {1, 2, 3, 1, 3, 2};
    double price[3] = {5, 5, 6};
    double costs[6] = {100, 100, 200, 300, 300, 200};
    struct Runner r = {
        3,
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
