#include <stdio.h>
#include <time.h>
#include <stc/crandom.h>
#ifdef __cplusplus
#include <random>
#endif


#define NN 1000000000

int main(void)
{
    clock_t difference, before;
    uint64_t v;
    printf("start\n");

    CRand32 pcg = crand32_init(time(NULL));
    before = clock(); \
    v = 0;
    for (size_t i=0; i<NN; i++) {
        v += crand32_gen(&pcg);
    }
    difference = clock() - before;
    printf("pcg32: %.02f, %zu\n", (float) difference / CLOCKS_PER_SEC, v);

    CRand64 sfc = crand64_init(time(NULL));
    before = clock(); \
    v = 0;
    for (size_t i=0; i<NN; i++) {
        v += crand64_gen(&sfc) & 0xffffffff;
    }
    difference = clock() - before;
    printf("sfc64: %.02f, %zu\n", (float) difference / CLOCKS_PER_SEC, v);

    for (int i=0; i<8; ++i) printf("%f ", crand32_genReal(&pcg));
    puts("");
    for (int i=0; i<8; ++i) printf("%f ", crand64_genReal(&sfc));
    puts("");
}