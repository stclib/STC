/* MIT License
 *
 * Copyright (c) 2022 Tyge Løvset, NORCE, www.norceresearch.no
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

#ifndef CRANDOM_H_INCLUDED
#define CRANDOM_H_INCLUDED
/*
// crandom: Pseudo-random number generator
#include "stc/crandom.h"
int main() {
    uint64_t seed = 123456789;
    stc64_t rng = stc64_init(seed);
    stc64_uniform_t dist1 = stc64_uniform_init(1, 6);
    stc64_uniformf_t dist2 = stc64_uniformf_init(1.0, 10.0);
    stc64_normalf_t dist3 = stc64_normalf_init(1.0, 10.0);

    uint64_t i = stc64_rand(&rng);
    int64_t iu = stc64_uniform(&rng, &dist1);
    double xu = stc64_uniformf(&rng, &dist2);
    double xn = stc64_normalf(&rng, &dist3);
}
*/
#include <string.h>
#include <math.h>

typedef struct stc64 { uint64_t state[5]; } stc64_t;
typedef struct stc32 { uint32_t state[5]; } stc32_t;
typedef struct stc64_uniform { int64_t lower; uint64_t range, threshold; } stc64_uniform_t;
typedef struct stc32_uniform { int32_t lower; uint32_t range, threshold; } stc32_uniform_t;
typedef struct stc64_uniformf { double lower, range; } stc64_uniformf_t;
typedef struct stc64_normalf { double mean, stddev, next; unsigned has_next; } stc64_normalf_t;

/* PRNG stc64.
 * Very fast PRNG suited for parallel usage with Weyl-sequence parameter.
 * 320-bit state, 256 bit is mutable.
 * Noticable faster than xoshiro and pcg, slighly slower than wyrand64 and
 * Romu, but these have restricted capacity for larger parallel jobs or unknown minimum periods.
 * stc64 supports 2^63 unique threads with a minimum 2^64 period lengths each.
 * Passes all statistical tests, e.g PractRand and correlation tests, i.e. interleaved
 * streams with one-bit diff state. Even the 16-bit version (LR=6, RS=5, LS=3) passes
 * PractRand to multiple TB input.
 */

/* Global STC64 PRNG */
STC_API void     stc64_srandom(uint64_t seed);
STC_API uint64_t stc64_random(void);

STC_API uint64_t stc64_rand(stc64_t* rng);
STC_API uint32_t stc32_rand(stc32_t* rng);

/* Init with sequence number */
STC_API stc64_t  stc64_with_seq(uint64_t seed, uint64_t seq);
STC_API stc32_t  stc32_with_seq(uint32_t seed, uint32_t seq);

/* Int uniform distributed RNG, range [low, high]. */
STC_API stc64_uniform_t stc64_uniform_init(int64_t low, int64_t high);
STC_API stc32_uniform_t stc32_uniform_init(int32_t low, int32_t high);

/* Float64 uniform distributed RNG, range [low, high). */
STC_API stc64_uniformf_t stc64_uniformf_init(double low, double high);

/* Normal distribution (double) */
STC_API stc64_normalf_t stc64_normalf_init(double mean, double stddev);
STC_API double          stc64_normalf(stc64_t* rng, stc64_normalf_t* dist);

/* Unbiased bounded uniform distribution. */
STC_API int64_t stc64_uniform(stc64_t* rng, stc64_uniform_t* d);

STC_INLINE stc64_t stc64_init(uint64_t seed) {
    return stc64_with_seq(seed, seed + 0x3504f333d3aa0b37);
}

STC_INLINE stc32_t stc32_init(uint32_t seed) {
    return stc32_with_seq(seed, seed + 0xd3aa0b37);
}

/* Float64 random number in range [0.0, 1.0). */
STC_INLINE double stc64_randf(stc64_t* rng) {
    union {uint64_t i; double f;} u = {0x3FF0000000000000ull | (stc64_rand(rng) >> 12)};
    return u.f - 1.0;
}

/* Float64 uniform distributed RNG, range [low, high). */
STC_INLINE double stc64_uniformf(stc64_t* rng, stc64_uniformf_t* dist) {
    return stc64_randf(rng)*dist->range + dist->lower;
}

STC_INLINE int32_t stc32_uniform(stc32_t* rng, stc32_uniform_t* d) {
    uint64_t val;
    do { val = stc32_rand(rng) * (uint64_t)d->range; } while ((uint32_t)val < d->threshold);
    return d->lower + (val >> 32);
}

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined(_i_implement)

/* Global random() */
static stc64_t stc64_global = {{
    0x26aa069ea2fb1a4d, 0x70c72c95cd592d04,
    0x504f333d3aa0b359, 0x9e3779b97f4a7c15,
    0x6a09e667a754166b
}};

STC_DEF void stc64_srandom(uint64_t seed) {
    stc64_global = stc64_init(seed);
}

STC_DEF uint64_t stc64_random(void) {
    return stc64_rand(&stc64_global);
}

/* rng.state[4] must be odd */
STC_DEF stc64_t stc64_with_seq(uint64_t seed, uint64_t seq) {
    stc64_t rng = {{seed, seed+0x26aa069ea2fb1a4d, seed+0x70c72c95cd592d04,
                          seed+0x504f333d3aa0b359, (seq << 1) | 1}};
    for (int i = 0; i < 6; ++i) stc64_rand(&rng);
    return rng;
}

STC_DEF stc32_t stc32_with_seq(uint32_t seed, uint32_t seq) {
    stc32_t rng = {{seed, seed+0x26aa069e, seed+0xa2fb1a4d,
                          seed+0x70c72c95, (seq << 1) | 1}};
    for (int i = 0; i < 6; ++i) stc32_rand(&rng);
    return rng;
}
#ifdef _MSC_VER
#pragma warning(disable: 4146) // unary minus operator applied to unsigned type
#endif
/* Init unbiased uniform uint RNG with bounds [low, high] */
STC_DEF stc64_uniform_t stc64_uniform_init(int64_t low, int64_t high) {
    stc64_uniform_t dist = {low, (uint64_t) (high - low + 1)};
    dist.threshold = (uint64_t)(-dist.range) % dist.range;
    return dist;
}

STC_DEF stc32_uniform_t stc32_uniform_init(int32_t low, int32_t high) {
    stc32_uniform_t dist = {low, (uint32_t) (high - low + 1)};
    dist.threshold = (uint32_t)(-dist.range) % dist.range;
    return dist;
}
#ifdef _MSC_VER
#pragma warning(default: 4146)
#endif

STC_DEF uint64_t stc64_rand(stc64_t* rng) {
    uint64_t *s = rng->state; enum {LR=24, RS=11, LS=3};
    const uint64_t result = (s[0] ^ (s[3] += s[4])) + s[1];
    s[0] = s[1] ^ (s[1] >> RS);
    s[1] = s[2] + (s[2] << LS);
    s[2] = ((s[2] << LR) | (s[2] >> (64 - LR))) + result;
    return result;
}

STC_DEF uint32_t stc32_rand(stc32_t* rng) {
    uint32_t *s = rng->state; enum {LR=21, RS=9, LS=3};
    const uint32_t result = (s[0] ^ (s[3] += s[4])) + s[1];
    s[0] = s[1] ^ (s[1] >> RS);
    s[1] = s[2] + (s[2] << LS);
    s[2] = ((s[2] << LR) | (s[2] >> (32 - LR))) + result;
    return result;
}

STC_DEF int64_t stc64_uniform(stc64_t* rng, stc64_uniform_t* d) {
#ifdef c_umul128
    uint64_t lo, hi;
    do { c_umul128(stc64_rand(rng), d->range, &lo, &hi); } while (lo < d->threshold);
    return d->lower + hi;
#else
    uint64_t x, r;
    do {
        x = stc64_rand(rng);
        r = x % d->range;
    } while (x - r > -d->range);
    return d->lower + r;
#endif
}

/* Init uniform distributed float64 RNG, range [low, high). */
STC_DEF stc64_uniformf_t stc64_uniformf_init(double low, double high) {
    return c_make(stc64_uniformf_t){low, high - low};
}

/* Marsaglia polar method for gaussian/normal distribution, float64. */
STC_DEF stc64_normalf_t stc64_normalf_init(double mean, double stddev) {
    return c_make(stc64_normalf_t){mean, stddev, 0.0, 0};
}

STC_DEF double stc64_normalf(stc64_t* rng, stc64_normalf_t* dist) {
    double u1, u2, s, m;
    if (dist->has_next++ & 1)
        return dist->next * dist->stddev + dist->mean;
    do {
        u1 = 2.0 * stc64_randf(rng) - 1.0;
        u2 = 2.0 * stc64_randf(rng) - 1.0;
        s = u1*u1 + u2*u2;
    } while (s >= 1.0 || s == 0.0);
    m = sqrt(-2.0 * log(s) / s);
    dist->next = u2 * m;
    return (u1 * m) * dist->stddev + dist->mean;
}
#endif
#endif
#undef i_opt
