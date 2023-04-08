#include <stc/crand.h>
#include <stdio.h>

#define i_val int
#define i_tag i
#include <stc/cqueue.h>

int main() {
    int n = 100000000;
    crand_unif_t dist;
    crand_t rng = crand_init(1234);
    dist = crand_unif_init(0, n);

    cqueue_i queue = {0};

    // Push ten million random numbers onto the queue.
    c_forrange (n)
        cqueue_i_push(&queue, (int)crand_unif(&rng, &dist));

    // Push or pop on the queue ten million times
    printf("%d\n", n);
    c_forrange (n) { // forrange uses initial n only.
        int r = (int)crand_unif(&rng, &dist);
        if (r & 1)
            ++n, cqueue_i_push(&queue, r);
        else
            --n, cqueue_i_pop(&queue);
    }
    printf("%d, %" c_ZI "\n", n, cqueue_i_size(&queue));

    cqueue_i_drop(&queue);
}
