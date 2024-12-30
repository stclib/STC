#include <stdio.h>

typedef struct Point { int x, y; } Point;

#define i_type PointQ,Point
#define i_less(a, b) a->x < b->x || (a->x == b->x && a->y < b->y)
#include "stc/pqueue.h"


int main(void)
{
    PointQ pqueue = c_make(PointQ, {{23, 80}, {12, 32}, {54, 74}, {12, 62}});
    // print
    for (; !PointQ_is_empty(&pqueue); PointQ_pop(&pqueue))
    {
        const Point *v = PointQ_top(&pqueue);
        printf(" (%d,%d)", v->x, v->y);
    }
    puts("");
    PointQ_drop(&pqueue);
}
