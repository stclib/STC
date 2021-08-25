
#include <stdio.h>
#include <time.h>
#include <stc/cvec.h>
#include <stc/cpque.h>
#include <stc/cmap.h>
#include <stc/crandom.h>

using_cvec(i, int64_t);
using_cpque(i, cvec_i, -c_default_compare); // min-heap (increasing values)

int main() {
    size_t N = 10000000;
    stc64_t rng = stc64_init(time(NULL));
    stc64_uniform_t dist = stc64_uniform_init(0, N * 10);
    c_forauto (cpque_i, heap)
    {
        // Push ten million random numbers to priority queue
        c_forrange (N)
            cpque_i_push(&heap, stc64_uniform(&rng, &dist));

        // push some negative numbers too.
        c_emplace(cpque_i, heap, {-231, -32, -873, -4, -343});

        c_forrange (N)
            cpque_i_push(&heap, stc64_uniform(&rng, &dist));

        // Extract the hundred smallest.
        c_forrange (100) {
            printf("%zd ", *cpque_i_top(&heap));
            cpque_i_pop(&heap);
        }
    }
}
