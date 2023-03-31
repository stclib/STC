#include <stc/crand.h>
#include <stc/forward.h>
#include <stdio.h>
#include <time.h>

forward_cqueue(cqueue_pnt, struct Point);

struct Point { int x, y; } typedef Point;
int point_cmp(const Point* a, const Point* b) {
    int c = c_default_cmp(&a->x, &b->x);
    return c ? c : c_default_cmp(&a->y, &b->y);
}
#define i_val Point
#define i_cmp point_cmp
#define i_is_forward
#define i_tag pnt
#include <stc/cqueue.h>

#define i_type IQ
#define i_val int
#include <stc/cqueue.h>

int main() {
    int n = 50000000;
    crand_t rng = crand_init((uint64_t)time(NULL));
    crand_unif_t dist = crand_unif_init(0, n);

    IQ Q = {0};

    // Push 50'000'000 random numbers onto the queue.
    c_forrange (n)
        IQ_push(&Q, (int)crand_unif(&rng, &dist));

    // Push or pop on the queue 50 million times
    printf("befor: size %" c_ZI ", capacity %" c_ZI "\n", IQ_size(&Q), IQ_capacity(&Q));
    
    c_forrange (n) {
        int r = (int)crand_unif(&rng, &dist);
        if (r & 3)
            IQ_push(&Q, r);
        else
            IQ_pop(&Q);
    }

    printf("after: size %" c_ZI ", capacity %" c_ZI "\n", IQ_size(&Q), IQ_capacity(&Q));
    IQ_drop(&Q);
}
