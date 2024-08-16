/* MIT License
 *
 * Copyright (c) 2024 Tyge Løvset
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
#ifndef STC_RANDOM_H_INCLUDED
#define STC_RANDOM_H_INCLUDED

#include <math.h>
#include <stdint.h>

/* Random numbers */

typedef struct { uint64_t data[3]; } crandom_s;

static inline crandom_s* _romu3(void) {
    static crandom_s state = {
        .data={0xa4c1f32680f70c55, 0x6f68261b57e7a770, 0xe220a838bf5c9dde}
    };
    return &state;
}

static inline void csrandom_r(crandom_s* state, uint64_t seed) {
    uint64_t* s = state->data;
    s[0]  = seed*0x9e3779b97f4a7c15;
    s[0] ^= s[0] >> 30;
    s[1]  = s[0]*0xbf58476d1ce4e5b9;
    s[1] ^= s[1] >> 27;
    s[2]  = s[1]*0x94d049bb133111eb;
    s[2] ^= s[2] >> 31;
}

static inline void csrandom(uint64_t seed)
    { csrandom_r(_romu3(), seed); }

static inline crandom_s crandom_rng(uint64_t seed) {
    crandom_s state;
    csrandom_r(&state, seed);
    return state;
}


static inline uint64_t crandom_r(crandom_s* state) { // romu_trio
    uint64_t* s = state->data;
    uint64_t xp = s[0], yp = s[1], zp = s[2];
    s[0] = zp * 0xd3833e804f4c574b;
    s[1] = yp - xp; s[1] = ((s[1]<<12) | (s[1]>>52));
    s[2] = zp - yp; s[2] = ((s[2]<<44) | (s[2]>>20));
    return xp;
}

static inline uint64_t crandom(void)
    { return crandom_r(_romu3()); }


static inline double crandom_float_r(crandom_s* state) {
    union {uint64_t i; double f;} u = {0x3FF0000000000000 | (crandom_r(state) >> 12)};
    return u.f - 1.0; /* [0.0, 1.0) */
}
static inline double crandom_float(void)
    { return crandom_float_r(_romu3()); }


typedef struct {
    double mean, stddev;
    double _next;
    int _has_next;
} crandom_normal_distr;

static inline double crandom_normal_r(crandom_s* state, crandom_normal_distr* d) {
    double v1, v2, sq, rt;
    if (d->_has_next++ & 1)
        return d->_next*d->stddev + d->mean;
    do {
        v1 = 2.0 * crandom_float_r(state) - 1.0;
        v2 = 2.0 * crandom_float_r(state) - 1.0;
        sq = v1*v1 + v2*v2;
    } while (sq >= 1.0 || sq == 0.0);
    rt = sqrt(-2.0 * log(sq) / sq);
    d->_next = v2*rt;
    return (v1*rt)*d->stddev + d->mean;
}

static inline double crandom_normal(crandom_normal_distr* d)
    { return crandom_normal_r(_romu3(), d); }

#endif // STC_RANDOM_H_INCLUDED