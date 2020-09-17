#include <stc/crandom.h>
#include <stc/cqueue.h>
#include <stdio.h>

using_clist(i, int);
using_cqueue(i, clist_i); // min-heap (increasing values)

int main() {
    int n = 10000000;
    crand_uniform_i32_t dist;
    crand_rng32_t rng = crand_rng32_init(1234);
    dist = crand_uniform_i32_init(0, n);

    cqueue_i queue = cqueue_i_init();

    // Push ten million random numbers onto the queue.
    for (int i=0; i<n; ++i)
        cqueue_i_push(&queue, crand_uniform_i32(&rng, &dist));
    
    // Push or pop on the queue ten million times
    printf("%d\n", n);
    for (int i=n; i>0; --i) {
        int r = crand_uniform_i32(&rng, &dist);
        if (r & 1)
            ++n, cqueue_i_push(&queue, r);
        else
            --n, cqueue_i_pop(&queue);
    }
    printf("%d, %zu\n", n, cqueue_i_size(queue));
    cqueue_i_destroy(&queue);
}
