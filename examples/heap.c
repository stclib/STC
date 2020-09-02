#include <stdio.h>
#include <time.h>
#include <stc/crandom.h>
#include <stc/cvec.h>
#include <stc/cprique.h>

declare_cvec(f, float);
declare_cprique(f, cvec_f, >);

int main()
{
    uint32_t seed = time(NULL);
    crand_rng32_t pcg;
    int N = 3000000, M = 100;

    cprique_f pq = cprique_f_init();
    
    pcg = crand_rng32_init(seed);
    clock_t start = clock();
    for (int i=0; i<N; ++i)
        cvec_f_push_back(&pq, crand_i32(&pcg));
    
    cprique_f_build(&pq);
    printf("Built priority queue: %f secs\n", (clock() - start) / (float) CLOCKS_PER_SEC);

    for (int i=0; i<M; ++i)
        printf("%.0f ", *cprique_f_top(&pq)), cprique_f_pop(&pq);
    
    start = clock();
    for (int i=M; i<N; ++i)
        cprique_f_pop(&pq);
    printf("\n\npopped PQ: %f secs\n", (clock() - start) / (float) CLOCKS_PER_SEC);

    pcg = crand_rng32_init(seed);
    start = clock();
    for (int i=0; i<N; ++i)
        cprique_f_push(&pq, crand_i32(&pcg));
    printf("pushed PQ: %f secs\n", (clock() - start) / (float) CLOCKS_PER_SEC);
    
    for (int i=0; i<M; ++i)
        printf("%.0f ", *cprique_f_top(&pq)), cprique_f_pop(&pq);
    puts("");

    cprique_f_destroy(&pq);
}
