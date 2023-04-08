#include <stdio.h>
#include <stc/forward.h>

forward_clist(clist_i32, int);
forward_clist(clist_pnt, struct Point);

typedef struct {
    clist_i32 intlst;
    clist_pnt pntlst;
} MyStruct;

#define i_val int
#define i_tag i32
#define i_is_forward
#include <stc/clist.h>

typedef struct Point { int x, y; } Point;
int point_cmp(const Point* a, const Point* b) {
    int c = a->x - b->x;
    return c ? c : a->y - b->y;
}

#define i_val Point
#define i_cmp point_cmp
#define i_is_forward
#define i_tag pnt
#include <stc/clist.h>

#define i_val float
#include <stc/clist.h>

void MyStruct_drop(MyStruct* s);
#define i_type MyList
#define i_valclass MyStruct        // i_valclass uses MyStruct_drop
#define i_opt c_no_clone|c_no_cmp
#include <stc/clist.h>

void MyStruct_drop(MyStruct* s) {
    clist_i32_drop(&s->intlst);
    clist_pnt_drop(&s->pntlst);
}


int main()
{
    MyStruct my = {0};
    clist_i32_push_back(&my.intlst, 123);
    clist_pnt_push_back(&my.pntlst, (Point){123, 456});
    MyStruct_drop(&my);

    clist_pnt plst = c_make(clist_pnt, {{42, 14}, {32, 94}, {62, 81}});
    clist_pnt_sort(&plst);

    c_foreach (i, clist_pnt, plst) 
        printf(" (%d %d)", i.ref->x, i.ref->y);
    puts("");
    clist_pnt_drop(&plst);


    clist_float flst = c_make(clist_float, {123.3f, 321.2f, -32.2f, 78.2f});
    clist_float_sort(&flst);

    c_foreach (i, clist_float, flst)
        printf(" %g", *i.ref);

    puts("");
    clist_float_drop(&flst);
}
