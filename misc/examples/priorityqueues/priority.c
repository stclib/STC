
#include <stdio.h>
#include <time.h>
#include "stc/crand.h"

#define i_TYPE PQueue,int
#define i_cmp -c_default_cmp  // min-heap (increasing values)
#include "stc/pque.h"

int main(void) {
    int N = 10000000;
    crand_t rng = crand_init((uint64_t)time(NULL));
    crand_uniform_t dist = crand_uniform_init(0, N * 10);

    PQueue heap = {0};

    // Push ten million random numbers to priority queue
    printf("Push %d numbers\n", N);
    c_forrange (N)
        PQueue_push(&heap, (int)crand_uniform(&rng, &dist));

    // push some negative numbers too.
    c_foritems (i, int, {-231, -32, -873, -4, -343})
        PQueue_push(&heap, *i.ref);

    c_forrange (N)
        PQueue_push(&heap, (int)crand_uniform(&rng, &dist));

    puts("Extract the hundred smallest.");
    c_forrange (100) {
        printf("%d ", PQueue_pull(&heap));
    }

    PQueue_drop(&heap);
}
