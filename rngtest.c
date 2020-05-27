#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "stc/crandom.h"
#ifdef __cplusplus
#include <random>
#endif



/* Period parameters */  
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */

static unsigned long mt[N]; /* the array for the state vector  */
static int mti=N+1; /* mti==N+1 means mt[N] is not initialized */

/* initializes mt[N] with a seed */
void init_genrand(unsigned long s)
{
    mt[0]= s & 0xffffffffUL;
    for (mti=1; mti<N; mti++) {
        mt[mti] = 
        (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti); 
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array mt[].                        */
        /* 2002/01/09 modified by Makoto Matsumoto             */
        mt[mti] &= 0xffffffffUL;
        /* for >32 bit machines */
    }
}

/* generates a random number on [0,0xffffffff]-interval */
unsigned long genrand_int32(void)
{
    unsigned long y;
    static unsigned long mag01[2]={0x0UL, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mti >= N) { /* generate N words at one time */
        int kk;

        if (mti == N+1)   /* if init_genrand() has not been called, */
            init_genrand(5489UL); /* a default initial seed is used */

        for (kk=0;kk<N-M;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (;kk<N-1;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
        mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mti = 0;
    }
  
    y = mt[mti++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}



#define NN 1000000000

int main(void)
{
    clock_t difference, before;
    uint64_t v;
    printf("start\n");

    before = clock(); \
    v = 0;
    for (size_t i=0; i<NN; i++) {
        v += genrand_int32();
    }
    difference = clock() - before;
    printf("refmt: %.02f, %llu\n", (float) difference / CLOCKS_PER_SEC, v);


    mt19937_t state = mt19937_default();

    before = clock(); \
    v = 0;
    for (size_t i=0; i<NN; i++) {
        v += mt19937_rand(&state);
    }
    difference = clock() - before;
    printf("mymt : %.02f, %llu\n", (float) difference / CLOCKS_PER_SEC, v);

#ifdef __cplusplus
    std::mt19937 mt_rand;
    before = clock(); \
    v = 0;
    for (size_t i=0; i<NN; i++) {
        v += mt_rand();
    }
    difference = clock() - before;
    printf("c++mt: %.02f, %llu\n", (float) difference / CLOCKS_PER_SEC, v);
#endif

    xoroshiro128ss_t xo = xoroshiro128ss_seed(1234);
    before = clock(); \
    v = 0;
    for (size_t i=0; i<NN; i++) {
        v += xoroshiro128ss_rand(&xo) & 0xffffffff;
    }
    difference = clock() - before;
    printf("xoros: %.02f, %llu\n", (float) difference / CLOCKS_PER_SEC, v);


    sfc64_t sfc = sfc64_seed(1234);
    before = clock(); \
    v = 0;
    for (size_t i=0; i<NN; i++) {
        v += sfc64_rand(&sfc) & 0xffffffff;
    }
    difference = clock() - before;
    printf("sfc64: %.02f, %llu\n", (float) difference / CLOCKS_PER_SEC, v);


    before = clock(); \
    v = 0;
    for (size_t i=0; i<NN; i++) {
        v += rand();
    }
    difference = clock() - before;
    printf("rand : %.02f, %llu\n", (float) difference / CLOCKS_PER_SEC, v);
}