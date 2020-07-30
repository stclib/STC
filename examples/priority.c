
#include <stdio.h>
#include <time.h>
#include <stc/cvec_pq.h>
#include <stc/cmap.h>
#include <stc/crand.h>

declare_cvec(i, uint32_t);
declare_cvec_pqueue(i, >); // min-heap (increasing values)

int main() {
    crand_eng32_t pcg = crand_eng32_init(time(NULL));
    crand_uniform_i32_t dist = crand_uniform_i32_init(0, 100000000);
    cvec_i heap = cvec_init;

    // Push ten million random numbers to priority queue
    for (int i=0; i<10000000; ++i)
        cvec_i_pqueue_push(&heap, crand_uniform_i32(&pcg, dist));

    // Extract the hundred smallest.
    for (int i=0; i<100; ++i) {
        printf("%u ", cvec_i_pqueue_top(&heap));
        cvec_i_pqueue_pop(&heap);
    }
    cvec_i_destroy(&heap);
}
