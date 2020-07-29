
#include <stdio.h>
#include <time.h>
#include <stc/cvecpq.h>
#include <stc/cmap.h>
#include <stc/crand.h>

declare_cvec(i, uint32_t);
declare_cvec_priority_queue(i, >); // min-heap (increasing values)

int main() {
    crand_eng32_t pcg = crand_eng32(time(NULL));
    cvec_i heap = cvec_init;

    // Push ten million random numbers to queue
    for (int i=0; i<10000000; ++i)
        cvecpq_i_push(&heap, crand_gen_i32(&pcg));

    // Extract the hundred smallest.
    for (int i=0; i<100; ++i) {
        printf("%u ", cvecpq_i_top(&heap));
        cvecpq_i_pop(&heap);
    }
    cvec_i_destroy(&heap);
}
