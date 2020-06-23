#include <stdio.h>
#include <time.h>
#include "../stc/crandom.h"
#ifdef __cplusplus
#include <random>
#endif


#define NN 1000000000

int main(void)
{
    clock_t difference, before;
    uint64_t v;
    printf("start\n");

    mt19937_t state = mt19937_default();
    uint32_t k = mt19937_rand(&state);
    printf("%u - %g\n", k, c_randToFloat(k));

    before = clock(); \
    v = 0;
    for (size_t i=0; i<NN; i++) {
        v += mt19937_rand(&state);
    }
    difference = clock() - before;
    printf("my-mt: %.02f, %zu\n", (float) difference / CLOCKS_PER_SEC, v);

#ifdef __cplusplus
    std::mt19937 mt_rand;
    before = clock(); \
    v = 0;
    for (size_t i=0; i<NN; i++) {
        v += mt_rand();
    }
    difference = clock() - before;
    printf("c++mt: %.02f, %zu\n", (float) difference / CLOCKS_PER_SEC, v);
#endif

    xoroshiro128ss_t xo = xoroshiro128ss_seed(1234);
    before = clock(); \
    v = 0;
    for (size_t i=0; i<NN; i++) {
        v += xoroshiro128ss_rand(&xo) & 0xffffffff;
    }
    difference = clock() - before;
    printf("xoros: %.02f, %zu\n", (float) difference / CLOCKS_PER_SEC, v);


    sfc64_t sfc = sfc64_seed(1234);
    before = clock(); \
    v = 0;
    for (size_t i=0; i<NN; i++) {
        v += sfc64_rand(&sfc) & 0xffffffff;
    }
    difference = clock() - before;
    printf("sfc64: %.02f, %zu\n", (float) difference / CLOCKS_PER_SEC, v);
}