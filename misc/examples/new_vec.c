#include <stdio.h>
#include <stc/forward.h>

forward_cvec(cvec_i32, int);
forward_cvec(cvec_pnt, struct Point);

struct MyStruct {
    cvec_i32 intvec;
    cvec_pnt pntvec;
} typedef MyStruct;

#define i_val int
#define i_is_forward
#define i_tag i32
#include <stc/cvec.h>

typedef struct Point { int x, y; } Point;

#define i_val Point
#define i_less(a, b) a->x < b->x || (a->x == b->x && a->y < b->y)
#define i_is_forward
#define i_tag pnt
#include <stc/cvec.h>

int main()
{
    MyStruct my = {0};

    cvec_pnt_push(&my.pntvec, (Point){42, 14});
    cvec_pnt_push(&my.pntvec, (Point){32, 94});
    cvec_pnt_push(&my.pntvec, (Point){62, 81});
    cvec_pnt_push(&my.pntvec, (Point){32, 91});

    cvec_pnt_sort(&my.pntvec);

    c_foreach (i, cvec_pnt, my.pntvec)
        printf(" (%d %d)", i.ref->x, i.ref->y);
    puts("");

    cvec_i32_drop(&my.intvec);
    cvec_pnt_drop(&my.pntvec);
}
