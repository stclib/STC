#include <stc/crand.h>
#include <stdio.h>

#define i_key int
#define i_tag i
#include <stc/cqueue.h>

int main(void) {
    int n = 1000000;
    crand_uniform_t dist;
    crand_t rng = crand_init(1234);
    dist = crand_uniform_init(0, n);

    cqueue_i queue = {0};

    // Push ten million random numbers onto the queue.
    c_forrange (n)
        cqueue_i_push(&queue, (int)crand_uniform(&rng, &dist));

    // Push or pop on the queue ten million times
    printf("%d\n", n);
    c_forrange (n) { // forrange uses initial n only.
        int r = (int)crand_uniform(&rng, &dist);
        if (r & 1)
            ++n, cqueue_i_push(&queue, r);
        else
            --n, cqueue_i_pop(&queue);
    }
    printf("%d, %" c_ZI "\n", n, cqueue_i_size(&queue));

    cqueue_i_drop(&queue);
}
