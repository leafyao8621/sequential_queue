#include <stdio.h>
#include <math.h>
#include "engine.h"
#include "engine_events.h"
#include "priority_queue.h"
#include "../model/system.h"

static int Event_arrive_handler(struct Event *ev,
                                struct Engine *e) {
    struct Event buf;
    double next_arrival;
    if (!ev || !e) {
        return 1;
    }
    System_arrive(&e->model, e->stats.num_arrived, e->time_now);
    next_arrival = e->time_now -
                   log(mt19937_gen(&e->gen) / (double)0xffffffffu) /
                   e->model.arrival_rate;
    Event_arrive(&buf, ++e->stats.num_arrived);
    PriorityQueue_push(&e->events, next_arrival, buf);
    if (e->model.stations->available && e->model.stations->input.size == 1) {
        Event_start_working(&buf, 0);
        PriorityQueue_push(&e->events, e->time_now, buf);
    }
    return 0;
}

static int Event_arrive_logger(struct Event *e, FILE *fout) {
    if (!e || !fout) {
        return 1;
    }
    fprintf(fout, "Arrival id: %u\n", e->data.arrive.id);
    return 0;
}

int Event_arrive(struct Event *e, unsigned id) {
    if (!e) {
        return 1;
    }
    e->data.arrive.id = id;
    e->handler = Event_arrive_handler;
    e->logger = Event_arrive_logger;
    return 0;
}

static int Event_start_working_handler(struct Event *ev,
                                       struct Engine *e) {
    struct Event buf;
    double next;
    unsigned idx;
    if (!ev || !e) {
        return 1;
    }
    System_start_working(&e->model, ev->data.start_working.station_idx, &idx);
    next = e->time_now -
           log(mt19937_gen(&e->gen) / (double)0xffffffffu) /
           e->model.service_rates[ev->data.start_working.station_idx];
    if (ev->data.start_working.station_idx < e->model.steps - 1) {
        Event_transfer(&buf, ev->data.start_working.station_idx, idx);
    } else {
        Event_depart(&buf, idx);
    }
    PriorityQueue_push(&e->events, next, buf);
    return 0;
}

static int Event_start_working_logger(struct Event *e, FILE *fout) {
    if (!e || !fout) {
        return 1;
    }
    fprintf(fout, "Start Working station_idx: %u\n",
            e->data.start_working.station_idx);
    return 0;
}

int Event_start_working(struct Event *e, unsigned station_idx) {
    if (!e) {
        return 1;
    }
    e->data.start_working.station_idx = station_idx;
    e->handler = Event_start_working_handler;
    e->logger = Event_start_working_logger;
    return 0;
}

static int Event_transfer_handler(struct Event *ev,
                                  struct Engine *e) {
    struct Event buf;
    if (!ev || !e) {
        return 1;
    }
    System_transfer(&e->model, ev->data.transfer.from, ev->data.transfer.idx);
    if (e->model.stations[ev->data.transfer.from].input.size) {
        Event_start_working(&buf, ev->data.transfer.from);
        PriorityQueue_push(&e->events, e->time_now, buf);
    }
    if (e->model.stations[ev->data.transfer.from + 1].available &&
        e->model.stations[ev->data.transfer.from + 1].input.size == 1) {
        Event_start_working(&buf, ev->data.transfer.from + 1);
        PriorityQueue_push(&e->events, e->time_now, buf);
    }
    return 0;
}

static int Event_transfer_logger(struct Event *e, FILE *fout) {
    if (!e || !fout) {
        return 1;
    }
    fprintf(fout, "Transfer from: %u idx: %u\n",
            e->data.transfer.from,
            e->data.transfer.idx);
    return 0;
}

int Event_transfer(struct Event *e, unsigned from, unsigned idx) {
    if (!e) {
        return 1;
    }
    e->data.transfer.from = from;
    e->data.transfer.idx = idx;
    e->handler = Event_transfer_handler;
    e->logger = Event_transfer_logger;
    return 0;
}

static int Event_depart_handler(struct Event *ev,
                                  struct Engine *e) {
    struct Event buf;
    double tis, profit;
    if (!ev || !e) {
        return 1;
    }
    System_depart(&e->model, ev->data.depart.idx, e->time_now, &tis, &profit);
    if (e->model.stations[e->model.steps - 1].input.size) {
        Event_start_working(&buf, e->model.steps - 1);
        PriorityQueue_push(&e->events, e->time_now, buf);
    }
    e->stats.num_departed++;
    e->stats.TIS += tis;
    e->stats.profit += profit;
    return 0;
}

static int Event_depart_logger(struct Event *e, FILE *fout) {
    if (!e || !fout) {
        return 1;
    }
    fprintf(fout, "Departure idx: %u\n",
            e->data.depart.idx);
    return 0;
}

int Event_depart(struct Event *e, unsigned idx) {
    if (!e) {
        return 1;
    }
    e->data.depart.idx = idx;
    e->handler = Event_depart_handler;
    e->logger = Event_depart_logger;
    return 0;
}
