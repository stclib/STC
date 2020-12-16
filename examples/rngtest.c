#include <stdio.h>
#include <time.h>
#include <stc/crand.h>
#ifdef __cplusplus
#include <random>
#endif


#define NN 1000000000

int main(void)
{
    clock_t diff, before;
    uint64_t v;
    
    crand_t rng = crand_init(time(NULL));
    crand_uniform_t idist = crand_uniform_init(10, 20);
    crand_uniformf_t fdist = crand_uniformf_init(10, 20);

    before = clock();
    v = 0;
    c_forrange (NN) {
        v += crand_next(&rng);
    }
    diff = clock() - before;
    printf("stc64_rand: %.02f, %zu\n", (float) diff / CLOCKS_PER_SEC, v);

    before = clock();
    v = 0;
    c_forrange (NN) {
        v += crand_uniform(&rng, &idist);
    }
    diff = clock() - before;
    printf("stc64_uniform: %.02f, %zu\n\n", (float) diff / CLOCKS_PER_SEC, v);

    c_forrange (30) printf("%02zd ", crand_uniform(&rng, &idist)); 
    puts("");
    c_forrange (8) printf("%f ", crand_uniformf(&rng, &fdist));
    puts("");
}