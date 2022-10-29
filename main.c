#include <stdio.h>
#include <stdarg.h>

#include "worker.h"

void fatal_error(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    exit(-1);
}

void help_message(char *prog) {
    printf(
            "%s - description\n\n"
            "Usage: %s [option(s)]\n"
            "Options:\n"
            "   -h          Show this help message and exit.\n"
            "   -i          Input file with URLs\n"
            "   -s          Add domain to scope\n"
            "   -d          Maximum depth to crawl\n"
            "   -o          Output file to log results\n"
            "   -t          Number of concurrent threads\n",
            prog, prog
    );
    exit(0);
}

#define WORKERS 10

int main(int argc, char *argv[]) {
    /* struct worker_context_s ctx;
    worker_context_init(&ctx, WORKERS, 2);
    list_head_push(&(ctx.scope), "scope", 10);
    worker_context_start(&ctx);
    worker_add_work(&ctx, strdup("url"), 0);
    worker_context_wait(&ctx);
    worker_context_finish(&ctx); */
    return 0;
}
