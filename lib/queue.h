#ifndef _QUEUE_H
#define _QUEUE_H

#include "memdebug.h"

#include <stdlib.h>
#include <pthread.h>

#define QUEUE_MAX_PENDING 10000

struct queue_item {
    void *data;
    struct queue_item *next;
};

struct work_queue_s {
    int end;
    size_t size;
    size_t waiting;
    pthread_mutex_t mutex;
    pthread_cond_t cond_get;
    pthread_cond_t cond_put;
    struct queue_item *first;
    struct queue_item *last;
};

void work_queue_init(struct work_queue_s * const);
void work_queue_end(struct work_queue_s * const);
void *work_queue_pop(struct work_queue_s * const);
size_t work_queue_pending(struct work_queue_s * const);
size_t work_queue_waiting(struct work_queue_s * const);
void work_queue_push(struct work_queue_s * const, void *);
void work_queue_finish(struct work_queue_s * const, void (*cb)(void *));

#endif /* queue.h */
