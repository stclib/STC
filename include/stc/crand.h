/* MIT License
 *
 * Copyright (c) 2023 Tyge Løvset
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
#include "ccommon.h"

#ifndef CRAND_H_INCLUDED
#define CRAND_H_INCLUDED
/*
// crand: Pseudo-random number generator
#include "stc/crand.h"

int main() {
    uint64_t seed = 123456789;
    crand_t rng = crand_init(seed);
    crand_unif_t dist1 = crand_unif_init(1, 6);
    crand_norm_t dist3 = crand_norm_init(1.0, 10.0);

    uint64_t i = crand_u64(&rng);
    int64_t iu = crand_unif(&rng, &dist1);
    double xn = crand_norm(&rng, &dist3);
}
*/
#include <string.h>
#include <math.h>

typedef struct crand { uint64_t state[5]; } crand_t;
typedef struct crand_unif { int64_t lower; uint64_t range, threshold; } crand_unif_t;
typedef struct crand_norm { double mean, stddev, next; int has_next; } crand_norm_t;

/* PRNG crand_t.
 * Very fast PRNG suited for parallel usage with Weyl-sequence parameter.
 * 320-bit state, 256 bit is mutable.
 * Noticable faster than xoshiro and pcg, slighly slower than wyrand64 and
 * Romu, but these have restricted capacity for larger parallel jobs or unknown minimum periods.
 * crand_t supports 2^63 unique threads with a minimum 2^64 period lengths each.
 * Passes all statistical tests, e.g PractRand and correlation tests, i.e. interleaved
 * streams with one-bit diff state. Even the 16-bit version (LR=6, RS=5, LS=3) passes
 * PractRand to multiple TB input.
 */

/* Global crand_t PRNGs */
STC_API void csrand(uint64_t seed);
STC_API uint64_t crand(void);
STC_API double crandf(void);

/* Init crand_t prng with a seed */
STC_API crand_t crand_init(uint64_t seed);

/* Unbiased bounded uniform distribution. range [low, high] */
STC_API crand_unif_t crand_unif_init(int64_t low, int64_t high);
STC_API int64_t crand_unif(crand_t* rng, crand_unif_t* dist);

/* Normal/gaussian distribution. */
STC_INLINE crand_norm_t crand_norm_init(double mean, double stddev)
    { crand_norm_t r = {mean, stddev, 0.0, 0}; return r; }

STC_API double crand_norm(crand_t* rng, crand_norm_t* dist);

/* Main crand_t prng */
STC_INLINE uint64_t crand_u64(crand_t* rng) {
    uint64_t *s = rng->state;
    const uint64_t result = (s[0] ^ (s[3] += s[4])) + s[1];
    s[0] = s[1] ^ (s[1] >> 11);
    s[1] = s[2] + (s[2] << 3);
    s[2] = ((s[2] << 24) | (s[2] >> (64 - 24))) + result;
    return result;
}

/* Float64 random number in range [0.0, 1.0). */
STC_INLINE double crand_f64(crand_t* rng) {
    union {uint64_t i; double f;} u = {0x3FF0000000000000U | (crand_u64(rng) >> 12)};
    return u.f - 1.0;
}

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined(i_implement)

/* Global random() */
static crand_t crand_global = {{
    0x26aa069ea2fb1a4d, 0x70c72c95cd592d04,
    0x504f333d3aa0b359, 0x9e3779b97f4a7c15,
    0x6a09e667a754166b
}};

STC_DEF void csrand(uint64_t seed)
    { crand_global = crand_init(seed); }

STC_DEF uint64_t crand(void)
    { return crand_u64(&crand_global); }

STC_DEF double crandf(void)
    { return crand_f64(&crand_global); }

STC_DEF crand_t crand_init(uint64_t seed) {
    crand_t rng; uint64_t* s = rng.state;
    s[0] = seed + 0x9e3779b97f4a7c15;
    s[1] = (s[0] ^ (s[0] >> 30))*0xbf58476d1ce4e5b9;
    s[2] = (s[1] ^ (s[1] >> 27))*0x94d049bb133111eb;
    s[3] = (s[2] ^ (s[2] >> 31));
    s[4] = ((seed + 0x6aa069ea2fb1a4d) << 1) | 1;
    return rng;
}

/* Init unbiased uniform uint RNG with bounds [low, high] */
STC_DEF crand_unif_t crand_unif_init(int64_t low, int64_t high) {
    crand_unif_t dist = {low, (uint64_t) (high - low + 1)};
    dist.threshold = (uint64_t)(0 - dist.range) % dist.range;
    return dist;
}

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

/* Int64 uniform distributed RNG, range [low, high]. */
STC_DEF int64_t crand_unif(crand_t* rng, crand_unif_t* d) {
    uint64_t lo, hi;
#ifdef c_umul128
    do { c_umul128(crand_u64(rng), d->range, &lo, &hi); } while (lo < d->threshold);
#else
    do { lo = crand_u64(rng); hi = lo % d->range; } while (lo - hi > -d->range);
#endif
    return d->lower + (int64_t)hi;
}

/* Normal distribution PRNG. Marsaglia polar method */
STC_DEF double crand_norm(crand_t* rng, crand_norm_t* dist) {
    double u1, u2, s, m;
    if (dist->has_next++ & 1)
        return dist->next*dist->stddev + dist->mean;
    do {
        u1 = 2.0 * crand_f64(rng) - 1.0;
        u2 = 2.0 * crand_f64(rng) - 1.0;
        s = u1*u1 + u2*u2;
    } while (s >= 1.0 || s == 0.0);
    m = sqrt(-2.0 * log(s) / s);
    dist->next = u2*m;
    return (u1*m)*dist->stddev + dist->mean;
}

#endif
#endif
#undef i_opt
#undef i_static
#undef i_header
#undef i_implement
#undef i_extern
