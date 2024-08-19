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

#if defined STC_RANDOM64 || (!defined STC_RANDOM32 && UINTPTR_MAX == UINT64_MAX)
typedef struct { uint64_t data[3]; } crandom_s;
typedef double crandom_real_type;
#define logf log
#define sqrtf sqrt

static inline void csrandom_r(crandom_s* state, uint64_t seed) {
    uint64_t* s = state->data;
    s[0] = seed*0x9e3779b97f4a7c15; s[0] ^= s[0] >> 30;
    s[1] = s[0]*0xbf58476d1ce4e5b9; s[1] ^= s[1] >> 27;
    s[2] = s[1]*0x94d049bb133111eb; s[2] ^= s[2] >> 31;
}
// Est. capacity = 2^75 bytes. Register pressure = 6. State size = 192 bits.
static inline uint64_t crandom_r(crandom_s* state) { // romu_trio64
    uint64_t* s = state->data;
    uint64_t xp = s[0], yp = s[1], zp = s[2];
    s[0] = zp * 15241094284759029579u;
    s[1] = yp - xp; s[1] = (s[1]<<12) | (s[1]>>52);
    s[2] = zp - yp; s[2] = (s[2]<<44) | (s[2]>>20);
    return xp;
}
static inline double crandom_real_r(crandom_s* state)
    { return (double)(int64_t)crandom_r(state) * (0.5/(1ull<<63)) + 0.5; }

static inline crandom_s* _romu3(void) {
    static crandom_s s = {{0xa4c1f32680f70c55,0x6f68261b57e7a770,0xe220a838bf5c9dde}};
    return &s;
}

#else // 32-bit

typedef struct { uint32_t data[3]; } crandom_s;
typedef float crandom_real_type;

static inline void csrandom_r(crandom_s* state, uint32_t seed) {
    uint32_t* s = state->data;
    s[0] = seed*0x9e3779b9; s[0] ^= s[0] >> 16;
    s[1] = s[0]*0x21f0aaad; s[1] ^= s[1] >> 15;
    s[2] = s[1]*0x735a2d97; s[2] ^= s[2] >> 15;
}
// Est. capacity >= 2^53 bytes. Register pressure = 5. State size = 96 bits.
static inline uint32_t crandom_r(crandom_s* state) { // romu_trio32
    uint32_t* s = state->data;
    uint32_t xp = s[0], yp = s[1], zp = s[2];
    s[0] = zp * 3323815723u;
    s[1] = yp - xp; s[1] = (s[1]<< 6) | (s[1]>>26);
    s[2] = zp - yp; s[2] = (s[2]<<22) | (s[2]>>10);
    return xp;
}
static inline float crandom_real_r(crandom_s* state)
    { return (float)(int32_t)crandom_r(state) * (0.5f/(1u<<31)) + 0.5f; }

static inline crandom_s* _romu3(void) {
    static crandom_s s = {{0xa4c1f326,0x6f68261b,0xe220a838}};
    return &s;
}
#endif

static inline void csrandom(uintptr_t seed)
    { csrandom_r(_romu3(), seed); }

static inline crandom_s crandom_make(uintptr_t seed)
    { crandom_s s; csrandom_r(&s, seed); return s; }

static inline uintptr_t crandom(void)
    { return crandom_r(_romu3()); }

static inline crandom_real_type crandom_real(void)
    { return crandom_real_r(_romu3()); }

// ===== crandom_uniform =====

typedef struct {
    int64_t low;
    uint32_t range, threshold;
} crandom_uniform_dist;

static inline crandom_uniform_dist
crandom_uniform_lowhigh(int64_t low, int64_t high) { // high - low < UINT32_MAX
    crandom_uniform_dist dist = {low, (uint32_t)(high - low + 1)};
    dist.threshold = (uint32_t)(0 - dist.range) % dist.range;
    return dist;
}

static inline int64_t
crandom_uniform_r(crandom_s* rng, crandom_uniform_dist* d) {
    uint64_t r;
    do { r = (uint32_t)crandom_r(rng)*(uint64_t)d->range; } while ((uint32_t)r < d->threshold);
    return d->low + (int64_t)(r >> 32);
}

static inline int64_t
crandom_uniform(crandom_uniform_dist* d)
    { return crandom_uniform_r(_romu3(), d); }

// ===== crandom_normal =====

typedef struct {
    crandom_real_type mean, stddev;
    crandom_real_type _next;
    int _has_next;
} crandom_normal_dist;

static inline crandom_real_type
crandom_normal_r(crandom_s* state, crandom_normal_dist* d) {
    crandom_real_type v1, v2, sq, rt;
    if (d->_has_next++ & 1)
        return d->_next*d->stddev + d->mean;
    do {
        v1 = 2*crandom_real_r(state) - 1;
        v2 = 2*crandom_real_r(state) - 1;
        sq = v1*v1 + v2*v2;
    } while (sq >= 1 || sq == 0);
    rt = sqrtf(-2*logf(sq)/sq);
    d->_next = v2*rt;
    return (v1*rt)*d->stddev + d->mean;
}

static inline crandom_real_type
crandom_normal(crandom_normal_dist* d)
    { return crandom_normal_r(_romu3(), d); }

#undef logf
#undef sqrtf
#endif // STC_RANDOM_H_INCLUDED
