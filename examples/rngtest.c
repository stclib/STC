#include <stdio.h>
#include <time.h>
#include <stc/crandom.h>
#ifdef __cplusplus
#include <random>
#endif


#define NN 1000000000

int main(void)
{
    clock_t diff, before;
    uint64_t v;
    
    cstc64_t stc = cstc64_init(time(NULL));
    cstc64_uniform_t idist = cstc64_uniform_init(10, 20);
    cstc64_uniformf_t fdist = cstc64_uniformf_init(10, 20);

    before = clock();
    v = 0;
    c_forrange (NN) {
        v += cstc64_rand(&stc);
    }
    diff = clock() - before;
    printf("stc64_rand: %.02f, %zu\n", (float) diff / CLOCKS_PER_SEC, v);

    before = clock();
    v = 0;
    c_forrange (NN) {
        v += cstc64_uniform(&stc, &idist);
    }
    diff = clock() - before;
    printf("stc64_uniform: %.02f, %zu\n\n", (float) diff / CLOCKS_PER_SEC, v);

    c_forrange (30) printf("%02zd ", cstc64_uniform(&stc, &idist)); 
    puts("");
    c_forrange (8) printf("%f ", cstc64_uniformf(&stc, &fdist));
    puts("");
}