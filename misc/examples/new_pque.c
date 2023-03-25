#include <stdio.h>

struct Point { int x, y; } typedef Point;

#define i_type PointQ
#define i_val Point
#define i_less(a, b) a->x < b->x || (a->x == b->x && a->y < b->y)
#include <stc/cpque.h>


int main()
{
    PointQ pque = c_make(PointQ, {{23, 80}, {12, 32}, {54, 74}, {12, 62}});
    // print
    for (; !PointQ_empty(&pque); PointQ_pop(&pque))
    {
        const Point *v = PointQ_top(&pque);
        printf(" (%d,%d)", v->x, v->y);
    }
    puts("");
    PointQ_drop(&pque);
}
