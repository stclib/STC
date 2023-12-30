#include "stc/crand.h"
#include "stc/types.h"
#include <stdio.h>
#include <time.h>

forward_queue(queue_pnt, struct Point);

typedef struct Point { int x, y; } Point;
#define i_TYPE queue_pnt,Point
#define i_is_forward
#include "stc/queue.h"

#define i_TYPE IntQ,int
#include "stc/queue.h"

int main(void) {
    int n = 50000000;
    crand_t rng = crand_init((uint64_t)time(NULL));
    crand_uniform_t dist = crand_uniform_init(0, n);

    IntQ Q = {0};

    // Push 50'000'000 random numbers onto the queue.
    c_forrange (n)
        IntQ_push(&Q, (int)crand_uniform(&rng, &dist));

    // Push or pop on the queue 50 million times
    printf("befor: size %" c_ZI ", capacity %" c_ZI "\n", IntQ_size(&Q), IntQ_capacity(&Q));

    c_forrange (n) {
        int r = (int)crand_uniform(&rng, &dist);
        if (r & 3)
            IntQ_push(&Q, r);
        else
            IntQ_pop(&Q);
    }

    printf("after: size %" c_ZI ", capacity %" c_ZI "\n", IntQ_size(&Q), IntQ_capacity(&Q));
    IntQ_drop(&Q);
}
