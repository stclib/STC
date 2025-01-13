#include <stdio.h>
#include <time.h>
#include "stc/random.h"
#include "stc/types.h"
declare_queue(queue_pnt, struct Point);

typedef struct Point { int x, y; } Point;
#define i_type queue_pnt,Point
#define i_declared
#include "stc/queue.h"

#define i_type IntQ,int
#include "stc/queue.h"

int main(void) {
    int n = 50000000;
    crand64 rng = crand64_from((uint64_t)time(NULL));

    IntQ Q = {0};

    // Push 50'000'000 random numbers onto the queue.
    for (c_range(n))
        IntQ_push(&Q, crand64_uint_r(&rng, 1) & ((1<<24) - 1));

    // Push or pop on the queue 50 million times
    printf("befor: size %" c_ZI ", capacity %" c_ZI "\n", IntQ_size(&Q), IntQ_capacity(&Q));

    for (c_range(n)) {
        int r = crand64_uint_r(&rng, 1) & ((1<<24) - 1);
        if (r & 3)
            IntQ_push(&Q, r);
        else
            IntQ_pop(&Q);
    }

    printf("after: size %" c_ZI ", capacity %" c_ZI "\n", IntQ_size(&Q), IntQ_capacity(&Q));
    IntQ_drop(&Q);
}
