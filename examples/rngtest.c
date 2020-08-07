#include <stdio.h>
#include <time.h>
#include <stc/crandom.h>
#ifdef __cplusplus
#include <random>
#endif


#define NN 3000000000

int main(void)
{
    clock_t difference, before;
    uint64_t v;
    
    crandom_eng64_t sfc = crandom_eng64_init(time(NULL));
    crandom_distrib_i32_t i32dist = crandom_uniform_i32_init(10, 20);
    crandom_distrib_f32_t f32dist = crandom_uniform_f32_init(10, 20);

    crandom_distrib_i64_t idist = crandom_uniform_i64_init(10, 20);
    crandom_distrib_f64_t fdist = crandom_uniform_f64_init(10, 20);

    for (int i=0; i<30; ++i) printf("%02zd ", crandom_uniform_i64(&sfc, idist)); 
    puts("");

    crandom_eng32_t pcg = crandom_eng32_init(time(NULL));
    before = clock(); \
    v = 0;
    for (size_t i=0; i<NN; i++) {
        //v += crandom_i32(&pcg);
        v += crandom_uniform_i32(&pcg, i32dist);
    }
    difference = clock() - before;
    printf("pcg32: %.02f, %zu\n", (float) difference / CLOCKS_PER_SEC, v);

    before = clock(); \
    v = 0;
    for (size_t i=0; i<NN; i++) {
        //v += crandom_i64(&sfc) & 0xffffffff;
        v += crandom_uniform_i64(&sfc, idist);
    }
    difference = clock() - before;
    printf("sfc64: %.02f, %zu\n", (float) difference / CLOCKS_PER_SEC, v);

    
    for (int i=0; i<8; ++i) printf("%d ", crandom_uniform_i32(&pcg, i32dist));
    puts("");

    
    for (int i=0; i<8; ++i) printf("%f ", crandom_uniform_f32(&pcg, f32dist));
    puts("");
    
    for (int i=0; i<8; ++i) printf("%f ", crandom_uniform_f64(&sfc, fdist));
    puts("");
}