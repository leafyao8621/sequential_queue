#include <stdio.h>
#include <stdlib.h>
#include "engine.h"
#include "priority_queue.h"

int PriorityQueue_initialize(struct PriorityQueue *pq) {
    if (!pq) {
        return 1;
    }
    pq->size = 0;
    pq->root = 0;
    return 0;
}

int PriorityQueue_push(struct PriorityQueue *pq, double ts, struct Event data) {
    if (!pq) {
        return 1;
    }
    pq->size++;
    struct PriorityQueueNode *e = malloc(sizeof(struct PriorityQueueNode));
    e->ts = ts;
    e->data = data;
    e->parent = 0;
    e->left = 0;
    e->right = 0;
    if (!pq->root) {
        pq->root = e;
        return 0;
    }
    unsigned n = pq->size;
    struct PriorityQueueNode *c = pq->root;
    for (; n >> 2; n >>= 1) {
        c = (n & 1) ? c->right : c->left;
    }
    e->parent = c;
    if (n & 1) {
        c->right = e;
    } else {
        c->left = e;
    }
    for (c = e; c->parent && (c->ts < c->parent->ts); c = c->parent) {
        c->ts = c->parent->ts;
        c->data = c->parent->data;
        c->parent->ts = ts;
        c->parent->data = data;
    }
    return 0;
}

int PriorityQueue_top(struct PriorityQueue *pq,
                      double *out_ts,
                      struct Event *out_data) {
    if (!pq || !out_ts || !out_data) {
        return 1;
    }
    if (!pq->root) {
        return 2;
    }
    *out_ts = pq->root->ts;
    *out_data = pq->root->data;
    return 0;
}

int PriorityQueue_pop(struct PriorityQueue *pq) {
    if (!pq) {
        return 1;
    }
    if (!pq->root) {
        return 2;
    }
    if (pq->size == 1) {
        pq->size = 0;
        free(pq->root);
        pq->root = 0;
        return 0;
    }
    unsigned n = pq->size;
    struct PriorityQueueNode *c = pq->root;
    for (; n >> 2; n >>= 1) {
        c = (n & 1) ? c->right : c->left;
    }
    if (n & 1) {
        c = c->right;
        c->parent->right = 0;
    } else {
        c = c->left;
        c->parent->left = 0;
    }
    pq->size--;
    pq->root->ts = c->ts;
    pq->root->data = c->data;
    free(c);
    char side = 0;
    char swap = 1;
    double buf_ts = 0;
    struct Event buf_data;
    for (c = pq->root; (c->left || c->right) && swap;) {
        side = 0;
        swap = 0;
        if (c->left) {
            side |= 1;
        }
        if (c->right) {
            side |= 2;
        }
        buf_ts = c->ts;
        buf_data = c->data;
        switch (side) {
        case 1:
            if (c->ts >= c->left->ts) {
                c->ts = c->left->ts;
                c->data = c->left->data;
                c->left->ts = buf_ts;
                c->left->data = buf_data;
                c = c->left;
                swap = 1;
            }
            break;
        case 2:
            if (c->ts >= c->right->ts) {
                c->ts = c->right->ts;
                c->data = c->right->data;
                c->right->ts = buf_ts;
                c->right->data = buf_data;
                c = c->right;
                swap = 1;
            }
            break;
        case 3:
            if (c->left->ts < c->right->ts) {
                if (c->ts >= c->left->ts) {
                    c->ts = c->left->ts;
                    c->data = c->left->data;
                    c->left->ts = buf_ts;
                    c->left->data = buf_data;
                    c = c->left;
                    swap = 1;
                } else if (c->ts >= c->right->ts) {
                    c->ts = c->right->ts;
                    c->data = c->right->data;
                    c->right->ts = buf_ts;
                    c->right->data = buf_data;
                    c = c->right;
                    swap = 1;
                }
            } else {
                if (c->ts >= c->right->ts) {
                    c->ts = c->right->ts;
                    c->data = c->right->data;
                    c->right->ts = buf_ts;
                    c->right->data = buf_data;
                    c = c->right;
                    swap = 1;
                } else if (c->ts >= c->left->ts) {
                    c->ts = c->left->ts;
                    c->data = c->left->data;
                    c->left->ts = buf_ts;
                    c->left->data = buf_data;
                    c = c->left;
                    swap = 1;
                }
            }
            break;
        }    
    }
    return 0;
}

int PriorityQueue_finalize(struct PriorityQueue *pq) {
    if (!pq) {
        return 1;
    }
    for (; pq->size; PriorityQueue_pop(pq));
    return 0;
}
