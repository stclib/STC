#include <stc/crandom.h>
#include <stc/cqueue.h>
#include <stdio.h>

using_clist(i, int);
using_cqueue(i, clist_i); // min-heap (increasing values)

int main() {
    int n = 10000000;
    cstc64_uniform_t dist;
    cstc64_t rng = cstc64_init(1234);
    dist = cstc64_uniform_init(0, n);

    cqueue_i queue = cqueue_i_init();

    // Push ten million random numbers onto the queue.
    c_forrange (n)
        cqueue_i_push(&queue, cstc64_uniform(&rng, &dist));

    // Push or pop on the queue ten million times
    printf("%d\n", n);
    c_forrange (n) { // range uses initial n only.
        int r = cstc64_uniform(&rng, &dist);
        if (r & 1)
            ++n, cqueue_i_push(&queue, r);
        else
            --n, cqueue_i_pop(&queue);
    }
    printf("%d, %zu\n", n, cqueue_i_size(queue));
    cqueue_i_del(&queue);
}
