#include <stdio.h>
#include <time.h>
#include <stc/crandom.h>
#include <stc/cvec.h>
#include <stc/cpqueue.h>

declare_cvec(f, float);
declare_cpqueue(f, >, cvec);

int main()
{
    uint32_t seed = time(NULL);
    crand_rng32_t pcg;
    int N = 3000000, M = 100;

    cpqueue_f pq = cpqueue_f_init();
    
    pcg = crand_rng32_init(seed);
    clock_t start = clock();
    for (int i=0; i<N; ++i)
        cvec_f_push_back(&pq, crand_i32(&pcg));
    
    cpqueue_f_build(&pq);
    printf("Built priority queue: %f secs\n", (clock() - start) / (float) CLOCKS_PER_SEC);

    for (int i=0; i<M; ++i)
        printf("%.0f ", *cpqueue_f_top(&pq)), cpqueue_f_pop(&pq);
    
    start = clock();
    for (int i=M; i<N; ++i)
        cpqueue_f_pop(&pq);
    printf("\n\npopped PQ: %f secs\n", (clock() - start) / (float) CLOCKS_PER_SEC);

    pcg = crand_rng32_init(seed);
    start = clock();
    for (int i=0; i<N; ++i)
        cpqueue_f_push(&pq, crand_i32(&pcg));
    printf("pushed PQ: %f secs\n", (clock() - start) / (float) CLOCKS_PER_SEC);
    
    for (int i=0; i<M; ++i)
        printf("%.0f ", *cpqueue_f_top(&pq)), cpqueue_f_pop(&pq);
    puts("");

    cpqueue_f_destroy(&pq);
}
