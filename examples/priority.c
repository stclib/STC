
#include <stdio.h>
#include <time.h>
#include <stc/cvecpq.h>
#include <stc/cmap.h>
#include <stc/crandom.h>

declare_CVec(i, uint32_t);
declare_CVec_priority_queue(i, >); // min-heap (increasing values)

int main() {
    CRand32 pcg = crand32_init(time(NULL));
    CVec_i heap = cvec_init;

    // Push ten million random numbers to queue
    for (int i=0; i<10000000; ++i)
        cvecpq_i_push(&heap, crand32_gen(&pcg));

    // Extract the hundred smallest.
    for (int i=0; i<100; ++i) {
        printf("%u ", cvecpq_i_top(&heap));
        cvecpq_i_pop(&heap);
    }
    cvec_i_destroy(&heap);
}
