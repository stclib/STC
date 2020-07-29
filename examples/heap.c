#include <stdio.h>
#include <time.h>
#include "stc/cvecpq.h"
#include "stc/crand.h"

declare_cvec(f, float);
declare_cvec_priority_queue(f, >);

int main()
{
    uint32_t seed = time(NULL);
    crand_eng32_t pcg = crand_eng32(seed);
    int N = 30000000, M = 100;
    cvec_f vec = cvec_init;
    clock_t start = clock();
    for (int i=0; i<N; ++i)
        cvec_f_push_back(&vec, crand_gen_i32(&pcg));
    cvecpq_f_build(&vec);
    printf("Built priority queue: %f secs\n", (clock() - start) / (float) CLOCKS_PER_SEC);

    for (int i=0; i<M; ++i)
        printf("%.0f ", cvecpq_f_top(&vec)), cvecpq_f_pop(&vec);
    start = clock();
    for (int i=M; i<N; ++i)
        cvecpq_f_pop(&vec);
    printf("\n\npopped PQ: %f secs\n", (clock() - start) / (float) CLOCKS_PER_SEC);

    pcg = crand_eng32(seed);
    start = clock();
    for (int i=0; i<N; ++i)
        cvecpq_f_push(&vec, crand_gen_i32(&pcg));
    printf("pushed PQ: %f secs\n", (clock() - start) / (float) CLOCKS_PER_SEC);
    for (int i=0; i<M; ++i)
        printf("%.0f ", cvecpq_f_top(&vec)), cvecpq_f_pop(&vec);
    puts("");
}
