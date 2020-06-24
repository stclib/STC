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

    pcg32_random_t pcg = pcg32_seed(time(NULL), 1);
    before = clock(); \
    v = 0;
    for (size_t i=0; i<NN; i++) {
        v += pcg32_random(&pcg) & 0xffffffff;
    }
    difference = clock() - before;
    printf("pcg32: %.02f, %zu\n", (float) difference / CLOCKS_PER_SEC, v);

    sfc64_random_t sfc = sfc64_seed(time(NULL));
    before = clock(); \
    v = 0;
    for (size_t i=0; i<NN; i++) {
        v += sfc64_random(&sfc) & 0xffffffff;
    }
    difference = clock() - before;
    printf("sfc64: %.02f, %zu\n", (float) difference / CLOCKS_PER_SEC, v);

    for (int i=0; i<8; ++i) printf("%f ", sfc64_fRandom(&sfc));
    puts("");
    for (int i=0; i<8; ++i) printf("%f ", pcg32_fRandom(&pcg));
    puts("");
}