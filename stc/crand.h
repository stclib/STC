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

/*
// crand: Pseudo-random number generator
#include "stc/crand.h"
int main() {
    uint64_t seed = 123456789;
    crand_t rng = crand_init(seed);
    crand_uniform_t dist1 = crand_uniform_init(1, 6);
    crand_uniformf_t dist2 = crand_uniformf_init(1.0, 10.0);
    crand_normalf_t dist3 = crand_normalf_init(1.0, 10.0);

    uint64_t i = crand_next(&rng);
    int64_t iu = crand_uniform(&rng, &dist1);
    double xu = crand_uniformf(&rng, &dist2);
    double xn = crand_normalf(&rng, &dist3);
}
*/
#include "ccommon.h"
#include <string.h>
#include <math.h>

typedef struct {uint64_t state[4];}                        crand_t;
typedef struct {int64_t lower; uint64_t range, threshold;} crand_uniform_t;
typedef struct {double lower, range;}                      crand_uniformf_t;
typedef struct {double mean, stddev, next; bool has_next;} crand_normalf_t;


/* int random number generator, range [0, 2^64). PRNG copyright Tyge Løvset, NORCE Research, 2020 */
STC_API crand_t crand_init(uint64_t seed);
STC_API crand_t crand_with_seq(uint64_t seed, uint64_t seq);

STC_INLINE uint64_t crand_next(crand_t* rng) {
    enum {LROT = 24, RSHIFT = 11, LSHIFT = 3};
    uint64_t *s = rng->state;
    const uint64_t b = s[1], result = s[0] ^ (s[2] += s[3]|1);
    s[0] = (b + (b << LSHIFT)) ^ (b >> RSHIFT);
    s[1] = ((b << LROT) | (b >> (64 - LROT))) + result;
    return result;
}

/* double random number in range [low, high). */
STC_INLINE double crand_nextf(crand_t* rng) {
    union {uint64_t i; double f;} u = {0x3FF0000000000000ull | (crand_next(rng) >> 12)};
    return u.f - 1.0;
}

/* integer uniform distributed RNG, range [low, high]. */
STC_API crand_uniform_t crand_uniform_init(int64_t low, int64_t high);

/* double uniform distributed RNG, range [low, high). */
STC_INLINE crand_uniformf_t crand_uniformf_init(double low, double high) {
    crand_uniformf_t dist = {low, high - low}; return dist;
}
STC_INLINE double crand_uniformf(crand_t* rng, crand_uniformf_t* dist) {
    return crand_nextf(rng)*dist->range + dist->lower;
}

#if defined(__SIZEOF_INT128__)
    #define cmul128(a, b, lo, hi) \
        do { __uint128_t _z = (__uint128_t)(a) * (b); \
             *(lo) = (uint64_t)_z, *(hi) = _z >> 64; } while(0)
#elif defined(_MSC_VER) && defined(_WIN64)
    #include <intrin.h>
    #define cmul128(a, b, lo, hi) (*(lo) = _umul128(a, b, hi), (void)0)
#elif defined(__x86_64__)
    #define cmul128(a, b, lo, hi) \
        asm("mulq %[rhs]" : "=a" (*(lo)), "=d" (*(hi)) \
                          : [lhs] "0" (a), [rhs] "rm" (b))
#endif

STC_INLINE int64_t crand_uniform(crand_t* rng, crand_uniform_t* d) {
    uint64_t lo, hi;
    do { cmul128(crand_next(rng), d->range, &lo, &hi); } while (lo < d->threshold);
    return d->lower + hi;
}

/* double normal distributed RNG. */
STC_INLINE crand_normalf_t crand_normalf_init(double mean, double stddev) {
    crand_normalf_t dist = {mean, stddev, 0.0, false}; return dist;
}
STC_API double crand_normalf(crand_t* rng, crand_normalf_t* dist);


#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)

/* PRNG crand: by Tyge Løvset, NORCE Research, 2020.
 * Extremely fast PRNG suited for parallel usage with Weyl-sequence parameter.
 * Faster than sfc64, wyhash64, and almost 50% faster than xoshiro256** on gcc.
 * 256bit state, updates only 192bit per rng.
 * 2^63 unique threads with a minimum 2^64 period lengths each.
 * 2^127 minimum period length for single thread (double loop).
 * Passes PractRand tested up to 8TB output, Vigna's Hamming weight test,
 * and simple correlation tests, i.e. interleaved streams with one-bit diff state.
 */

STC_DEF crand_t crand_init(uint64_t seed) {
    return crand_with_seq(seed, 0x3504f333d3aa0b34);
}
STC_DEF crand_t crand_with_seq(uint64_t seed, uint64_t seq) {
    crand_t rng = {{seed, seed, seed, (seq << 1u) | 1u}};
    for (int i = 0; i < 8; ++i) crand_next(&rng);
    return rng;
}

/* Very fast unbiased uniform int RNG with bounds [low, high] */
STC_DEF crand_uniform_t crand_uniform_init(int64_t low, int64_t high) {
    crand_uniform_t dist = {low, (uint64_t) (high - low + 1)};
    dist.threshold = (uint64_t)(-dist.range) % dist.range;
    return dist;
}

/* Marsaglia polar method for gaussian/normal distribution. */
STC_DEF double crand_normalf(crand_t* rng, crand_normalf_t* dist) {
    double u1, u2, s, m;
    if (dist->has_next) {
        dist->has_next = false;
        return dist->next * dist->stddev + dist->mean;
    }
    do {
        u1 = 2.0 * crand_nextf(rng) - 1.0;
        u2 = 2.0 * crand_nextf(rng) - 1.0;
        s = u1*u1 + u2*u2;
    } while (s >= 1.0 || s == 0.0);
    m = sqrt(-2.0 * log(s) / s);
    dist->next = u2 * m, dist->has_next = true;
    return (u1 * m) * dist->stddev + dist->mean;
}

#endif
#endif
