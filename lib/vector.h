#ifndef _VECTOR_H
#define _VECTOR_H

#include "memdebug.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>

#define HASH uint64_t
#define MIN_ARRAY_SIZE 0xff

struct hash_vector_s {
    size_t size;
    size_t capacity;
    pthread_mutex_t mutex;
    HASH *array;
};


void hash_vector_init(struct hash_vector_s * const);
void hash_vector_finish(struct hash_vector_s * const);
int hash_vector_insert(struct hash_vector_s * const, HASH);

#endif /* vector.h */
