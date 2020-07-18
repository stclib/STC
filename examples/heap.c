#include <stdio.h>
#include <time.h>
#include "stc/cvecque.h"
#include "stc/crandom.h"

declare_CVec(f, float);
declare_CVecque(f);

int main()
{
    //float arr[] = {1, 3, 6, 5, 9, 8, -2};
    float arr[] = {9., 3., 5., 10., 2., 4., 16., 7., 12., 8.};
    uint32_t seed = time(NULL);
    pcg32_random_t pcg = pcg32_seed(seed, 0);
    int N = 30000000, M = 100;
    CVec_f vec = cvec_init;
    clock_t start = clock();
    for (int i=0; i<N; ++i) cvec_f_pushBack(&vec, pcg32_random(&pcg)); // arr[i]);
    cvec_f_queueify(&vec);
    printf("\n\nqueueified: %f\n", (clock() - start) / (float) CLOCKS_PER_SEC);

    for (int i=0; i<M; ++i) printf("%.0f ", vec.data[i]);
    puts("\n");
    for (int i=0; i<M; ++i) printf("%.0f ", cvecque_f_pop(&vec));
    start = clock();
    for (int i=M; i<N; ++i) cvecque_f_pop(&vec);
    printf("\n\npopped: %f\n", (clock() - start) / (float) CLOCKS_PER_SEC);
    printf("size: %zu\n", cvec_size(vec));
    pcg = pcg32_seed(seed, 0);
    start = clock();
    for (int i=0; i<N; ++i) cvecque_f_push(&vec, pcg32_random(&pcg)); // arr[i]);
    printf("pushed: %f\n", (clock() - start) / (float) CLOCKS_PER_SEC);
    for (int i=0; i<M; ++i) printf("%.0f ", vec.data[i]);
    puts("\n");
    for (int i=0; i<M; ++i) printf("%.0f ", cvecque_f_pop(&vec));
}
