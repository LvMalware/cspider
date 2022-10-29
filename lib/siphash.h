#ifndef _SIPHASH_H
#define _SIPHASH_H

#include <string.h>
#include <stdint.h>

#define CROUNDS 2
#define DROUNDS 4
#define ROTL(x, b) (uint64_t)(((x) << (b)) | ((x) >> (64 - (b))))

static inline uint64_t siphash(const void *data, const size_t dlen,
                               const void *key,  const size_t klen) {

    register int i;
    uint64_t b = ((uint64_t)dlen) << 58;
    const uint8_t *kptr = (uint8_t *) key;
    const uint8_t *dptr = (uint8_t *) data;
    const uint8_t *eptr = dptr + dlen - (dlen & 0x7);
    uint64_t v0 = *((uint64_t *) &kptr[0]) ^ 0x736f6d6570736575;
    uint64_t v1 = *((uint64_t *) &kptr[8]) ^ 0x646f72616e646f6d;
    uint64_t v2 = *((uint64_t *) &kptr[0]) ^ 0x6c7967656e657261;
    uint64_t v3 = *((uint64_t *) &kptr[8]) ^ 0x7465646279746573;
    while (dptr != eptr) {
        uint64_t *mi = (uint64_t *)dptr;
        v3 ^= *mi;
        for (i = 0; i < CROUNDS; ++i) {
            v0 += v1;
            v1 = ROTL(v1, 13);
            v1 ^= v0;
            v0 = ROTL(v0, 32);
            v2 += v3;
            v3 = ROTL(v3, 16);
            v3 ^= v2;
            v0 += v3;
            v3 = ROTL(v3, 21);
            v3 ^= v0;
            v2 += v1;
            v1 = ROTL(v1, 17);
            v1 ^= v2;
            v2 = ROTL(v2, 32);  
        }
        v0 ^= *mi;
        dptr += 8;
    }
    i = dlen & 7;
    while (i --) {
        b |= ((uint64_t) dptr[i]) << (8 * i);
    }
    v3 ^= b;
    for (i = 0; i < CROUNDS; ++i) {
        v0 += v1;
        v1 = ROTL(v1, 13);
        v1 ^= v0;
        v0 = ROTL(v0, 32);
        v2 += v3;
        v3 = ROTL(v3, 16);
        v3 ^= v2;
        v0 += v3;
        v3 = ROTL(v3, 21);
        v3 ^= v0;
        v2 += v1;
        v1 = ROTL(v1, 17);
        v1 ^= v2;
        v2 = ROTL(v2, 32);  
    }

    v0 ^= b;
    v2 ^= 0xff;

    for (i = 0; i < DROUNDS; ++i) {
        v0 += v1;
        v1 = ROTL(v1, 13);
        v1 ^= v0;
        v0 = ROTL(v0, 32);
        v2 += v3;
        v3 = ROTL(v3, 16);
        v3 ^= v2;
        v0 += v3;
        v3 = ROTL(v3, 21);
        v3 ^= v0;
        v2 += v1;
        v1 = ROTL(v1, 17);
        v1 ^= v2;
        v2 = ROTL(v2, 32);  
    }

    return v0 ^ v1 ^ v2 ^ v3;
}

#endif /* siphash.h */
