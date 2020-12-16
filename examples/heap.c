#include <stdio.h>
#include <time.h>
#include <stc/crand.h>
#include <stc/cvec.h>
#include <stc/cpqueue.h>

using_cvec(f, float);
using_cpqueue(f, cvec_f, >);

int main()
{
    uint32_t seed = time(NULL);
    crand_t rng;
    int N = 3000000, M = 100;

    cpqueue_f pq = cpqueue_f_init();

    rng = crand_init(seed);
    clock_t start = clock();
    c_forrange (i, int, N)
        cvec_f_push_back(&pq, (float) crand_nextf(&rng)*100000);

    cpqueue_f_make_heap(&pq);
    printf("Built priority queue: %f secs\n", (clock() - start) / (float) CLOCKS_PER_SEC);

    c_forrange (i, int, M) {
        printf("%g ", *cpqueue_f_top(&pq));
        cpqueue_f_pop(&pq);
    }

    start = clock();
    c_forrange (i, int, M, N)
        cpqueue_f_pop(&pq);
    printf("\n\npopped PQ: %f secs\n", (clock() - start) / (float) CLOCKS_PER_SEC);

    start = clock();
    c_forrange (i, int, N)
        cpqueue_f_push(&pq, (float) crand_nextf(&rng)*100000);
    printf("pushed PQ: %f secs\n", (clock() - start) / (float) CLOCKS_PER_SEC);

    c_forrange (i, int, M) {
        printf("%g ", *cpqueue_f_top(&pq));
        cpqueue_f_pop(&pq);
    }
    puts("");

    cpqueue_f_del(&pq);
}
