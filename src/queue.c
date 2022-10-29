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

#include "queue.h"

void work_queue_init(struct work_queue_s * const q) {
    q->end = 0;
    q->size = 0;
    q->waiting = 0;
    q->last = NULL;
    q->first = NULL;
    pthread_mutex_init(&(q->mutex), NULL);
    pthread_cond_init(&(q->cond_get), NULL);
    pthread_cond_init(&(q->cond_put), NULL);
}

void work_queue_end(struct work_queue_s * const q) {
    pthread_mutex_lock(&(q->mutex));
    q->end = 1;
    pthread_cond_broadcast(&(q->cond_put));
    pthread_mutex_unlock(&(q->mutex));
}

size_t work_queue_pending(struct work_queue_s * const q) {
    pthread_mutex_lock(&(q->mutex));
    size_t size = q->size;
    pthread_mutex_unlock(&(q->mutex));
    return size;
}

size_t work_queue_waiting(struct work_queue_s * const q) {
    pthread_mutex_lock(&(q->mutex));
    size_t waiting = q->waiting;
    pthread_mutex_unlock(&(q->mutex));
    return waiting;
}

void *work_queue_pop(struct work_queue_s * const q) {
    pthread_mutex_lock(&(q->mutex));
    while (q->size == 0) {
        if (q->end) {
            pthread_mutex_unlock(&(q->mutex));
            return NULL;
        }
        q->waiting ++;
        pthread_cond_wait(&(q->cond_put), &(q->mutex));
        q->waiting --;
    }
        
    struct queue_item *first = q->first;
    q->first = first->next;
    if (q->first == NULL) q->last = NULL;
    q->size --;

    pthread_cond_signal(&(q->cond_get));
    pthread_mutex_unlock(&(q->mutex));

    void *data = first->data;
    free(first);
    return data;
}

void work_queue_finish(struct work_queue_s * const q, void (*cb)(void *)) {
    if (!q) return;
    struct queue_item *next, *item = q->first;
    while (item) {
        next = item->next;
        if (cb) cb(item->data);
        free(item);
        item = next;
    }
}

void work_queue_push(struct work_queue_s * const q, void *data) {
    struct queue_item *item = malloc(1 * sizeof(*item));
    item->data = data;
    item->next = NULL;
    pthread_mutex_lock(&(q->mutex));
    while (q->size > QUEUE_MAX_PENDING) pthread_cond_wait(&(q->cond_get), &(q->mutex));
    if (q->last == NULL) {
        q->first = item;
    } else {
        q->last->next = item;
    }
    q->last = item;
    q->size ++;
    pthread_cond_broadcast(&(q->cond_put));
    pthread_mutex_unlock(&(q->mutex));
}
