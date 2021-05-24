#include <stc/crandom.h>
#include <stc/cqueue.h>
#include <stdio.h>

#if 1
using_cdeq(i, int);
using_cqueue(i, cdeq_i);
#else
#include <stc/clist.h>
using_clist(i, int);
using_cqueue(i, clist_i);
#endif

int main() {
    int n = 100000000;
    stc64_uniform_t dist;
    stc64_t rng = stc64_init(1234);
    dist = stc64_uniform_init(0, n);

    c_forvar (cqueue_i queue = cqueue_i_init(), cqueue_i_del(&queue))
    {
        // Push ten million random numbers onto the queue.
        c_forrange (n)
            cqueue_i_push(&queue, stc64_uniform(&rng, &dist));

        // Push or pop on the queue ten million times
        printf("%d\n", n);
        c_forrange (n) { // range uses initial n only.
            int r = stc64_uniform(&rng, &dist);
            if (r & 1)
                ++n, cqueue_i_push(&queue, r);
            else
                --n, cqueue_i_pop(&queue);
        }
        printf("%d, %zu\n", n, cqueue_i_size(queue));
    }
}
