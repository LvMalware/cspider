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

#include "worker.h"

void worker_context_init(struct worker_context_s * const ctx,
                         size_t workers, int max_depth) {
    ctx->workers = workers;
    ctx->max_depth = max_depth;
    list_head_init(&(ctx->scope));
    hash_vector_init(&(ctx->hashes));
    work_queue_init(&(ctx->work_pool));
    pthread_mutex_init(&(ctx->mutex), NULL);
    memset(ctx->hash_key, 0x41, 0x10 * sizeof(uint8_t));
    ctx->threads = malloc(workers * sizeof(ctx->threads[0]));
}

void worker_context_finish(struct worker_context_s * const ctx) {
    hash_vector_finish(&(ctx->hashes));
    list_head_finish(&(ctx->scope), NULL);
    work_queue_finish(&(ctx->work_pool), NULL);
}

static inline int is_sub(char * const s1, const size_t l1, char * const s2, const size_t l2) {
    if (l2 > l1) return 0;
    size_t i = l1, j = l2;
    while (j) {
        if (s1[--i] != s2[--j]) return 0;
    }
    return 1;
}

static inline int in_scope(struct list_head_s * const scope, char * const url) {
    char *host = strstr(url, "://");
    if (!host) return 0;
    size_t size = strcspn(host += 3, ":/?");
    struct list_item_s *item = scope->first;
    while (item) {
        if (is_sub(host, size, item->data, item->size))
            return 1;
        item = item->next;
    }
    return 0;
}

void *worker_routine(void *arg) {
    struct worker_context_s *ctx = (struct worker_context_s *) arg;
    struct task_info_s *data;
    struct list_head_s list;
    list_head_init(&list);
    while ((data = work_queue_pop(&(ctx->work_pool))) != NULL) {
        crawl_over(data->url, &list);
        while (list.length > 0) {
            char *link;
            size_t len;
            list_head_pop(&list, (void **) &link, &len);
            if (link == NULL) continue;
            // len = strcpan(link, "?#");
            HASH h = siphash(link, len, ctx->hash_key, 0x10);
            if (in_scope(&(ctx->scope), link) &&
                    hash_vector_insert(&(ctx->hashes), h)) {
                pthread_mutex_lock(&(ctx->mutex));
                printf("%s\n", link);
                pthread_mutex_unlock(&(ctx->mutex));
                if ((data->depth + 1) < ctx->max_depth) {
                    worker_add_work(ctx, link, data->depth + 1);
                    continue;
                }
            }
            free(link);
        }

        free(data->url);
        free(data);
    }
    list_head_finish(&list, NULL);
    return NULL;
}

void worker_add_work(struct worker_context_s * const ctx,
                     char * const url, int depth) {
    struct task_info_s *task = malloc(1 * sizeof(*task));
    if (task) {
        task->url = url;
        task->depth = depth;
        work_queue_push(&(ctx->work_pool), task);
    }
}

void worker_context_start(struct worker_context_s * const ctx) {
    size_t i;
    for (i = 0; i < ctx->workers; i ++)
        pthread_create(&(ctx->threads[i]), NULL, worker_routine, ctx);
}

void worker_context_wait(struct worker_context_s * const ctx) {
    while (work_queue_waiting(&(ctx->work_pool)) < ctx->workers) {
        sleep(1);
    }
    work_queue_end(&(ctx->work_pool));
    size_t i;
    for (i = 0; i < ctx->workers; i ++)
        pthread_join(ctx->threads[i], NULL);
}
