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
/*
    crandom_eng32_t eng = crandom_eng32_init(seed);
    crandom_uniform_f32_t fdist = crandom_uniform_f32_init(1.0f, 6.0f);
    crandom_uniform_i32_t idist = crandom_uniform_i32_init(1, 6);
    
    uint32_t i = crandom_i32(&eng);
    int j = crandom_uniform_i32(&eng, idist);
    float r = crandom_uniform_f32(&eng, fdist);
*/

typedef struct {uint64_t state[2];} crandom_eng32_t;
typedef struct {int32_t min, range;} crandom_uniform_i32_t;
typedef struct {float min, range;} crandom_uniform_f32_t;

/* 32 bit random number generator engine */
STC_API crandom_eng32_t crandom_eng32_with_seq(uint64_t seed, uint64_t seq);
STC_INLINE crandom_eng32_t crandom_eng32_init(uint64_t seed) {
    return crandom_eng32_with_seq(seed, seed);
}

/* int random number generator, range [0, 2^32) */
STC_API uint32_t crandom_i32(crandom_eng32_t* rng);

STC_INLINE float crandom_f32(crandom_eng32_t* rng) {
    union {uint32_t i; float f;} u = {0x3F800000u | (crandom_i32(rng) >> 9)};
    return u.f - 1.0f;
}

/* int random number generator in range [low, high] */
STC_INLINE crandom_uniform_i32_t crandom_uniform_i32_init(int32_t low, int32_t high) {
    crandom_uniform_i32_t dist = {low, high - low + 1}; return dist;
}
STC_INLINE int32_t crandom_uniform_i32(crandom_eng32_t* rng, crandom_uniform_i32_t dist) {
    return dist.min + (int32_t) (((uint64_t) crandom_i32(rng) * dist.range) >> 32);
}

/* float random number in range [low, high). Note: 23 bit resolution. */
STC_INLINE crandom_uniform_f32_t crandom_uniform_f32_init(float low, float high) {
    crandom_uniform_f32_t dist = {low, high - low}; return dist;
}
STC_INLINE float crandom_uniform_f32(crandom_eng32_t* rng, crandom_uniform_f32_t dist) {
    return dist.min + crandom_f32(rng) * dist.range;
}


typedef struct {uint64_t state[4];} crandom_eng64_t;
typedef struct {double min, range;} crandom_uniform_f64_t;

/* 64 bit random number generator engine */
STC_API crandom_eng64_t crandom_eng64_with_seq(uint64_t seed, uint64_t seq);
STC_INLINE crandom_eng64_t crandom_eng64_init(uint64_t seed) {
    return crandom_eng64_with_seq(seed, 1);
}
/* int random number generator, range [0, 2^64) */
STC_API uint64_t crandom_i64(crandom_eng64_t* rng);

STC_INLINE double crandom_f64(crandom_eng64_t* rng) {
    union {uint64_t i; double f;} u = {0x3FF0000000000000ull | (crandom_i64(rng) >> 12)};
    return u.f - 1.0;
}

/* double random number in range [low, high). 52 bit resolution. */
STC_INLINE crandom_uniform_f64_t crandom_uniform_f64_init(float low, float high) {
    crandom_uniform_f64_t dist = {low, high - low}; return dist;
}
STC_INLINE double crandom_uniform_f64(crandom_eng64_t* rng, crandom_uniform_f64_t dist) {
    return dist.min + crandom_f64(rng) * dist.range;
}


#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)

/* PCG32 random number generator: https://www.pcg-random.org/download.html */

STC_API crandom_eng32_t crandom_eng32_with_seq(uint64_t seed, uint64_t seq) {
    crandom_eng32_t rng = {0u, (seq << 1u) | 1u}; /* inc must be odd */
    crandom_i32(&rng);
    rng.state[0] += seed;
    crandom_i32(&rng);
    return rng;
}

STC_API uint32_t crandom_i32(crandom_eng32_t* rng) {
    uint64_t old = rng->state[0];
    rng->state[0] = old * 6364136223846793005ull + rng->state[1];
    uint32_t xors = ((old >> 18u) ^ old) >> 27u;
    uint32_t rot = old >> 59u;
    return (xors >> rot) | (xors << ((-rot) & 31));
}

STC_API crandom_eng64_t crandom_eng64_with_seq(uint64_t seed, uint64_t seq) {
    crandom_eng64_t rng = {seed, seed, seed, (seq << 1u) | 1u}; /* increment must be odd */
    for (int i = 0; i < 12; ++i) crandom_i64(&rng);
    return rng;
}

#ifdef STC_USE_SFC64
/* SFC64 random number generator: http://pracrand.sourceforge.net */
STC_API uint64_t crandom_i64(crandom_eng64_t* rng) {
    enum {LROT = 24, RSHIFT = 11, LSHIFT = 3};
    uint64_t *s = rng->state;
    const uint64_t result = s[0] + s[1] + s[3]++;
    s[0] = s[1] ^ (s[1] >> RSHIFT);
    s[1] = s[2] + (s[2] << LSHIFT);
    s[2] = ((s[2] << LROT) | (s[2] >> (64 - LROT))) + result;
    return result;
}
#else
/* My own PRNG inspired by SFC64: Faster and has Weyl-sequence parameter. */
/* Copyright Tyge Løvset, 2020 */
/* Faster: updates only 192bit state. Parallel: Ensures unique sequence per seq (2^63) */
/* Minimum period is 2^64 per seq, average ~ 2^127 per seq */
STC_API uint64_t crandom_i64(crandom_eng64_t* rng) {
    enum {LROT = 24, RSHIFT = 11, LSHIFT = 3};
    uint64_t *s = rng->state;
    const uint64_t b = s[1], result = s[0] ^ (s[2] += s[3]|1);
    s[0] = (b + (b << LSHIFT)) ^ (b >> RSHIFT);
    s[1] = ((b << LROT) | (b >> (64 - LROT))) + result;
    return result;
}
#endif

#endif
#endif
