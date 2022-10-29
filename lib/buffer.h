#ifndef _BUFFER_H
#define _BUFFER_H

#include <string.h>
#include <stdlib.h>

#include "memdebug.h"

struct string_buffer_s {
    char *buff;
    size_t size;
};

static inline void string_buffer_init(struct string_buffer_s * const sb) {
    sb->size = 0;
    sb->buff = malloc(1 * sizeof(*(sb->buff)));
    sb->buff[0] = '\0';
}

static inline void string_buffer_finish(struct string_buffer_s * const sb) {
    free(sb->buff);
    sb->buff = NULL;
    sb->size = 0;
}

static inline void string_buffer_concat(struct string_buffer_s * const sb, char * const str, size_t len) {
    char *ptr = realloc(sb->buff, (sb->size + len + 1) * sizeof(*str));
    if (ptr == NULL) return;
    sb->buff = ptr;
    memcpy(&(sb->buff[sb->size]), str, len);
    sb->size = sb->size + len;
    sb->buff[sb->size] = '\0';
}

#endif /* buffer.h */
