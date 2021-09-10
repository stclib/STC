#include <stc/crandom.h>
#include <stdio.h>

#define i_val int
#include <stc/cqueue.h>
#include <time.h>

int main() {
    int n = 8000000;
    stc64_t rng = stc64_init(time(NULL));
    stc64_uniform_t dist = stc64_uniform_init(0, n);

    c_forauto (cqueue_int, Q)
    {
        // Push eight million random numbers onto the queue.
        for (int i=0; i<n; ++i)
            cqueue_int_push(&Q, stc64_uniform(&rng, &dist));

        // Push or pop on the queue ten million times
        printf("befor: size %zu, capacity %zu\n", cqueue_int_size(Q), cqueue_int_capacity(Q));
        for (int i=n; i>0; --i) {
            int r = stc64_uniform(&rng, &dist);
            if (r & 1)
                cqueue_int_push(&Q, r);
            else
                cqueue_int_pop(&Q);
        }
        printf("after: size %zu, capacity %zu\n", cqueue_int_size(Q), cqueue_int_capacity(Q));
    }
}