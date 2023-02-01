
#include <stdio.h>
#include <time.h>
#include <stc/crandom.h>

#define i_val int64_t
#define i_cmp -c_default_cmp  // min-heap (increasing values)
#define i_tag i
#include <stc/cpque.h>

int main() {
    intptr_t N = 10000000;
    stc64_t rng = stc64_new((uint64_t)time(NULL));
    stc64_uniform_t dist = stc64_uniform_new(0, N * 10);
    c_AUTO (cpque_i, heap)
    {
        // Push ten million random numbers to priority queue
        printf("Push %" c_ZI " numbers\n", N);
        c_FORRANGE (N)
            cpque_i_push(&heap, stc64_uniform(&rng, &dist));

        // push some negative numbers too.
        c_FORLIST (i, int, {-231, -32, -873, -4, -343})
            cpque_i_push(&heap, *i.ref);

        c_FORRANGE (N)
            cpque_i_push(&heap, stc64_uniform(&rng, &dist));

        puts("Extract the hundred smallest.");
        c_FORRANGE (100) {
            printf("%" PRId64 " ", *cpque_i_top(&heap));
            cpque_i_pop(&heap);
        }
    }
}
