#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

struct QueueNode {
    struct QueueNode *next;
    struct QueueNode *prev;
    struct Entity *data;
};

int Queue_initialize(struct Queue *q) {
    if (!q) {
        return 1;
    }
    q->size = 0;
    q->begin = 0;
    q->end = 0;
    return 0;
}

int Queue_push(struct Queue *q, struct Entity *data) {
    if (!q) {
        return 1;
    }
    struct QueueNode *node = malloc(sizeof(struct QueueNode));
    if (!node) {
        return 2;
    }
    node->next = 0;
    node->prev = q->end;
    node->data = data;
    if (!q->begin) {
        q->begin = node;
    }
    if (q->end) {
        q->end->next = node;
    }
    q->end = node;
    q->size++;
    return 0;
}

int Queue_top(struct Queue *q, struct Entity **out) {
    if (!q || !out) {
        return 1;
    }
    if (!q->begin) {
        return 2;
    }
    *out = q->begin->data;
    return 0;
}

int Queue_pop(struct Queue *q) {
    if (!q) {
        return 1;
    }
    if (!q->size) {
        return 2;
    }
    q->size--;
    struct QueueNode *temp = q->begin->next;
    free(q->begin);
    q->begin = temp;
    if (q->begin) {
        q->begin->prev = 0;
    }
    if (!q->begin) {
        q->end = 0;
    }
    return 0;
}

int Queue_finalize(struct Queue *q, char clear_content) {
    if (!q) {
        return 1;
    }
    struct Entity *data;
    for (; q->size; Queue_pop(q)) {
        if (clear_content) {
            Queue_top(q, &data);
            free(data);
        }
    }
    return 0;
}

int Queue_log(struct Queue *q, FILE *fout) {
    if (!q || !fout) {
        return 1;
    }
    fprintf(fout, "%s\n", "Queue:");
    if (!q->size) {
        fprintf(fout, "%s\n", "Empty");
        return 0;
    }
    for(struct QueueNode *iter = q->begin;
        iter; iter = iter->next) {
        fprintf(fout, "id: %u ts: %lf\n",
                iter->data->id, iter->data->arrival_time);
    }
    return 0;
}
