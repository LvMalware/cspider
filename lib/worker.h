#ifndef _WORKER_H
#define _WORKER_H

#include "memdebug.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "list.h"
#include "queue.h"
#include "utils.h"
#include "vector.h"
#include "siphash.h"

struct task_info_s {
    char *url;
    int depth;
};

struct worker_context_s {
    FILE *output;
    int max_depth;
    size_t workers;
    pthread_t *threads;
    uint8_t hash_key[16];
    pthread_mutex_t mutex;
    struct list_head_s scope;
    struct hash_vector_s hashes;
    struct work_queue_s work_pool;
};

void worker_context_wait(struct worker_context_s * const);
void worker_context_finish(struct worker_context_s * const);
void worker_context_start(struct worker_context_s * const, size_t);
void worker_add_work(struct worker_context_s * const, char * const, int);
void worker_context_init(struct worker_context_s * const, int, FILE * const);

#endif /* worker.h */
