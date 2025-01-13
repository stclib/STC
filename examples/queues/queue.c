#include <stdio.h>
#include "stc/random.h"

#define i_type queue_i, int
#include "stc/queue.h"

int main(void) {
    int n = 1000000;
    crand64_seed(1234);

    queue_i queue = {0};

    // Push ten million random numbers onto the queue.
    for (c_range(n))
        queue_i_push(&queue, crand64_uint() & ((1 << 20) - 1));

    // Push or pop on the queue ten million times
    printf("%d\n", n);
    for (c_range(n)) { // forrange uses initial n only.
        int r = (int)crand64_uint() & ((1 << 20) - 1);
        if (r & 1)
            ++n, queue_i_push(&queue, r);
        else
            --n, queue_i_pop(&queue);
    }
    printf("%d, %" c_ZI "\n", n, queue_i_size(&queue));

    queue_i_drop(&queue);
}
