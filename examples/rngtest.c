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
    
    crand_rng64_t stc = crand_rng64_init(time(NULL));
    crand_uniform_i64_t idist = crand_uniform_i64_init(stc, 10, 20);
    crand_uniform_f64_t fdist = crand_uniform_f64_init(stc, 10, 20);

    for (int i=0; i<30; ++i) printf("%02zd ", crand_uniform_i64(&idist)); 
    puts("");

    crand_rng32_t pcg = crand_rng32_init(time(NULL));
    crand_uniform_i32_t i32dist = crand_uniform_i32_init(pcg, 10, 20);
    crand_uniform_f32_t f32dist = crand_uniform_f32_init(pcg, 10, 20);

    before = clock(); \
    v = 0;
    for (size_t i=0; i<NN; i++) {
        //v += crand_i32(&pcg);
        v += crand_uniform_i32(&i32dist);
    }
    difference = clock() - before;
    printf("pcg32: %.02f, %zu\n", (float) difference / CLOCKS_PER_SEC, v);

    before = clock(); \
    v = 0;
    for (size_t i=0; i<NN; i++) {
        //v += crand_i64(&stc) & 0xffffffff;
        v += crand_uniform_i64(&idist);
    }
    difference = clock() - before;
    printf("stc64: %.02f, %zu\n", (float) difference / CLOCKS_PER_SEC, v);

    
    for (int i=0; i<8; ++i) printf("%d ", crand_uniform_i32(&i32dist));
    puts("");

    
    for (int i=0; i<8; ++i) printf("%f ", crand_uniform_f32(&f32dist));
    puts("");
    
    for (int i=0; i<8; ++i) printf("%f ", crand_uniform_f64(&fdist));
    puts("");
}