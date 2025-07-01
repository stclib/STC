#include <stdio.h>
#include <stc/random.h>

#define T IntQ, int
#include <stc/queue.h>

int main(void) {
    int n = 1000000;
    crand64_seed(1234);

    IntQ queue = {0};

    // Push ten million random numbers onto the queue.
    for (c_range(n))
        IntQ_push(&queue, crand64_uint() & ((1 << 20) - 1));

    // Push or pop on the queue ten million times
    printf("%d\n", n);
    for (c_range(n)) { // forrange uses initial n only.
        int r = (int)crand64_uint() & ((1 << 20) - 1);
        if (r & 1)
            ++n, IntQ_push(&queue, r);
        else
            --n, IntQ_pop(&queue);
    }
    printf("%d, %" c_ZI "\n", n, IntQ_size(&queue));

    IntQ_drop(&queue);
}
