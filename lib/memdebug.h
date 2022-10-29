#ifndef _MEMDEBUG_H
#define _MEMDEBUG_H

#include <stdio.h>
#include <stdlib.h>

#ifdef MEMDEBUG

#define free(p) { printf("%s:%s:%d: free(%p)\n", __FILE__, __func__, __LINE__, p); free(p); }
#define malloc(s) malloc(s); printf("%s:%s:%d: malloc(%lu)\n", __FILE__, __func__, __LINE__, s);
#define calloc(n, s) calloc(n, s); printf("%s:%s:%d: calloc(%u, %lu)\n", __FILE__, __func__, __LINE__, n, s);
#define realloc(p, s) realloc(p, s); printf("%s:%s:%d: realloc(%p, %lu)\n", __FILE__, __func__, __LINE__, p, s);

#endif /* debug */
#endif /* memdebug.h */
