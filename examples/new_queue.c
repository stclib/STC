#include <stc/crandom.h>
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
#define i_opt c_is_fwd
#define i_tag pnt
#include <stc/cqueue.h>

#define i_val int
#include <stc/cqueue.h>

int main() {
    int n = 60000000;
    stc64_t rng = stc64_new(time(NULL));
    stc64_uniform_t dist = stc64_uniform_new(0, n);

    c_auto (cqueue_int, Q)
    {
        // Push eight million random numbers onto the queue.
        for (int i=0; i<n; ++i)
            cqueue_int_push(&Q, stc64_uniform(&rng, &dist));

        // Push or pop on the queue ten million times
        printf("befor: size %" PRIuMAX ", capacity %" PRIuMAX "\n", cqueue_int_size(Q), cqueue_int_capacity(Q));
        for (int i=n; i>0; --i) {
            int r = stc64_uniform(&rng, &dist);
            if (r & 1)
                cqueue_int_push(&Q, r);
            else
                cqueue_int_pop(&Q);
        }
        printf("after: size %" PRIuMAX ", capacity %" PRIuMAX "\n", cqueue_int_size(Q), cqueue_int_capacity(Q));
    }
}
