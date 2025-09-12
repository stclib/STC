#include <stdio.h>
#include <stc/types.h>

declare_vec(vec_i32, int);
declare_vec(vec_pnt, struct Point);

typedef struct MyStruct {
    vec_i32 intvec;
    vec_pnt pntvec;
} MyStruct;

#define T vec_i32, int, (c_declared)
#include <stc/vec.h>

typedef struct Point { int x, y; } Point;

#define T vec_pnt, Point, (c_declared)
#define i_less(a, b) a->x < b->x || (a->x == b->x && a->y < b->y)
#define i_eq(a, b) a->x == b->x && a->y == b->y
#include <stc/vec.h>

int main(void)
{
    MyStruct my = {0};

    vec_pnt_push(&my.pntvec, c_literal(Point){42, 14});
    vec_pnt_push(&my.pntvec, c_literal(Point){32, 94});
    vec_pnt_push(&my.pntvec, c_literal(Point){62, 81});
    vec_pnt_push(&my.pntvec, c_literal(Point){32, 91});

    vec_pnt_sort(&my.pntvec);

    for (c_each_item(e, vec_pnt, my.pntvec))
        printf(" (%d %d)", e->x, e->y);
    puts("");

    vec_i32_drop(&my.intvec);
    vec_pnt_drop(&my.pntvec);
}
