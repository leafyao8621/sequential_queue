#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "system.h"

int Queue_initialize(struct Queue *q);
int Queue_push(struct Queue *q, struct Entity *data);
int Queue_top(struct Queue *q, struct Entity **out);
int Queue_pop(struct Queue *q);
int Queue_finalize(struct Queue *q, char clear_content);
int Queue_log(struct Queue *q, FILE *fout);

#endif
