
#include <stdio.h>
#include <time.h>
#include "stc/crand.h"

#define i_TYPE pque_i64, int64_t
#define i_cmp -c_default_cmp  // min-heap (increasing values)
#include "stc/pque.h"

int main(void) {
    intptr_t N = 10000000;
    crand_t rng = crand_init((uint64_t)time(NULL));
    crand_uniform_t dist = crand_uniform_init(0, N * 10);

    pque_i64 heap = {0};

    // Push ten million random numbers to priority queue
    printf("Push %" c_ZI " numbers\n", N);
    c_forrange (N)
        pque_i64_push(&heap, crand_uniform(&rng, &dist));

    // push some negative numbers too.
    c_forlist (i, int, {-231, -32, -873, -4, -343})
        pque_i64_push(&heap, *i.ref);

    c_forrange (N)
        pque_i64_push(&heap, crand_uniform(&rng, &dist));

    puts("Extract the hundred smallest.");
    c_forrange (100) {
        printf("%" PRIi64 " ", *pque_i64_top(&heap));
        pque_i64_pop(&heap);
    }

    pque_i64_drop(&heap);
}
