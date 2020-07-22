
#include <stdio.h>
#include <time.h>
#include <stc/cvecpq.h>
#include <stc/cmap.h>
#include <stc/crandom.h>

declare_CVec(i, uint32_t);
declare_CVec_priorityQ(i, >); // min-heap (increasing values)
declare_CMap(ii, int, int);

int main() {
    pcg32_random_t pcg = pcg32_seed(time(NULL), 0);
    CVec_i heap = cvec_init;

    // Push ten million random numbers to queue
    for (int i=0; i<10000000; ++i)
        cvec_i_pushPriorityQ(&heap, pcg32_random(&pcg));

    // Extract the hundred smallest.
    for (int i=0; i<100; ++i) {
        printf("%u ", cvec_i_topPriorityQ(&heap));
        cvec_i_popPriorityQ(&heap);
    }
    cvec_i_destroy(&heap);
}