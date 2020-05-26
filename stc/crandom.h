#ifndef CRANDOM__H__
#define CRANDOM__H__
/*
     Mersenne Twister random number generator MT19937, 32 bit.
     A C-program for MT19937, with initialization improved 2002/1/26.
     Coded by Takuji Nishimura and Makoto Matsumoto.
     Optimized by Tyge Løvset.

     Before using, initialize the state by using mt19937_init(),
     mt19937_seed() or mt19937_default().

     Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
     All rights reserved.                                                    

     Redistribution and use in source and binary forms, with or without
     modification, are permitted provided that the following conditions
     are met:

         1. Redistributions of source code must retain the above copyright
                notice, this list of conditions and the following disclaimer.

         2. Redistributions in binary form must reproduce the above copyright
                notice, this list of conditions and the following disclaimer in the
                documentation and/or other materials provided with the distribution.

         3. The names of its contributors may not be used to endorse or promote 
                products derived from this software without specific prior written 
                permission.

     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
     "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
     LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
     A PARTICULAR PURPOSE ARE DISCLAIMED.    IN NO EVENT SHALL THE COPYRIGHT OWNER OR
     CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
     EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
     PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
     PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
     LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
     NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

     Any feedback is very welcome.
     http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
     email: m-mat @ math.sci.hiroshima-u.ac.jp (remove space)
*/

#include <stdint.h>

enum { /* period parameters */
    mt19937_N = 624,
    mt19937_M = 397,
};

typedef struct mt19937 {
    uint32_t idx;
    uint32_t arr[mt19937_N]; /* the array for the state vector */
} mt19937_t;

/* initializes state with a seed */
static inline void mt19937_init(mt19937_t *state, uint32_t seed) {
    state->idx = mt19937_N;
    state->arr[0] = seed;
    for (int i = 1; i < mt19937_N; ++i) {
        seed = state->arr[i] = 1812433253 * (seed ^ (seed >> 30)) + i; 
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array arr[].                       */
        /* 2002/01/09 modified by Makoto Matsumoto             */
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

#endif
