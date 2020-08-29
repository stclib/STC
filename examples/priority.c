
#include <stdio.h>
#include <time.h>
#include <stc/cpqueue.h>
#include <stc/cmap.h>
#include <stc/crandom.h>

declare_cvec(i, int64_t);
declare_cvec_pqueue(i, >); // min-heap (increasing values)

int main() {
    crand_rng32_t pcg = crand_rng32_init(time(NULL));
    crand_uniform_i32_t dist = crand_uniform_i32_init(pcg, 0, 100000000);
    cvec_i heap = cvec_init;

    // Push ten million random numbers to priority queue
    for (int i=0; i<10000000; ++i)
        cvec_i_pqueue_push(&heap, crand_uniform_i32(&dist));

    // push some negative numbers too.
    c_push(&heap, cvec_i_pqueue, c_items(-231, -32, -873, -4, -343));

    for (int i=0; i<10000000; ++i)
        cvec_i_pqueue_push(&heap, crand_uniform_i32(&dist));


    // Extract the hundred smallest.
    for (int i=0; i<100; ++i) {
        printf("%d ", cvec_i_pqueue_top(&heap));
        cvec_i_pqueue_pop(&heap);
    }
    cvec_i_destroy(&heap);
}
