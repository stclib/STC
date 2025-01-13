#include <stdio.h>
#include "stc/types.h"

declare_vec(vec_i32, int);
declare_vec(vec_pnt, struct Point);

typedef struct MyStruct {
    vec_i32 intvec;
    vec_pnt pntvec;
} MyStruct;

#define i_type vec_i32, int
#define i_declared
#include "stc/vec.h"

typedef struct Point { int x, y; } Point;

#define i_type vec_pnt, Point
#define i_less(a, b) a->x < b->x || (a->x == b->x && a->y < b->y)
#define i_eq(a, b) a->x == b->x && a->y == b->y
#define i_declared
#include "stc/vec.h"

int main(void)
{
    MyStruct my = {0};

    vec_pnt_push(&my.pntvec, c_literal(Point){42, 14});
    vec_pnt_push(&my.pntvec, c_literal(Point){32, 94});
    vec_pnt_push(&my.pntvec, c_literal(Point){62, 81});
    vec_pnt_push(&my.pntvec, c_literal(Point){32, 91});

    vec_pnt_sort(&my.pntvec);

    for (c_each(i, vec_pnt, my.pntvec))
        printf(" (%d %d)", i.ref->x, i.ref->y);
    puts("");

    vec_i32_drop(&my.intvec);
    vec_pnt_drop(&my.pntvec);
}
