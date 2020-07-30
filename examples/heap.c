#include <stdio.h>
#include <time.h>
#include <stc/crand.h>
#include <stc/cvec_pq.h>

declare_cvec(f, float);
declare_cvec_pqueue(f, >);

int main()
{
    uint32_t seed = time(NULL);
    crand_eng32_t pcg = crand_eng32_init(seed);
    int N = 30000000, M = 100;
    cvec_f vec = cvec_init;
    clock_t start = clock();
    for (int i=0; i<N; ++i)
        cvec_f_push_back(&vec, crand_gen_i32(&pcg));
    cvec_f_pqueue_build(&vec);
    printf("Built priority queue: %f secs\n", (clock() - start) / (float) CLOCKS_PER_SEC);

    for (int i=0; i<M; ++i)
        printf("%.0f ", cvec_f_pqueue_top(&vec)), cvec_f_pqueue_pop(&vec);
    start = clock();
    for (int i=M; i<N; ++i)
        cvec_f_pqueue_pop(&vec);
    printf("\n\npopped PQ: %f secs\n", (clock() - start) / (float) CLOCKS_PER_SEC);

    pcg = crand_eng32_init(seed);
    start = clock();
    for (int i=0; i<N; ++i)
        cvec_f_pqueue_push(&vec, crand_gen_i32(&pcg));
    printf("pushed PQ: %f secs\n", (clock() - start) / (float) CLOCKS_PER_SEC);
    for (int i=0; i<M; ++i)
        printf("%.0f ", cvec_f_pqueue_top(&vec)), cvec_f_pqueue_pop(&vec);
    puts("");
}
