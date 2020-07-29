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

    crand_eng32_t pcg = crand_eng32_init(time(NULL));
    before = clock(); \
    v = 0;
    for (size_t i=0; i<NN; i++) {
        v += crand_gen_i32(&pcg);
    }
    difference = clock() - before;
    printf("pcg32: %.02f, %zu\n", (float) difference / CLOCKS_PER_SEC, v);

    crand_eng64_t sfc = crand_eng64_init(time(NULL));
    before = clock(); \
    v = 0;
    for (size_t i=0; i<NN; i++) {
        v += crand_gen_i64(&sfc) & 0xffffffff;
    }
    difference = clock() - before;
    printf("sfc64: %.02f, %zu\n", (float) difference / CLOCKS_PER_SEC, v);

    crand_uniform_i32_t i32dist = crand_uniform_i32_init(10, 20);
    for (int i=0; i<8; ++i) printf("%d ", crand_uniform_i32(&pcg, i32dist));
    puts("");

    crand_uniform_f32_t f32dist = crand_uniform_f32_init(10, 20);
    for (int i=0; i<8; ++i) printf("%f ", crand_uniform_f32(&pcg, f32dist));
    puts("");

    crand_uniform_f64_t fdist = crand_uniform_f64_init(10, 20);
    for (int i=0; i<8; ++i) printf("%f ", crand_uniform_f64(&sfc, fdist));
    //for (int i=0; i<8; ++i) printf("%zu ", crand_gen_i64(&sfc));
    puts("");
}