#ifndef CRANDOM__H__
#define CRANDOM__H__

#include "cdefs.h"
#include <string.h>

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
STC_API void mt19937_init(mt19937_t *state, uint32_t seed) {
    state->idx = mt19937_N;
    state->arr[0] = seed;
    for (int i = 1; i < mt19937_N; ++i) {
        seed = state->arr[i] = 1812433253 * (seed ^ (seed >> 30)) + i; 
    }
}
/* creates a new state from a seed */
STC_API mt19937_t mt19937_seed(uint32_t seed) {
    mt19937_t state;
    mt19937_init(&state, seed);
    return state;
}
/* creates a new state from default seed */
STC_API mt19937_t mt19937_default(void) {
    mt19937_t state;
    mt19937_init(&state, 5489);
    return state;
}

/* generates a random number on [0, 0xffffffff]-interval */
STC_API uint32_t mt19937_rand(mt19937_t *state) {
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
 * rotate bits left uint64
 */

STC_INLINE uint64_t c_rotateLeft64(uint64_t x, int bits) {
    return (x << bits) | (x >> (64 - bits));
}

/* 
 * xoroshiro128** with splitmix64 seed initialization
 */

STC_API uint64_t splitmix64(uint64_t *state) {
    uint64_t z = (*state += 0x9e3779b97f4a7c15);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
    z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
    return z ^ (z >> 31);
}

typedef struct xoroshiro128ss {
    uint64_t s[2];
} xoroshiro128ss_t;

STC_API xoroshiro128ss_t xoroshiro128ss_seed(uint64_t seed) {
    xoroshiro128ss_t state;
    state.s[0] = splitmix64(&seed);
    state.s[1] = splitmix64(&seed);
    return state;
}

STC_API uint64_t xoroshiro128ss_rand(xoroshiro128ss_t *state) {
    uint64_t *s = state->s, s1 = s[1];
    const uint64_t s0 = s[0];
    const uint64_t result = c_rotateLeft64(s0 * 5, 7) * 9;
    s1 ^= s0;
    s[0] = c_rotateLeft64(s0, 24) ^ s1 ^ (s1 << 16);
    s[1] = c_rotateLeft64(s1, 37);
    return result;
}

/*
 * sfc64: http://pracrand.sourceforge.net
 */

typedef struct sfc64 {
    uint64_t s[3], counter;
} sfc64_t;

STC_API uint64_t sfc64_rand(sfc64_t* state) {
    enum {LROT = 24, RSHIFT = 11, LSHIFT = 3};
    uint64_t *s = state->s;

    uint64_t result = s[0] + s[1] + ++state->counter;
    s[0] = s[1] ^ (s[1] >> RSHIFT);
    s[1] = s[2] + (s[2] << LSHIFT);
    s[2] = c_rotateLeft64(s[2], LROT) + result;
    return result;
}

STC_API sfc64_t sfc64_seed(const uint64_t seed) {
    sfc64_t state = {{seed, seed, seed}, 0};
    for (int i = 0; i < 12; ++i) sfc64_rand(&state);
    return state;
}

/*
 * siphash implementation.
 */

#if defined(_WIN32) || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    STC_INLINE uint64_t c_le64ToHost(uint64_t x) { return x; }
#elif defined(__APPLE__)
    #include <libkern/OSByteOrder.h>
    STC_INLINE uint64_t c_le64ToHost(uint64_t x) { return OSSwapLittleToHostInt64(x); }
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
    #include <sys/endian.h>
    STC_INLINE uint64_t c_le64ToHost(uint64_t x) { return letoh64(x); }
#elif defined(__linux__) || defined(__CYGWIN__) || defined(__GNUC__) || defined(__GNU_LIBRARY__)
    #include <endian.h>
    STC_INLINE uint64_t c_le64ToHost(uint64_t x) { return le64toh(x); }
#endif

typedef struct siphash_t {
    uint64_t padding, v[4];
    size_t length;
    int c, d;
} siphash_t;

/* c=2, d=4 or c=1, d=3 */
STC_API void siphash_init_c_d(siphash_t* s, const uint64_t key[2], const int c, const int d) {
    s->c = c;
    s->d = d;
    s->length = 0;
    s->padding = 0;
    s->v[0] = key[0] ^ 0x736f6d6570736575;
    s->v[1] = key[1] ^ 0x646f72616e646f6d;
    s->v[2] = key[0] ^ 0x6c7967656e657261;
    s->v[3] = key[1] ^ 0x7465646279746573;
}

/* default init 2-4 */
STC_API siphash_t siphash_init(const uint64_t key[2]) {
    siphash_t s;
    siphash_init_c_d(&s, key, 2, 4);
    return s;
}

#define _siphash_halfRound(i, j, a, b, c, d) \
    (a += b, \
     c += d, \
     b = c_rotateLeft64(b, i) ^ a, \
     d = c_rotateLeft64(d, j) ^ c, \
     a = c_rotateLeft64(a, 32))

#define _siphash_compress(rounds, v) \
    for (int r = 0; r < rounds; ++r) { \
        _siphash_halfRound(13, 16, v[0], v[1], v[2], v[3]); \
        _siphash_halfRound(17, 21, v[2], v[1], v[0], v[3]); \
    }

#define _siphash_digest(rounds, v, m) { \
        uint64_t _m = m; \
        v[3] ^= _m; \
        _siphash_compress(rounds, v); \
        v[0] ^= _m; \
    }

STC_API void siphash_update(siphash_t* s, const void* bytes, size_t size) {
    union { const uint8_t* u8; const uint64_t* u64; } in;
    in.u8 = (const uint8_t*) bytes;
    size_t offset = s->length & 7;
    uint64_t *v = s->v;
    s->length += size;

    if (offset) {
        size_t end = offset + size;
        size -= 8 - offset;
        while (offset < end && offset < 8) {
            s->padding |= ((uint64_t) *in.u8++) << (offset++ << 3);
        }
        if (end < 8) return;

        _siphash_digest(s->c, v, s->padding);
        s->padding = 0;
    }
    size_t n_words = size >> 3;
    uint64_t m;

    while (n_words--) {
        memcpy(&m, in.u64++, 8);
        _siphash_digest(s->c, v, c_le64ToHost(m));
    }
    switch (s->length & 7) {
        case 7: s->padding |= ((uint64_t) in.u8[6]) << 48;
        case 6: s->padding |= ((uint64_t) in.u8[5]) << 40;
        case 5: s->padding |= ((uint64_t) in.u8[4]) << 32;
        case 4: s->padding |= ((uint64_t) in.u8[3]) << 24;
        case 3: s->padding |= ((uint64_t) in.u8[2]) << 16;
        case 2: s->padding |= ((uint64_t) in.u8[1]) << 8;
        case 1: s->padding |= ((uint64_t) in.u8[0]);
    }
}

STC_API uint64_t siphash_finalize(siphash_t* s) {
    uint64_t *v = s->v;
    _siphash_digest(s->c, v, s->padding | (s->length << 56));
    v[2] ^= 0xff;
    _siphash_compress(s->d, v);
    return v[0] ^ v[1] ^ v[2] ^ v[3];
}
/* c=2, d=4 or c=1, d=3 */
STC_API uint64_t siphash_hash_c_d(const uint64_t key[2], const void* bytes, const uint64_t size, const int c, const int d) {
    siphash_t s;
    siphash_init_c_d(&s, key, c, d);
    siphash_update(&s, bytes, size);
    return siphash_finalize(&s);
}

/* default hash 2-4 */
STC_API uint64_t siphash_hash(const uint64_t key[2], const void* bytes, const uint64_t size) {
    return siphash_hash_c_d(key, bytes, size, 2, 4);
}

#endif
