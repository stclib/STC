#include <stdio.h>
#include <time.h>
#include <stc/crand.h>
#ifdef __cplusplus
#include <random>
#endif


#define NN 1000000000

int main(void)
{
    clock_t difference, before;
    uint64_t v;
    printf("start\n");

    crand_eng32_t pcg = crand_eng32(time(NULL));
    before = clock(); \
    v = 0;
    for (size_t i=0; i<NN; i++) {
        v += crand_gen_i32(&pcg);
    }
    difference = clock() - before;
    printf("pcg32: %.02f, %zu\n", (float) difference / CLOCKS_PER_SEC, v);

    crand_eng64_t sfc = crand_eng64(time(NULL));
    before = clock(); \
    v = 0;
    for (size_t i=0; i<NN; i++) {
        v += crand_gen_i64(&sfc) & 0xffffffff;
    }
    difference = clock() - before;
    printf("sfc64: %.02f, %zu\n", (float) difference / CLOCKS_PER_SEC, v);

    crand_f64_uniform_t fdist = crand_f64_uniform(10, 20);
    for (int i=0; i<8; ++i) printf("%f ", crand_gen_f64_uniform(&sfc, fdist));
    puts("");
    crand_f32_uniform_t f32dist = crand_f32_uniform(10, 20);
    for (int i=0; i<8; ++i) printf("%f ", crand_gen_f32_uniform(&pcg, f32dist));
    puts("");
}