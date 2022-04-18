
#include <stdio.h>
#include <time.h>
#include <stc/crandom.h>

#define i_val int64_t
#define i_cmp -c_default_cmp  // min-heap (increasing values)
#define i_tag i
#include <stc/cpque.h>

int main() {
    size_t N = 10000000;
    stc64_t rng = stc64_new(time(NULL));
    stc64_uniform_t dist = stc64_uniform_new(0, N * 10);
    c_auto (cpque_i, heap)
    {
        // Push ten million random numbers to priority queue
        printf("Push %" PRIuMAX " numbers\n", N);
        c_forrange (N)
            cpque_i_push(&heap, stc64_uniform(&rng, &dist));

        // push some negative numbers too.
        c_apply(v, cpque_i_push(&heap, v), int, {-231, -32, -873, -4, -343});

        c_forrange (N)
            cpque_i_push(&heap, stc64_uniform(&rng, &dist));

        puts("Extract the hundred smallest.");
        c_forrange (100) {
            printf("%" PRIdMAX " ", *cpque_i_top(&heap));
            cpque_i_pop(&heap);
        }
    }
}
