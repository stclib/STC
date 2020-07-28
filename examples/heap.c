#include <stdio.h>
#include <time.h>
#include "stc/cvecpq.h"
#include "stc/crandom.h"

declare_cvec(f, float);
declare_cvec_priority_queue(f, >);

int main()
{
    uint32_t seed = time(NULL);
    crandom32_t pcg = crandom32_uniform_engine(seed);
    int N = 30000000, M = 100;
    cvec_f vec = cvec_init;
    clock_t start = clock();
    for (int i=0; i<N; ++i)
        cvec_f_push_back(&vec, crandom32_uniform_int(&pcg));
    cvecpq_f_build(&vec);
    printf("Built priority queue: %f secs\n", (clock() - start) / (float) CLOCKS_PER_SEC);

    for (int i=0; i<M; ++i)
        printf("%.0f ", cvecpq_f_top(&vec)), cvecpq_f_pop(&vec);
    start = clock();
    for (int i=M; i<N; ++i)
        cvecpq_f_pop(&vec);
    printf("\n\npopped PQ: %f secs\n", (clock() - start) / (float) CLOCKS_PER_SEC);

    pcg = crandom32_uniform_engine(seed);
    start = clock();
    for (int i=0; i<N; ++i)
        cvecpq_f_push(&vec, crandom32_uniform_int(&pcg));
    printf("pushed PQ: %f secs\n", (clock() - start) / (float) CLOCKS_PER_SEC);
    for (int i=0; i<M; ++i)
        printf("%.0f ", cvecpq_f_top(&vec)), cvecpq_f_pop(&vec);
    puts("");
}
