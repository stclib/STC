#include <stdio.h>
#include <time.h>
#include <stc/crandom.h>
#include <stc/cvec.h>
#include <stc/cpqueue.h>

using_cvec(f, float);
using_cpqueue(f, cvec_f, >);

int main()
{
    uint32_t seed = time(NULL);
    crand_rng32_t pcg;
    int N = 3000000, M = 100;

    cpqueue_f pq = cpqueue_f_init();
    
    pcg = crand_rng32_init(seed);
    clock_t start = clock();
    c_forrange (i, int, N)
        cvec_f_push_back(&pq, (float) crand_i32(&pcg));
    
    cpqueue_f_make_heap(&pq);
    printf("Built priority queue: %f secs\n", (clock() - start) / (float) CLOCKS_PER_SEC);

    c_forrange (i, int, M)
        printf("%.0f ", *cpqueue_f_top(&pq)), cpqueue_f_pop(&pq);
    
    start = clock();
    c_forrange (i, int, M, N)
        cpqueue_f_pop(&pq);
    printf("\n\npopped PQ: %f secs\n", (clock() - start) / (float) CLOCKS_PER_SEC);

    pcg = crand_rng32_init(seed);
    start = clock();
    c_forrange (i, int, N)
        cpqueue_f_push(&pq, (float) crand_i32(&pcg));
    printf("pushed PQ: %f secs\n", (clock() - start) / (float) CLOCKS_PER_SEC);
    
    c_forrange (i, int, M)
        printf("%.0f ", *cpqueue_f_top(&pq)), cpqueue_f_pop(&pq);
    puts("");

    cpqueue_f_del(&pq);
}
