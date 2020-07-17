/* MIT License
 *
 * Copyright (c) 2020 Tyge Løvset, NORCE, www.norceresearch.no
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef CRANDOM__H__
#define CRANDOM__H__

#include "cdefs.h"
#include <string.h>

typedef struct {uint64_t state;  uint64_t inc;} pcg32_random_t;

/* 32 bit random number generator */
STC_INLINE uint32_t pcg32_random(pcg32_random_t* rng)
{
    uint64_t old = rng->state;
    rng->state = old * 6364136223846793005ull + rng->inc;
    uint32_t xos = ((old >> 18u) ^ old) >> 27u;
    uint32_t rot = old >> 59u;
    return (xos >> rot) | (xos << ((-rot) & 31));
}

/* float random int number in range [0, 1). NB: 23 bit resolution. */
STC_INLINE float pcg32_randomFloat(pcg32_random_t* rng) {
    union {uint32_t i; float f;} u = {0x3F800000u | (pcg32_random(rng) >> 9)};
    return u.f - 1.0f;
}

/* Uniform random number in range [0, bound) */
STC_INLINE uint32_t pcg32_randomBounded(pcg32_random_t* rng, uint32_t bound) {
    return (uint32_t) (((uint64_t) pcg32_random(rng) * bound) >> 32);
}

STC_INLINE pcg32_random_t pcg32_seed(uint64_t seed, uint64_t seq) {
    pcg32_random_t rng = {0u, (seq << 1u) | 1u}; /* inc must be odd */
    pcg32_random(&rng);
    rng.state += seed;
    pcg32_random(&rng);
    return rng;
}

/* 
 * Rotate bits left
 */
STC_INLINE uint64_t c_rotateLeft64(uint64_t x, int bits) {
    return (x << bits) | (x >> (64 - bits));
}

/*
 * sfc64: http://pracrand.sourceforge.net
 */
typedef struct {uint64_t state[4];} sfc64_random_t;

/* 64 bit random number generator */
STC_API uint64_t sfc64_random(sfc64_random_t* rng) {
    enum {LR=24, RS=11, LS=3};
    uint64_t *s = rng->state;
    const uint64_t result = s[0] + s[1] + s[3]++;
    s[0] = s[1] ^ (s[1] >> RS);
    s[1] = s[2] + (s[2] << LS);
    s[2] = c_rotateLeft64(s[2], LR) + result;
    return result;
}

/* double random int number in range [0, 1). */
STC_INLINE double sfc64_randomFloat(sfc64_random_t* rng) {
    union {uint64_t i; double f;} u = {0x3FF0000000000000ull | (sfc64_random(rng) >> 12)};
    return u.f - 1.0;
}

STC_API sfc64_random_t sfc64_seed(const uint64_t seed) {
    sfc64_random_t state = {{seed, seed, seed, 1}};
    for (int i = 0; i < 12; ++i) sfc64_random(&state);
    return state;
}

/*
 * SipHash implementation.
 */
#if defined(_WIN32) || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    STC_INLINE uint64_t c_le64ToHost(uint64_t x) { return x; }
#elif defined(__APPLE__)
    #include <libkern/OSByteOrder.h>
    STC_INLINE uint64_t c_le64ToHost(uint64_t x) { return OSSwapLittleToHostInt64(x); }
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
    #include <sys/endian.h>
    STC_INLINE uint64_t c_le64ToHost(uint64_t x) { return letoh64(x); }
#elif defined(__linux__) || defined(__CYGWIN__) || defined(__GNUC__) || defined(__GNU_LIBRARY__)
    #include <endian.h>
    STC_INLINE uint64_t c_le64ToHost(uint64_t x) { return le64toh(x); }
#endif

typedef struct siphash_t {
    uint64_t v[4], padding;
    size_t length;
    int c, d;
} siphash_t;

/* c=2, d=4 or c=1, d=3 */
STC_INLINE void siphash_init_c_d(siphash_t* s, const uint64_t key[2], const int c, const int d) {
    s->c = c;
    s->d = d;
    s->length = 0;
    s->padding = 0;
    s->v[0] = key[0] ^ 0x736f6d6570736575;
    s->v[1] = key[1] ^ 0x646f72616e646f6d;
    s->v[2] = key[0] ^ 0x6c7967656e657261;
    s->v[3] = key[1] ^ 0x7465646279746573;
}

/* default init 2-4 */
STC_API siphash_t siphash_init(const uint64_t key[2]) {
    siphash_t state;
    siphash_init_c_d(&state, key, 2, 4);
    return state;
}

#define _siphash_halfRound(i, j, a, b, c, d) \
    (a += b, \
     c += d, \
     b = c_rotateLeft64(b, i) ^ a, \
     d = c_rotateLeft64(d, j) ^ c, \
     a = c_rotateLeft64(a, 32))

#define _siphash_compress(rounds, v) \
    for (int r = 0; r < rounds; ++r) { \
        _siphash_halfRound(13, 16, v[0], v[1], v[2], v[3]); \
        _siphash_halfRound(17, 21, v[2], v[1], v[0], v[3]); \
    }

#define _siphash_digest(rounds, v, m) { \
        const uint64_t _m = m; \
        v[3] ^= _m; \
        _siphash_compress(rounds, v); \
        v[0] ^= _m; \
    }

STC_API void siphash_update(siphash_t* s, const void* bytes, size_t size) {
    union { const uint8_t* u8; const uint64_t* u64; } in;
    in.u8 = (const uint8_t*) bytes;
    size_t offset = s->length & 7;
    uint64_t *v = s->v;
    s->length += size;

    if (offset) {
        const size_t end = offset + size;
        size -= 8 - offset;
        while (offset < end && offset < 8) {
            s->padding |= ((uint64_t) *in.u8++) << (offset++ << 3);
        }
        if (end < 8) return;

        _siphash_digest(s->c, v, s->padding);
        s->padding = 0;
    }
    size_t n_words = size >> 3;
    uint64_t m;

    while (n_words--) {
        memcpy(&m, in.u64++, 8);
        _siphash_digest(s->c, v, c_le64ToHost(m));
    }
    switch (s->length & 7) {
        case 7: s->padding |= ((uint64_t) in.u8[6]) << 48;
        case 6: s->padding |= ((uint64_t) in.u8[5]) << 40;
        case 5: s->padding |= ((uint64_t) in.u8[4]) << 32;
        case 4: s->padding |= ((uint64_t) in.u8[3]) << 24;
        case 3: s->padding |= ((uint64_t) in.u8[2]) << 16;
        case 2: s->padding |= ((uint64_t) in.u8[1]) << 8;
        case 1: s->padding |= ((uint64_t) in.u8[0]);
    }
}

STC_API uint64_t siphash_finalize(siphash_t* s) {
    uint64_t *v = s->v;
    _siphash_digest(s->c, v, s->padding | (s->length << 56));
    v[2] ^= 0xff;
    _siphash_compress(s->d, v);
    return v[0] ^ v[1] ^ v[2] ^ v[3];
}

/* c=2, d=4 or c=1, d=3 */
STC_API uint64_t siphash_hash_c_d(const uint64_t key[2], const void* bytes, const uint64_t size, const int c, const int d) {
    siphash_t state;
    siphash_init_c_d(&state, key, c, d);
    siphash_update(&state, bytes, size);
    return siphash_finalize(&state);
}

/* default hash 2-4 */
STC_INLINE uint64_t siphash_hash(const uint64_t key[2], const void* bytes, const uint64_t size) {
    return siphash_hash_c_d(key, bytes, size, 2, 4);
}

#endif
