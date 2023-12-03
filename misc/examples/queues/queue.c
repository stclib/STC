#include "stc/crand.h"
#include <stdio.h>

#define i_TYPE queue_i, int
#include "stc/queue.h"

int main(void) {
    int n = 1000000;
    crand_uniform_t dist;
    crand_t rng = crand_init(1234);
    dist = crand_uniform_init(0, n);

    queue_i queue = {0};

    // Push ten million random numbers onto the queue.
    c_forrange (n)
        queue_i_push(&queue, (int)crand_uniform(&rng, &dist));

    // Push or pop on the queue ten million times
    printf("%d\n", n);
    c_forrange (n) { // forrange uses initial n only.
        int r = (int)crand_uniform(&rng, &dist);
        if (r & 1)
            ++n, queue_i_push(&queue, r);
        else
            --n, queue_i_pop(&queue);
    }
    printf("%d, %" c_ZI "\n", n, queue_i_size(&queue));

    queue_i_drop(&queue);
}
