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

#include "list.h"

void list_head_init(struct list_head_s * const head) {
    head->length = 0;
    head->first = head->last = NULL;
}

void list_head_push(struct list_head_s * const head, void * const data, size_t const size) {
    struct list_item_s *item = malloc(1 * sizeof(*item));
    if (item == NULL) return;
    item->data = data;
    item->size = size;
    item->next = NULL;
    if (head->last) {
        head->last->next = item;
        head->last = item;
    } else {
        head->last = head->first = item;
    }
    head->length += 1;
}

void list_head_pop(struct list_head_s * const head, void ** const data, size_t * const size) {
    if (head->first == NULL) {
        *size = 0;
        *data = NULL;
        return;
    }
    struct list_item_s *item = head->first;
    head->first = item->next;
    if (head->first == NULL) head->last = NULL;
    *data = item->data;
    *size = item->size;
    free(item);
    head->length -= 1;
}

void list_head_finish(struct list_head_s * const head, void (*callback) (void *)) {
    struct list_item_s *next, *item = head->first;
    while (item) {
        next = item->next;
        if (callback) callback(item->data);
        item->data = NULL;
        item->size = 0;
        free(item);
        item = next;
    }
    head->first = head->last = NULL;
    head->length = 0;
}
