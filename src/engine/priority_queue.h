#ifndef _PRIORITY_QUEUE_H_
#define _PRIORITY_QUEUE_H_

#include "engine.h"

struct PriorityQueueNode {
    double ts;
    struct Event data;
    struct PriorityQueueNode *parent;
    struct PriorityQueueNode *left;
    struct PriorityQueueNode *right;
};

int PriorityQueue_initialize(struct PriorityQueue *pq);
int PriorityQueue_push(struct PriorityQueue *pq, double ts, struct Event data);
int PriorityQueue_top(struct PriorityQueue *pq,
                      double *out_ts,
                      struct Event *out_data);
int PriorityQueue_pop(struct PriorityQueue *pq);
int PriorityQueue_finalize(struct PriorityQueue *pq);

#endif
