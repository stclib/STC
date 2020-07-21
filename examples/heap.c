#include <stdio.h>
#include <time.h>
#include "stc/cvecpq.h"
#include "stc/crandom.h"

declare_CVec(f, float);
declare_CVec_priorityQ(f, >);

int main()
{
    uint32_t seed = time(NULL);
    pcg32_random_t pcg = pcg32_seed(seed, 0);
    int N = 30000000, M = 100;
    CVec_f vec = cvec_init;
    clock_t start = clock();
    for (int i=0; i<N; ++i)
        cvec_f_pushBack(&vec, pcg32_random(&pcg));
    cvec_f_buildPriorityQ(&vec);
    printf("Built priority queue: %f secs\n", (clock() - start) / (float) CLOCKS_PER_SEC);

    for (int i=0; i<M; ++i)
        printf("%.0f ", cvec_f_topPriorityQ(&vec)), cvec_f_popPriorityQ(&vec);
    start = clock();
    for (int i=M; i<N; ++i)
        cvec_f_popPriorityQ(&vec);
    printf("\n\npopped PQ: %f secs\n", (clock() - start) / (float) CLOCKS_PER_SEC);

    pcg = pcg32_seed(seed, 0);
    start = clock();
    for (int i=0; i<N; ++i)
        cvec_f_pushPriorityQ(&vec, pcg32_random(&pcg));
    printf("pushed PQ: %f secs\n", (clock() - start) / (float) CLOCKS_PER_SEC);
    for (int i=0; i<M; ++i)
        printf("%.0f ", cvec_f_topPriorityQ(&vec)), cvec_f_popPriorityQ(&vec);
    puts("");
}
