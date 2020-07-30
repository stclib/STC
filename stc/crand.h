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

#ifndef CRAND__H__
#define CRAND__H__

#include "cdefs.h"
#include <string.h>
/*
    crand_eng32_t eng = crand_eng32_init(seed);
    crand_uniform_f32_t fdist = crand_uniform_f32_init(1.0f, 6.0f);
    crand_uniform_i32_t idist = crand_uniform_i32_init(1, 6);
    
    uint32_t i = crand_gen_i32(&eng);
    int j = crand_uniform_i32(&eng, idist);
    float r = crand_uniform_f32(&eng, fdist);
*/

typedef struct {uint64_t state[2];} crand_eng32_t;
typedef struct {int32_t min, range;} crand_uniform_i32_t;
typedef struct {float min, range;} crand_uniform_f32_t;

/* 32 bit random number generator engine */
STC_API crand_eng32_t crand_eng32_with_seq(uint64_t seed, uint64_t seq);
STC_INLINE crand_eng32_t crand_eng32_init(uint64_t seed) {
    return crand_eng32_with_seq(seed, 1);
}

/* int random number generator, range [0, 2^32) */
STC_API uint32_t crand_gen_i32(crand_eng32_t* rng);

STC_INLINE float crand_gen_f32(crand_eng32_t* rng) {
    union {uint32_t i; float f;} u = {0x3F800000u | (crand_gen_i32(rng) >> 9)};
    return u.f - 1.0f;
}

/* int random number generator in range [low, high] */
STC_INLINE crand_uniform_i32_t crand_uniform_i32_init(int32_t low, int32_t high) {
    crand_uniform_i32_t dist = {low, high - low + 1}; return dist;
}
STC_INLINE int32_t crand_uniform_i32(crand_eng32_t* rng, crand_uniform_i32_t dist) {
    return dist.min + (int32_t) (((uint64_t) crand_gen_i32(rng) * dist.range) >> 32);
}

/* float random number in range [low, high). Note: 23 bit resolution. */
STC_INLINE crand_uniform_f32_t crand_uniform_f32_init(float low, float high) {
    crand_uniform_f32_t dist = {low, high - low}; return dist;
}
STC_INLINE float crand_uniform_f32(crand_eng32_t* rng, crand_uniform_f32_t dist) {
    return dist.min + crand_gen_f32(rng) * dist.range;
}


typedef struct {uint64_t state[4];} crand_eng64_t;
typedef struct {double min, range;} crand_uniform_f64_t;

/* 64 bit random number generator engine */
STC_API crand_eng64_t crand_eng64_with_seq(uint64_t seed, uint64_t seq);
STC_INLINE crand_eng64_t crand_eng64_init(uint64_t seed) {
    return crand_eng64_with_seq(seed, 1);
}
/* int random number generator, range [0, 2^64) */
STC_API uint64_t crand_gen_i64(crand_eng64_t* rng);

STC_INLINE double crand_gen_f64(crand_eng64_t* rng) {
    union {uint64_t i; double f;} u = {0x3FF0000000000000ull | (crand_gen_i64(rng) >> 12)};
    return u.f - 1.0;
}

/* double random number in range [low, high). 52 bit resolution. */
STC_INLINE crand_uniform_f64_t crand_uniform_f64_init(float low, float high) {
    crand_uniform_f64_t dist = {low, high - low}; return dist;
}
STC_INLINE double crand_uniform_f64(crand_eng64_t* rng, crand_uniform_f64_t dist) {
    return dist.min + crand_gen_f64(rng) * dist.range;
}


#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)

/* PCG32 random number generator: https://www.pcg-random.org/index.html */

STC_API crand_eng32_t crand_eng32_with_seq(uint64_t seed, uint64_t seq) {
    crand_eng32_t rng = {0u, (seq << 1u) | 1u}; /* inc must be odd */
    crand_gen_i32(&rng);
    rng.state[0] += seed;
    crand_gen_i32(&rng);
    return rng;
}
STC_API uint32_t crand_gen_i32(crand_eng32_t* rng) {
    uint64_t old = rng->state[0];
    rng->state[0] = old * 6364136223846793005ull + rng->state[1];
    uint32_t xors = ((old >> 18u) ^ old) >> 27u;
    uint32_t rot = old >> 59u;
    return (xors >> rot) | (xors << ((-rot) & 31));
}

/* New 64bit PRNG losely based on SFC64: Copyright 2020, Tyge Løvset */
/* Faster: Updates only 192bit state. Parallel: Ensures unique sequence for each seq (2^63 seqs) */
/* Minimal period is 2^64 per seq, average ~ 2^127 per seq */
STC_API crand_eng64_t crand_eng64_with_seq(uint64_t seed, uint64_t seq) {
    crand_eng64_t rng = {seed, seed, seed, (seq << 1u) | 1u}; /* increment must be odd */
    for (int i = 0; i < 12; ++i) crand_gen_i64(&rng);
    return rng;
}

STC_API uint64_t crand_gen_i64(crand_eng64_t* rng) {
    enum {LROT = 24, RSHIFT = 11, LSHIFT = 3};
    uint64_t *s = rng->state;
    const uint64_t b = s[1], result = s[0] ^ (s[2] += s[3]|1);
    s[0] = (b + (b << LSHIFT)) ^ (b >> RSHIFT);
    s[1] = ((b << LROT) | (b >> (64 - LROT))) + result;
    return result;
}

/* // Original SFC64 random number generator: http://pracrand.sourceforge.net
STC_API uint64_t crand_gen_i64(crand_eng64_t* rng) {
    enum {LROT = 24, RSHIFT = 11, LSHIFT = 3};
    uint64_t *s = rng->state;
    const uint64_t result = s[0] + s[1] + s[3]++;
    s[0] = s[1] ^ (s[1] >> RSHIFT);
    s[1] = s[2] + (s[2] << LSHIFT);
    s[2] = ((s[2] << LROT) | (s[2] >> (64 - LROT))) + result;
    return result;
}
*/
#endif

#endif
