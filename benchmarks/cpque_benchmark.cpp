#include <stdio.h>
#include <time.h>
#include <stc/crandom.h>
#include <stc/cvec.h>
#include <stc/cpque.h>

using_cvec(f, float);
using_cpque(f, cvec_f, >);

int main()
{
    uint32_t seed = time(NULL);
    stc64_t rng;
    int N = 10000000, M = 10;

    cpque_f pq = cpque_f_init();

    rng = stc64_init(seed);
    clock_t start = clock();
    c_forrange (i, int, N)
        cvec_f_push_back(&pq, (float) stc64_randf(&rng)*100000);

    cpque_f_make_heap(&pq);
    printf("Built priority queue: %f secs\n", (clock() - start) / (float) CLOCKS_PER_SEC);

    c_forrange (i, int, M) {
        printf("%g ", *cpque_f_top(&pq));
        cpque_f_pop(&pq);
    }

    start = clock();
    c_forrange (i, int, M, N)
        cpque_f_pop(&pq);
    printf("\n\npopped PQ: %f secs\n", (clock() - start) / (float) CLOCKS_PER_SEC);

    start = clock();
    c_forrange (i, int, N)
        cpque_f_push(&pq, (float) stc64_randf(&rng)*100000);
    printf("pushed PQ: %f secs\n", (clock() - start) / (float) CLOCKS_PER_SEC);

    c_forrange (i, int, M) {
        printf("%g ", *cpque_f_top(&pq));
        cpque_f_pop(&pq);
    }
    puts("");

    cpque_f_del(&pq);
}
