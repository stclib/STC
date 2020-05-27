#ifndef CRANDOM__H__
#define CRANDOM__H__

#include <stdint.h>

/*
 * Mersenne Twister random number generator MT19937, 32 bit.
 */

enum {
    mt19937_N = 624,
    mt19937_M = 397,
};

typedef struct mt19937 {
    uint32_t idx;
    uint32_t arr[mt19937_N];
} mt19937_t;

/* initializes state with a seed */
static inline void mt19937_init(mt19937_t *state, uint32_t seed) {
    state->idx = mt19937_N;
    state->arr[0] = seed;
    for (int i = 1; i < mt19937_N; ++i) {
        seed = state->arr[i] = 1812433253 * (seed ^ (seed >> 30)) + i; 
    }
}
/* creates a new state from a seed */
static inline mt19937_t mt19937_seed(uint32_t seed) {
    mt19937_t state;
    mt19937_init(&state, seed);
    return state;
}
/* creates a new state from default seed */
static inline mt19937_t mt19937_default(void) {
    mt19937_t state;
    mt19937_init(&state, 5489);
    return state;
}

/* generates a random number on [0, 0xffffffff]-interval */
static inline uint32_t mt19937_rand(mt19937_t *state) {
    enum {N = mt19937_N, M = mt19937_M};

    uint32_t y, *arr = state->arr;
    if (state->idx >= N) { /* generate N words at one time */
        int k = 0;
        for (; k < N-M; ++k) {
            y = (arr[k] & 0x80000000) | (arr[k + 1] & 0x7fffffff);
            arr[k] = arr[k + M] ^ (y >> 1) ^ ((y & 1) ? 0x9908b0df : 0);
        }
        for (; k < N-1; ++k) {
            y = (arr[k] & 0x80000000) | (arr[k + 1] & 0x7fffffff);
            arr[k] = arr[k - (N-M)] ^ (y >> 1) ^ ((y & 1) ? 0x9908b0df : 0);
        }
        y = (arr[N-1] & 0x80000000) | (arr[0] & 0x7fffffff);
        arr[N-1] = arr[M-1] ^ (y >> 1) ^ ((y & 1) ? 0x9908b0df : 0);

        state->idx = 0;
    }
    /* tempering */
    y = arr[state->idx++];
    y ^= (y >> 11);
    y ^= (y <<  7) & 0x9d2c5680;
    y ^= (y << 15) & 0xefc60000;
    y ^= (y >> 18);
    return y;
}

/* 
 * xoroshiro128** with splitmix64 seed initialization
 */

static inline uint64_t splitmix64(uint64_t *state) {
    uint64_t z = (*state += 0x9e3779b97f4a7c15);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
    z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
    return z ^ (z >> 31);
}

static inline uint64_t c_rotl(uint64_t x, int s) {
    return (x << s) | (x >> (64 - s));
}

typedef struct xoroshiro128ss {
    uint64_t s[2];
} xoroshiro128ss_t;

static inline xoroshiro128ss_t xoroshiro128ss_seed(uint64_t seed) {
    xoroshiro128ss_t state;
    state.s[0] = splitmix64(&seed);
    state.s[1] = splitmix64(&seed);
    return state;
}

static inline uint64_t xoroshiro128ss_rand(xoroshiro128ss_t *state) {
    uint64_t *s = state->s, s1 = s[1];
    const uint64_t s0 = s[0];
    const uint64_t result = c_rotl(s0 * 5, 7) * 9;
    s1 ^= s0;
    s[0] = c_rotl(s0, 24) ^ s1 ^ (s1 << 16);
    s[1] = c_rotl(s1, 37);
    return result;
}

/*
 * sfc64: http://pracrand.sourceforge.net
 */

typedef struct sfc64 {
    uint64_t s[3], counter;
} sfc64_t;

static inline uint64_t sfc64_rand(sfc64_t* state) {
    enum {LROT = 24, RSHIFT = 11, LSHIFT = 3};
    uint64_t *s = state->s;

    uint64_t result = s[0] + s[1] + ++state->counter;
    s[0] = s[1] ^ (s[1] >> RSHIFT);
    s[1] = s[2] + (s[2] << LSHIFT);
    s[2] = c_rotl(s[2], LROT) + result;
    return result;
}

static inline sfc64_t sfc64_seed(uint64_t seed) {
    sfc64_t state = {{seed, seed, seed}, 0};
    for (int i = 0; i < 12; ++i) sfc64_rand(&state);
    return state;
}

#endif
