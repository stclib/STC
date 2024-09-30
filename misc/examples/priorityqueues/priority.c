
#include <stdio.h>
#include <time.h>
#include "stc/random.h"

#define i_type PQueue,int
#define i_cmp -c_default_cmp  // min-heap (increasing values)
#include "stc/pqueue.h"

int main(void) {
    int N = 10000000;
    crand64 rng = crand64_from((uint64_t)time(NULL));
    PQueue heap = {0};

    // Push ten million random numbers to priority queue
    printf("Push %d numbers\n", N);
    c_forrange (N)
        PQueue_push(&heap, crand64_uint_r(&rng, 1) & ((1<<20) - 1));

    // push some negative numbers too.
    c_foritems (i, int, {-231, -32, -873, -4, -343})
        PQueue_push(&heap, *i.ref);

    c_forrange (N)
        PQueue_push(&heap, crand64_uint_r(&rng, 1) & ((1<<20) - 1));

    puts("Extract the hundred smallest.");
    c_forrange (100) {
        printf("%d ", PQueue_pull(&heap));
    }

    PQueue_drop(&heap);
}
