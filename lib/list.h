#ifndef _LIST_H
#define _LIST_H

#include "memdebug.h"

#include <stdlib.h>
#include <string.h>

struct list_item_s {
    void *data;
    size_t size;
    struct list_item_s *next;
};

struct list_head_s {
    size_t length;
    struct list_item_s *first;
    struct list_item_s *last;
};

void list_head_init(struct list_head_s * const);
void list_head_push(struct list_head_s * const, void * const, size_t const);
void list_head_pop(struct list_head_s * const, void ** const, size_t * const);
void list_head_finish(struct list_head_s * const, void (*callback) (void *));

#endif /* list.h */
