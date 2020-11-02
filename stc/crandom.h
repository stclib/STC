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
    crand_rng32_t rng = crand_rng32_init(seed);
    crand_uniform_f32_t fdist = crand_uniform_f32_init(rng, 1.0f, 6.0f);
    crand_uniform_i32_t idist = crand_uniform_i32_init(rng, 1, 6);
    
    uint32_t i = crand_i32(&rng);
    int j = crand_uniform_i32(&idist);
    float r = crand_uniform_f32(&fdist);
*/
#include "ccommon.h"
#include <string.h>
#include <math.h>

/* 32-BIT RANDOM NUMBER GENERATOR */

typedef struct {uint64_t state[2];} crand_rng32_t;
typedef struct {int32_t offset; uint32_t range;} crand_uniform_i32_t;
typedef struct {float offset, range;} crand_uniform_f32_t;

/* engine initializers */
STC_API crand_rng32_t crand_rng32_with_seq(uint64_t seed, uint64_t seq);
STC_INLINE crand_rng32_t crand_rng32_init(uint64_t seed) {
    return crand_rng32_with_seq(seed, seed);
}

/* int random number generator, range [0, 2^32) */
STC_API uint32_t crand_i32(crand_rng32_t* rng);

STC_INLINE float crand_f32(crand_rng32_t* rng) {
    union {uint32_t i; float f;} u = {0x3F800000u | (crand_i32(rng) >> 9)};
    return u.f - 1.0f;
}

/* int random number generator in range [low, high] */
STC_INLINE crand_uniform_i32_t crand_uniform_i32_init(int32_t low, int32_t high) {
    crand_uniform_i32_t dist = {low, (uint32_t) (high - low + 1)}; return dist;
}
STC_INLINE int32_t crand_uniform_i32(crand_rng32_t* rng, crand_uniform_i32_t* dist) {
    return dist->offset + (int32_t) (((uint64_t) crand_i32(rng) * dist->range) >> 32);
}
/* https://github.com/lemire/fastrange */
STC_API uint32_t crand_unbiased_i32(crand_rng32_t* rng, crand_uniform_i32_t* dist);

/* float random number in range [low, high). Note: 23 bit resolution. */
STC_INLINE crand_uniform_f32_t crand_uniform_f32_init(float low, float high) {
    crand_uniform_f32_t dist = {low, high - low}; return dist;
}
STC_INLINE float crand_uniform_f32(crand_rng32_t* rng, crand_uniform_f32_t* dist) {
    return dist->offset + crand_f32(rng) * dist->range;
}


/* 64 BIT RANDOM NUMBER GENERATOR */

typedef struct {uint64_t state[4];} crand_rng64_t;
typedef struct {int64_t offset; uint64_t range;} crand_uniform_i64_t;
typedef struct {double offset, range;} crand_uniform_f64_t;
typedef struct {double mean, stddev, next; bool has_next;} crand_normal_f64_t;


/* engine initializers */
STC_API crand_rng64_t crand_rng64_with_seq(uint64_t seed, uint64_t seq);
STC_INLINE crand_rng64_t crand_rng64_init(uint64_t seed) {
    return crand_rng64_with_seq(seed, 1);
}
/* int random number generator, range [0, 2^64). PRNG copyright Tyge Løvset, NORCE Research, 2020 */
STC_API uint64_t crand_i64(crand_rng64_t* rng);

/* double random number in range [low, high). 52 bit resolution. */
STC_INLINE double crand_f64(crand_rng64_t* rng) {
    union {uint64_t i; double f;} u = {0x3FF0000000000000ull | (crand_i64(rng) >> 12)};
    return u.f - 1.0;
}

/* int random number generator in range [low, high] */
STC_INLINE crand_uniform_i64_t crand_uniform_i64_init(int64_t low, int64_t high) {
    crand_uniform_i64_t dist = {low, (uint64_t) (high - low + 1)}; return dist;
}

#if defined(_MSC_VER) && defined(_WIN64)
#include <intrin.h>
#endif

STC_INLINE int64_t crand_uniform_i64(crand_rng64_t* rng, crand_uniform_i64_t* dist) {
    #if defined(__SIZEOF_INT128__)
        return dist->offset + (int64_t) (((__uint128_t) crand_i64(rng) * dist->range) >> 64);
    #elif defined(_MSC_VER) && defined(_WIN64)
        uint64_t hi; _umul128(crand_i64(rng), dist->range, &hi); return dist->offset + hi;
    #else
        return dist->offset + crand_i64(rng) % dist->range; // slower
    #endif
}

STC_INLINE crand_uniform_f64_t crand_uniform_f64_init(double low, double high) {
    crand_uniform_f64_t dist = {low, high - low}; return dist;
}
STC_INLINE double crand_uniform_f64(crand_rng64_t* rng, crand_uniform_f64_t* dist) {
    return dist->offset + crand_f64(rng) * dist->range;
}

STC_INLINE crand_normal_f64_t crand_normal_f64_init(double mean, double stddev) {
    crand_normal_f64_t dist = {mean, stddev, 0.0, false}; return dist;
}
STC_API double crand_normal_f64(crand_rng64_t* rng, crand_normal_f64_t* dist);


#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)

#if defined(_MSC_VER)
  #pragma warning(push)
  #pragma warning(disable: 4146)
#endif

/* PRNG PCG32 https://www.pcg-random.org/download.html */
STC_DEF crand_rng32_t crand_rng32_with_seq(uint64_t seed, uint64_t seq) {
    crand_rng32_t rng = {{0u, (seq << 1u) | 1u}}; /* inc must be odd */
    crand_i32(&rng);
    rng.state[0] += seed;
    crand_i32(&rng);
    return rng;
}
STC_DEF uint32_t crand_i32(crand_rng32_t* rng) {
    uint64_t old = rng->state[0];
    rng->state[0] = old * 6364136223846793005ull + rng->state[1];
    uint32_t xors = (uint32_t) (((old >> 18u) ^ old) >> 27u);
    uint32_t rot = old >> 59u;
    return (xors >> rot) | (xors << ((-rot) & 31));
}

/* PRNG STC64: copyright Tyge Løvset, NORCE Research, 2020 */
/* Extremely fast PRNG suited for parallel usage with Weyl-sequence parameter. */
/* Even faster than sfc64: updates only 192bit state. Better for parallel processing: */
/* Guarantees 2^63 unique threads with minimum 2^64 period length ~ 2^160 average period. */
/* Tested with PractRand to 8 TB output: no issues */
STC_DEF crand_rng64_t crand_rng64_with_seq(uint64_t seed, uint64_t seq) {
    crand_rng64_t rng = {{seed, seed, seed, (seq << 1u) | 1u}}; /* increment must be odd */
    for (int i = 0; i < 12; ++i) crand_i64(&rng);
    return rng;
}
STC_DEF uint64_t crand_i64(crand_rng64_t* rng) {
    enum {LROT = 24, RSHIFT = 11, LSHIFT = 3};
    uint64_t *s = rng->state;
    const uint64_t b = s[1], result = s[0] ^ (s[2] += s[3]|1);
    s[0] = (b + (b << LSHIFT)) ^ (b >> RSHIFT);
    s[1] = ((b << LROT) | (b >> (64 - LROT))) + result;
    return result;
}

/* Unbiased uniform https://github.com/lemire/fastrange */
STC_DEF uint32_t crand_unbiased_i32(crand_rng32_t* rng, crand_uniform_i32_t* dist) {
    uint32_t r = dist->range;
    uint64_t m = (uint64_t) crand_i32(rng) * r;
    uint32_t l = (uint32_t) m;
    if (l < r) {
        uint32_t t = -r;
        if (t >= r) if ((t -= r) >= r) t %= r;
        while (l < t) l = (uint32_t) (m = (uint64_t) crand_i32(rng) * r);
    }
    return dist->offset + (m >> 32);
}

/* Marsaglia polar method for gaussian distribution. */
STC_DEF double crand_normal_f64(crand_rng64_t* rng, crand_normal_f64_t* dist) {
    double u1, u2, s, m;
    if (dist->has_next) {
        dist->has_next = false;
        return dist->next * dist->stddev + dist->mean;
    }
    do {
        u1 = 2.0 * crand_f64(rng) - 1.0;
        u2 = 2.0 * crand_f64(rng) - 1.0;
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
