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
// cstc64: Pseudo-random number generator
#include "stc/crandom.h"
int main() {
    uint64_t seed = 123456789;
    cstc64_t rng = cstc64_init(seed);
    cstc64_uniform_t dist1 = cstc64_uniform_init(1, 6);
    cstc64_uniformf_t dist2 = cstc64_uniformf_init(1.0, 10.0);
    cstc64_normalf_t dist3 = cstc64_normalf_init(1.0, 10.0);
    
    uint64_t i = cstc64_rand(&rng);
    int64_t iu = cstc64_uniform(&rng, &dist1);
    double xu = cstc64_uniformf(&rng, &dist2);
    double xn = cstc64_normalf(&rng, &dist3);
}
*/
#include "ccommon.h"
#include <string.h>
#include <math.h>

typedef struct {uint64_t state[4];}                         cstc64_t;
typedef struct {int64_t offset; uint64_t range, threshold;} cstc64_uniform_t;
typedef struct {double offset, range;}                      cstc64_uniformf_t;
typedef struct {double mean, stddev, next; bool has_next;}  cstc64_normalf_t;


/* int random number generator, range [0, 2^64). PRNG copyright Tyge Løvset, NORCE Research, 2020 */
STC_API cstc64_t cstc64_init(uint64_t seed);
STC_API cstc64_t cstc64_with_seq(uint64_t seed, uint64_t seq);
STC_API uint64_t cstc64_rand(cstc64_t* rng);

/* double random number in range [low, high). */
STC_INLINE double cstc64_randf(cstc64_t* rng) {
    union {uint64_t i; double f;} u = {0x3FF0000000000000ull | (cstc64_rand(rng) >> 12)};
    return u.f - 1.0;
}

/* integer uniform distributed RNG, range [low, high]. */
STC_API cstc64_uniform_t cstc64_uniform_init(int64_t low, int64_t high);
STC_API int64_t          cstc64_uniform(cstc64_t* rng, cstc64_uniform_t* dist);

/* double uniform distributed RNG, range [low, high). */
STC_INLINE cstc64_uniformf_t cstc64_uniformf_init(double low, double high) {
    cstc64_uniformf_t dist = {low, high - low}; return dist;
}
STC_INLINE double cstc64_uniformf(cstc64_t* rng, cstc64_uniformf_t* dist) {
    return dist->offset + cstc64_randf(rng) * dist->range;
}

/* double normal distributed RNG. */
STC_INLINE cstc64_normalf_t cstc64_normalf_init(double mean, double stddev) {
    cstc64_normalf_t dist = {mean, stddev, 0.0, false}; return dist;
}
STC_API double cstc64_normalf(cstc64_t* rng, cstc64_normalf_t* dist);


#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)

#if defined(_MSC_VER)
  #pragma warning(push)
  //#pragma warning(disable: 4146)
#endif

/* PRNG stc64: Tyge Løvset, NORCE Research, 2020.
 * Extremely fast PRNG suited for parallel usage with Weyl-sequence parameter.
 * Faster than sfc64, wyhash64, and almost 50% faster than xoshiro256** on gcc.
 * 256bit state, updates only 192bit per rng.
 * 2^63 unique threads with a minimum 2^64 period lengths each.
 * 2^127 minimum period length for single thread (double loop).
 * Passes PractRand tested up to 8TB output, Vigna's Hamming weight test, 
 * and simple correlation tests, i.e. interleaved streams with one-bit diff state.
 */

STC_DEF cstc64_t cstc64_init(uint64_t seed) {
    return cstc64_with_seq(seed, 0x3504f333d3aa0b34);
}
STC_DEF cstc64_t cstc64_with_seq(uint64_t seed, uint64_t seq) {
    cstc64_t rng = {{seed, seed, seed, (seq << 1u) | 1u}};
    for (int i = 0; i < 8; ++i) cstc64_rand(&rng);
    return rng;
}
STC_DEF uint64_t cstc64_rand(cstc64_t* rng) {
    enum {LROT = 24, RSHIFT = 11, LSHIFT = 3};
    uint64_t *s = rng->state;
    const uint64_t result = s[0] ^ (s[2] += s[3]|1);
    s[0] = (s[1] + (s[1] << LSHIFT)) ^ (s[1] >> RSHIFT);
    s[1] = ((s[1] << LROT) | (s[1] >> (64 - LROT))) + result;
    return result;
}

/* unbiased integer random number generator in range [low, high] */
STC_DEF cstc64_uniform_t cstc64_uniform_init(int64_t low, int64_t high) {
    cstc64_uniform_t dist = {low, (uint64_t) (high - low + 1)};
    dist.threshold = (uint64_t)(-dist.range) % dist.range;
    return dist;
}
STC_DEF int64_t cstc64_uniform(cstc64_t* rng, cstc64_uniform_t* dist) {
    for (;;) {
        uint64_t r = cstc64_rand(rng);
        if (r >= dist->threshold) return dist->offset + (r % dist->range);
    }
}

/* Marsaglia polar method for gaussian distribution. */
STC_DEF double cstc64_normalf(cstc64_t* rng, cstc64_normalf_t* dist) {
    double u1, u2, s, m;
    if (dist->has_next) {
        dist->has_next = false;
        return dist->next * dist->stddev + dist->mean;
    }
    do {
        u1 = 2.0 * cstc64_randf(rng) - 1.0;
        u2 = 2.0 * cstc64_randf(rng) - 1.0;
        s = u1*u1 + u2*u2;
    } while (s >= 1.0 || s == 0.0);
    m = sqrt(-2.0 * log(s) / s);
    dist->next = u2 * m, dist->has_next = true;
    return (u1 * m) * dist->stddev + dist->mean;
}

#if defined(_MSC_VER)
  #pragma warning(pop)
#endif

#endif
#endif
