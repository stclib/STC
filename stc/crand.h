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
    crand_eng32_t eng = crand_eng32(seed);
    crand_f32_uniform_t fdist = crand_f32_uniform(1.0f, 6.0f);
    crand_i32_uniform_t idist = crand_i32_uniform(1, 6);
    
    uint32_t i = crand_gen_i32(&eng);
    int j = crand_gen_i32_uniform(&eng, idist);
    float r = crand_gen_f32_uniform(&eng, fdist);
*/

typedef struct {uint64_t state, inc;} crand_eng32_t;
typedef struct {int32_t min; uint32_t range;} crand_i32_uniform_t;
typedef struct {float min, range;} crand_f32_uniform_t;

/* 32 bit random number generator engine */
STC_API crand_eng32_t crand_eng32_with_id(uint64_t seed, uint64_t seq);

STC_INLINE crand_eng32_t crand_eng32(uint64_t seed) {
    return crand_eng32_with_id(seed, 0);
}

/* int random number generator, range [0, 2^32) */
STC_API uint32_t crand_gen_i32(crand_eng32_t* rng);

STC_INLINE float crand_gen_f32(crand_eng32_t* rng) {
    union {uint32_t i; float f;} u = {0x3F800000u | (crand_gen_i32(rng) >> 9)};
    return u.f - 1.0f;
}

/* int random number generator in range [low, high] */
STC_INLINE crand_i32_uniform_t crand_i32_uniform(int32_t low, int32_t high) {
    crand_i32_uniform_t dist = {low, high - low}; return dist;
}
STC_INLINE uint32_t crand_gen_i32_uniform(crand_eng32_t* rng, crand_i32_uniform_t dist) {
    return dist.min + (int32_t) (((uint64_t) crand_gen_i32(rng) * dist.range) >> 32);
}

/* float random number in range [low, high). Note: 23 bit resolution. */
STC_INLINE crand_f32_uniform_t crand_f32_uniform(float low, float high) {
    crand_f32_uniform_t dist = {low, high - low}; return dist;
}
STC_INLINE float crand_gen_f32_uniform(crand_eng32_t* rng, crand_f32_uniform_t dist) {
    return dist.min + crand_gen_f32(rng) * dist.range;
}


typedef struct {uint64_t state[4];} crand_eng64_t;
typedef struct {double min, range;} crand_f64_uniform_t;

/* 64 bit random number generator engine */
STC_API crand_eng64_t crand_eng64(const uint64_t seed);

/* int random number generator, range [0, 2^64) */
STC_API uint64_t crand_gen_i64(crand_eng64_t* rng);

STC_INLINE double crand_gen_f64(crand_eng64_t* rng) {
    union {uint64_t i; double f;} u = {0x3FF0000000000000ull | (crand_gen_i64(rng) >> 12)};
    return u.f - 1.0;
}

/* double random number in range [low, high). 52 bit resolution. */
STC_INLINE crand_f64_uniform_t crand_f64_uniform(float low, float high) {
    crand_f64_uniform_t dist = {low, high - low}; return dist;
}
STC_INLINE double crand_gen_f64_uniform(crand_eng64_t* rng, crand_f64_uniform_t dist) {
    return dist.min + crand_gen_f64(rng) * dist.range;
}


#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)

/* PCG32 random number generator: https://www.pcg-random.org/index.html */

STC_API crand_eng32_t crand_eng32_with_id(uint64_t seed, uint64_t seq) {
    crand_eng32_t rng = {0u, (seq << 1u) | 1u}; /* inc must be odd */
    crand_gen_i32(&rng);
    rng.state += seed;
    crand_gen_i32(&rng);
    return rng;
}

STC_API uint32_t crand_gen_i32(crand_eng32_t* rng) {
    uint64_t old = rng->state;
    rng->state = old * 6364136223846793005ull + rng->inc;
    uint32_t xos = ((old >> 18u) ^ old) >> 27u;
    uint32_t rot = old >> 59u;
    return (xos >> rot) | (xos << ((-rot) & 31));
}

/* SFC64 random number generator: http://pracrand.sourceforge.net */

STC_API crand_eng64_t crand_eng64(const uint64_t seed) {
    crand_eng64_t state = {{seed, seed, seed, 1}};
    for (int i = 0; i < 12; ++i) crand_gen_i64(&state);
    return state;
}

STC_API uint64_t crand_gen_i64(crand_eng64_t* rng) {
    enum {LR=24, RS=11, LS=3};
    uint64_t *s = rng->state;
    const uint64_t result = s[0] + s[1] + s[3]++;
    s[0] = s[1] ^ (s[1] >> RS);
    s[1] = s[2] + (s[2] << LS);
    s[2] = (s[2] << LR) | (s[2] >> (64 - LR));
    return result;
}

#endif

#endif
