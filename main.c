#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

#include "worker.h"

void fatal_error(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    exit(-1);
}

void help_message(char *prog) {
    printf(
            "%s - Webcrawler/spider for security tests\n\n"
            "Usage: %s [option(s)]\n"
            "Options:\n"
            "   -h          Show this help message and exit.\n"
            "   -i          Input file with URLs\n"
            "   -s          Add domain to scope\n"
            "   -d          Maximum depth to crawl\n"
            "   -o          Output file to log results\n"
            "   -t          Number of concurrent threads\n\n"
			"By Lucas V. Araujo <https://github.com/lvmalware>\n",
            prog, prog
    );
    exit(0);
}

int main(int argc, char *argv[]) {
    int depth = 4;
    int threads = 16;
    FILE *input = stdin;
    FILE *output = stdout;
    struct worker_context_s ctx;
    worker_context_init(&ctx, depth, output);

    int option;
    while ((option = getopt(argc, argv, "hi:s:d:o:t:"))  != -1) {
        switch (option) {
            case 'i':
                fclose(input);
                input = fopen(optarg, "r");
                if (!input)
                    fatal_error("Can't open %s for reading.\n", optarg);
                break;
            case 's':
                list_head_push(&(ctx.scope), optarg, strlen(optarg));
                break;
            case 'd':
                depth = atoi(optarg);
                if (depth < 1)
                    fatal_error("Invalid depth: %s\n", optarg);
                break;
            case 'o':
                fclose(output);
                output = fopen(optarg, "w");
                if (!output)
                    fatal_error("Can't open %s for writing.\n", optarg);
                break;
            case 't':
                threads = atoi(optarg);
                if (threads < 1)
                    fatal_error("Invalid number of threads: %s\n", optarg);
                break;
            case 'h':
                help_message(argv[0]);
        }
    }

    if (input == stdin)
        fprintf(stderr, "%s: Reading from standard input\n", argv[0]);

    ctx.output = output;
    ctx.max_depth = depth;
    worker_context_start(&ctx, threads);

    while (!feof(input)) {
        char *line = NULL;
        size_t len = 0;
        getline(&line, &len, input);
        if (len < 5) {
            free(line);
            continue;
        }
        len = strcspn(line, "\n\t ");
        line[len] = '\0';
        worker_add_work(&ctx, line, 0);
    }

    worker_context_wait(&ctx);
    worker_context_finish(&ctx);
    fclose(input);
    fclose(output);

    return 0;
}
