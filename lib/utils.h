#ifndef _UTILS_H
#define _UTILS_H

#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <curl/curl.h>
#include <libxml/uri.h>
#include <libxml/xpath.h>
#include <libxml/HTMLparser.h>

#include "list.h"
#include "queue.h"
#include "buffer.h"

int http_get(char * const, struct string_buffer_s * const);
void url_hostname(const char *, char ** const, size_t * const);
size_t crawl_over(char * const, struct list_head_s * const);

#endif /* utils.h */
