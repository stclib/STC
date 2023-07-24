
#include <stdio.h>
#include <time.h>
#include <stc/crand.h>

#define i_key int64_t
#define i_cmp -c_default_cmp  // min-heap (increasing values)
#define i_tag i
#include <stc/cpque.h>

int main(void) {
    intptr_t N = 10000000;
    crand_t rng = crand_init((uint64_t)time(NULL));
    crand_uniform_t dist = crand_uniform_init(0, N * 10);

    cpque_i heap = {0};

    // Push ten million random numbers to priority queue
    printf("Push %" c_ZI " numbers\n", N);
    c_forrange (N)
        cpque_i_push(&heap, crand_uniform(&rng, &dist));

    // push some negative numbers too.
    c_forlist (i, int, {-231, -32, -873, -4, -343})
        cpque_i_push(&heap, *i.ref);

    c_forrange (N)
        cpque_i_push(&heap, crand_uniform(&rng, &dist));

    puts("Extract the hundred smallest.");
    c_forrange (100) {
        printf("%" PRId64 " ", *cpque_i_top(&heap));
        cpque_i_pop(&heap);
    }

    cpque_i_drop(&heap);
}
