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

typedef struct {uint64_t state, inc;} crandom32_t;

/* 32 bit random number generator engine */
STC_API crandom32_t crandom32_uniform_engine2(uint64_t seed, uint64_t seq);

STC_INLINE crandom32_t crandom32_uniform_engine(uint64_t seed) {
    return crandom32_uniform_engine2(seed, 0);
}
/* int random number generator, range [0, 2^32) */
STC_API uint32_t crandom32_uniform_int(crandom32_t* rng);

/* float random number in range [0.0f, 1.0f). Note: 23 bit resolution. */
STC_INLINE float crandom32_uniform_real(crandom32_t* rng) {
    union {uint32_t i; float f;} u = {0x3F800000u | (crandom32_uniform_int(rng) >> 9)};
    return u.f - 1.0f;
}

/* int random number generator in range [0, bound) */
STC_INLINE uint32_t crandom32_uniform_int_bounded(crandom32_t* rng, uint32_t bound) {
    return (uint32_t) (((uint64_t) crandom32_uniform_int(rng) * bound) >> 32);
}


typedef struct {uint64_t state[4];} crandom64_t;

/* 64 bit random number generator engine */
STC_API crandom64_t crandom64_uniform_engine(const uint64_t seed);

/* int random number generator, range [0, 2^64) */
STC_API uint64_t crandom64_uniform_int(crandom64_t* rng);

/* float64 random number in range [0.0, 1.0), 52 bit resolution */
STC_INLINE double crandom64_uniform_real(crandom64_t* rng) {
    union {uint64_t i; double f;} u = {0x3FF0000000000000ull | (crandom64_uniform_int(rng) >> 12)};
    return u.f - 1.0;
}


#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)

/* PCG32 random number generator: https://www.pcg-random.org/index.html */

STC_API crandom32_t crandom32_uniform_engine2(uint64_t seed, uint64_t seq) {
    crandom32_t rng = {0u, (seq << 1u) | 1u}; /* inc must be odd */
    crandom32_uniform_int(&rng);
    rng.state += seed;
    crandom32_uniform_int(&rng);
    return rng;
}

STC_API uint32_t crandom32_uniform_int(crandom32_t* rng) {
    uint64_t old = rng->state;
    rng->state = old * 6364136223846793005ull + rng->inc;
    uint32_t xos = ((old >> 18u) ^ old) >> 27u;
    uint32_t rot = old >> 59u;
    return (xos >> rot) | (xos << ((-rot) & 31));
}

/* SFC64 random number generator: http://pracrand.sourceforge.net */

STC_API crandom64_t crandom64_uniform_engine(const uint64_t seed) {
    crandom64_t state = {{seed, seed, seed, 1}};
    for (int i = 0; i < 12; ++i) crandom64_uniform_int(&state);
    return state;
}

STC_API uint64_t crandom64_uniform_int(crandom64_t* rng) {
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
