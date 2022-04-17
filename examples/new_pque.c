#include <stc/forward.h>

forward_cpque(cpque_pnt, struct Point);

struct MyStruct {
    cpque_pnt priority_queue;
    int id;
};

#define i_val int
#include <stc/cstack.h>

#define i_val int
#include <stc/cpque.h>

struct Point { int x, y; } typedef Point;

int Point_cmp(const Point* a, const Point* b) {
    int c = a->x - b->x;
    return c ? c : a->y - b->y;
}

#define i_val Point
#define i_cmp Point_cmp
#define i_opt c_is_fwd
#define i_tag pnt
#include <stc/cpque.h>

#include <stdio.h>

int main()
{
    c_auto (cstack_int, istk)
    {
        cstack_int_push(&istk, 123);
        cstack_int_push(&istk, 321);
        // print
        c_foreach (i, cstack_int, istk)
            printf(" %d", *i.ref);
        puts("");
    }
    c_auto (cpque_pnt, pque)
    {
        cpque_pnt_push(&pque, (Point){23, 80});
        cpque_pnt_push(&pque, (Point){12, 32});
        cpque_pnt_push(&pque, (Point){54, 74});
        cpque_pnt_push(&pque, (Point){12, 62});
        // print
        while (!cpque_pnt_empty(pque)) {
            cpque_pnt_value *v = cpque_pnt_top(&pque);
            printf(" (%d,%d)", v->x, v->y);
            cpque_pnt_pop(&pque);
        }
        puts("");
    }
    c_auto (cpque_int, ique)
    {
        cpque_int_push(&ique, 123);
        cpque_int_push(&ique, 321);
        // print
        for (size_t i=0; i<cpque_int_size(ique); ++i)
            printf(" %d", ique.data[i]);
        puts("");
    }
}
