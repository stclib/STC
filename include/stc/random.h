/* MIT License
 *
 * Copyright (c) 2025 Tyge Løvset
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
#define i_header // external linkage of normal_dist by default.
#include "priv/linkage.h"

#ifndef STC_RANDOM_H_INCLUDED
#define STC_RANDOM_H_INCLUDED

#include "common.h"

// ===== crand64 ===================================

typedef struct {
    uint64_t data[4];
} crand64;

typedef struct {
    double mean, stddev;
    double _next;
    int _has_next;
} crand64_normal_dist;

STC_API double crand64_normal(crand64_normal_dist* d);
STC_API double crand64_normal_r(crand64* rng, uint64_t stream, crand64_normal_dist* d);

#if INTPTR_MAX == INT64_MAX
  #define crandWS crand64
#else
  #define crandWS crand32
#endif

#define c_shuffle_seed(s) \
    c_JOIN(crandWS, _seed)(s)

#define c_shuffle_array(array, n) do { \
    typedef struct { char d[sizeof 0[array]]; } _etype; \
    _etype* _arr = (_etype *)(array); \
    for (isize _i = (n) - 1; _i > 0; --_i) { \
        isize _j = (isize)(c_JOIN(crandWS, _uint)() % (_i + 1)); \
        c_swap(_arr + _i, _arr + _j); \
    } \
} while (0)

// Compiles with vec, stack, and deque container types:
#define c_shuffle(CntType, self) do { \
    CntType* _self = self; \
    for (isize _i = CntType##_size(_self) - 1; _i > 0; --_i) { \
        isize _j = (isize)(c_JOIN(crandWS, _uint)() % (_i + 1)); \
        c_swap(CntType##_at_mut(_self, _i), CntType##_at_mut(_self, _j)); \
    } \
} while (0)

STC_INLINE void crand64_seed_r(crand64* rng, uint64_t seed) {
    uint64_t* s = rng->data;
    s[0] = seed*0x9e3779b97f4a7c15; s[0] ^= s[0] >> 30;
    s[1] = s[0]*0xbf58476d1ce4e5b9; s[1] ^= s[1] >> 27;
    s[2] = s[1]*0x94d049bb133111eb; s[2] ^= s[2] >> 31;
    s[3] = seed;
}

// Minimum period length 2^64 per stream. 2^63 streams (odd numbers only)
STC_INLINE uint64_t crand64_uint_r(crand64* rng, uint64_t stream) {
    uint64_t* s = rng->data;
    const uint64_t result = (s[0] ^ (s[3] += stream)) + s[1];
    s[0] = s[1] ^ (s[1] >> 11);
    s[1] = s[2] + (s[2] << 3);
    s[2] = ((s[2] << 24) | (s[2] >> 40)) + result;
    return result;
}

STC_INLINE double crand64_real_r(crand64* rng, uint64_t stream)
    { return (double)(crand64_uint_r(rng, stream) >> 11) * 0x1.0p-53; }

STC_INLINE crand64* _stc64(void) {
    static crand64 rng = {{0x9e3779bb07979af0,0x6f682616bae3641a,0xe220a8397b1dcdaf,0x1}};
    return &rng;
}

STC_INLINE void crand64_seed(uint64_t seed)
    { crand64_seed_r(_stc64(), seed); }

STC_INLINE crand64 crand64_from(uint64_t seed)
    { crand64 rng; crand64_seed_r(&rng, seed); return rng; }

STC_INLINE uint64_t crand64_uint(void)
    { return crand64_uint_r(_stc64(), 1); }

STC_INLINE double crand64_real(void)
    { return crand64_real_r(_stc64(), 1); }

// --- crand64_uniform ---

typedef struct {
    int64_t low;
    uint64_t range, threshold;
} crand64_uniform_dist;

STC_INLINE crand64_uniform_dist
crand64_make_uniform(int64_t low, int64_t high) {
    crand64_uniform_dist d = {low, (uint64_t)(high - low + 1)};
    d.threshold = (uint64_t)(0 - d.range) % d.range;
    return d;
}

// 128-bit multiplication
#if defined(__SIZEOF_INT128__)
    #define c_umul128(a, b, lo, hi) \
        do { __uint128_t _z = (__uint128_t)(a)*(b); \
             *(lo) = (uint64_t)_z, *(hi) = (uint64_t)(_z >> 64U); } while(0)
#elif defined(_MSC_VER) && defined(_WIN64)
    #include <intrin.h>
    #define c_umul128(a, b, lo, hi) ((void)(*(lo) = _umul128(a, b, hi)))
#elif defined(__x86_64__)
    #define c_umul128(a, b, lo, hi) \
        asm("mulq %3" : "=a"(*(lo)), "=d"(*(hi)) : "a"(a), "rm"(b))
#endif

STC_INLINE int64_t
crand64_uniform_r(crand64* rng, uint64_t stream, crand64_uniform_dist* d) {
    uint64_t lo, hi;
    #ifdef c_umul128
        do { c_umul128(crand64_uint_r(rng, stream), d->range, &lo, &hi); } while (lo < d->threshold);
    #else
        do { lo = crand64_uint_r(rng, stream); hi = lo % d->range; } while (lo - hi > -d->range);
    #endif
    return d->low + (int64_t)hi;
}

STC_INLINE int64_t crand64_uniform(crand64_uniform_dist* d)
    { return crand64_uniform_r(_stc64(), 1, d); }

// ===== crand32 ===================================

typedef struct { uint32_t data[4]; } crand32;

STC_INLINE void crand32_seed_r(crand32* rng, uint32_t seed) {
    uint32_t* s = rng->data;
    s[0] = seed*0x9e3779b9; s[0] ^= s[0] >> 16;
    s[1] = s[0]*0x21f0aaad; s[1] ^= s[1] >> 15;
    s[2] = s[1]*0x735a2d97; s[2] ^= s[2] >> 15;
    s[3] = seed;
}

// Minimum period length 2^32 per stream. 2^31 streams (odd numbers only)
STC_INLINE uint32_t crand32_uint_r(crand32* rng, uint32_t stream) {
    uint32_t* s = rng->data;
    const uint32_t result = (s[0] ^ (s[3] += stream)) + s[1];
    s[0] = s[1] ^ (s[1] >> 9);
    s[1] = s[2] + (s[2] << 3);
    s[2] = ((s[2] << 21) | (s[2] >> 11)) + result;
    return result;
}

STC_INLINE double crand32_real_r(crand32* rng, uint32_t stream)
    { return crand32_uint_r(rng, stream) * 0x1.0p-32; }

STC_INLINE crand32* _stc32(void) {
    static crand32 rng = {{0x9e37e78e,0x6eab1ba1,0x64625032,0x1}};
    return &rng;
}

STC_INLINE void crand32_seed(uint32_t seed)
    { crand32_seed_r(_stc32(), seed); }

STC_INLINE crand32 crand32_from(uint32_t seed)
    { crand32 rng; crand32_seed_r(&rng, seed); return rng; }

STC_INLINE uint32_t crand32_uint(void)
    { return crand32_uint_r(_stc32(), 1); }

STC_INLINE double crand32_real(void)
    { return crand32_real_r(_stc32(), 1); }

// --- crand32_uniform ---

typedef struct {
    int32_t low;
    uint32_t range, threshold;
} crand32_uniform_dist;

STC_INLINE crand32_uniform_dist
crand32_make_uniform(int32_t low, int32_t high) {
    crand32_uniform_dist d = {low, (uint32_t)(high - low + 1)};
    d.threshold = (uint32_t)(0 - d.range) % d.range;
    return d;
}

STC_INLINE int32_t
crand32_uniform_r(crand32* rng, uint32_t stream, crand32_uniform_dist* d) {
    uint64_t r;
    do {
        r = crand32_uint_r(rng, stream) * (uint64_t)d->range;
    } while ((uint32_t)r < d->threshold);
    return d->low + (int32_t)(r >> 32);
}

STC_INLINE int64_t crand32_uniform(crand32_uniform_dist* d)
    { return crand32_uniform_r(_stc32(), 1, d); }

#endif // STC_RANDOM_H_INCLUDED

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined i_implement && !defined STC_RANDOM_IMPLEMENT
#define STC_RANDOM_IMPLEMENT
#include <math.h>

STC_DEF double
crand64_normal_r(crand64* rng, uint64_t stream, crand64_normal_dist* d) {
    double v1, v2, sq, rt;
    if (d->_has_next++ & 1)
        return d->_next*d->stddev + d->mean;
    do {
        // range (-1.0, 1.0):
        v1 = (double)((int64_t)crand64_uint_r(rng, stream) >> 11) * 0x1.0p-52;
        v2 = (double)((int64_t)crand64_uint_r(rng, stream) >> 11) * 0x1.0p-52;

        sq = v1*v1 + v2*v2;
    } while (sq >= 1.0 || sq == 0.0);
    rt = sqrt(-2.0 * log(sq) / sq);
    d->_next = v2*rt;
    return (v1*rt)*d->stddev + d->mean;
}

STC_DEF double crand64_normal(crand64_normal_dist* d)
    { return crand64_normal_r(_stc64(), 1, d); }

#endif // IMPLEMENT
#include "priv/linkage2.h"
