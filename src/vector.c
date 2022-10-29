/*
 * Copyright 2022 Lucas V. Araujo (https://github.com/lvmalware)
 *
 * This file is part of cspider.
 * RefFinder is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * RefFinder is distributed in the hope that it will be useful,but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with RefFinder. If not, see <https://www.gnu.org/licenses/>.
 */

#include "vector.h"

static inline int vector_ensure_space(struct hash_vector_s * const vec, size_t space_needed) {
    if (vec->capacity <= space_needed) {
        vec->capacity <<= 1;
        void *temp = realloc(vec->array, vec->capacity * sizeof(vec->array[0]));
        if (temp == NULL) return 0;
        vec->array = temp;
    }
    return 1;
}

static inline void vector_shrink_space(struct hash_vector_s * const vec) {
    if (vec->capacity > (vec->size << 2) && vec->capacity > MIN_ARRAY_SIZE) {
        vec->capacity >>= 1;
        vec->array = realloc(vec->array, vec->capacity * sizeof(vec->array[0]));
    }
}

void hash_vector_finish(struct hash_vector_s * const vec) {
    if (vec == NULL) return;
    free(vec->array);
    vec->array = NULL;
    vec->size  = vec->capacity = 0;
}

void hash_vector_init(struct hash_vector_s * const vec) {
    if (vec == NULL) return;
    vec->array = malloc(MIN_ARRAY_SIZE * sizeof(vec->array[0]));
    if (vec->array == NULL) return;
    vec->size = 0;
    vec->capacity = MIN_ARRAY_SIZE;
    pthread_mutex_init(&(vec->mutex), NULL);
}

static inline size_t insert_index(HASH * const array, size_t const size, HASH const h) {
    if (size == 0 || array[0] >= h) return 0;
    if (array[size - 1] < h) return size;
    size_t s = 0, e = size - 1;
    while (s != e) {
        size_t m = (s + e) / 2;
        if (array[m] == h) {
            return m;
        } else if (array[m] > h) {
            e = m - 1;
        } else {
            s = m + 1;
        }
        if ((s+1) == e) break;
    }

    while (array[s] < h) s ++;
    return s;
}

int hash_vector_insert(struct hash_vector_s * const vec, HASH h) {
    int success = 0;
    pthread_mutex_lock(&(vec->mutex));

    if (vec->array && vector_ensure_space(vec, vec->size + 1)) {
        size_t m = insert_index(vec->array, vec->size, h);
        if (vec->array[m] != h) {
            size_t j = vec->size;
            while (j > m) {
                vec->array[j] = vec->array[j - 1];
                j --;
            }
            vec->array[m] = h;
            vec->size ++;
            success = 1;
        }
    }

    pthread_mutex_unlock(&(vec->mutex));
    return success;
}
