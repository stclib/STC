
#include <stdio.h>
#include <time.h>
#include <stc/cpqueue.h>
#include <stc/cmap.h>
#include <stc/crandom.h>

declare_cvec(i, uint32_t);
declare_cvec_pqueue(i, >); // min-heap (increasing values)

int main() {
    crandom_eng32_t pcg = crandom_eng32_init(time(NULL));
    crandom_distrib_i32_t dist = crandom_uniform_i32_init(0, 100000000);
    cvec_i heap = cvec_init;

    // Push ten million random numbers to priority queue
    for (int i=0; i<10000000; ++i)
        cvec_i_pqueue_push(&heap, crandom_uniform_i32(&pcg, dist));

    // Extract the hundred smallest.
    for (int i=0; i<100; ++i) {
        printf("%u ", cvec_i_pqueue_top(&heap));
        cvec_i_pqueue_pop(&heap);
    }
    cvec_i_destroy(&heap);
}
