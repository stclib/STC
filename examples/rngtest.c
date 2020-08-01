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

    crandom_eng32_t pcg = crandom_eng32_init(time(NULL));
    before = clock(); \
    v = 0;
    for (size_t i=0; i<NN; i++) {
        v += crandom_gen_i32(&pcg);
    }
    difference = clock() - before;
    printf("pcg32: %.02f, %zu\n", (float) difference / CLOCKS_PER_SEC, v);

    crandom_eng64_t sfc = crandom_eng64_init(time(NULL));
    before = clock(); \
    v = 0;
    for (size_t i=0; i<NN; i++) {
        v += crandom_gen_i64(&sfc) & 0xffffffff;
    }
    difference = clock() - before;
    printf("sfc64: %.02f, %zu\n", (float) difference / CLOCKS_PER_SEC, v);

    crandom_uniform_i32_t i32dist = crandom_uniform_i32_init(10, 20);
    for (int i=0; i<8; ++i) printf("%d ", crandom_uniform_i32(&pcg, i32dist));
    puts("");

    crandom_uniform_f32_t f32dist = crandom_uniform_f32_init(10, 20);
    for (int i=0; i<8; ++i) printf("%f ", crandom_uniform_f32(&pcg, f32dist));
    puts("");

    crandom_uniform_f64_t fdist = crandom_uniform_f64_init(10, 20);
    for (int i=0; i<8; ++i) printf("%f ", crandom_uniform_f64(&sfc, fdist));
    //for (int i=0; i<8; ++i) printf("%zu ", crandom_gen_i64(&sfc));
    puts("");
}