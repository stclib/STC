#include <stc/cstr.h>
#include <stc/forward.h>

forward_clist(clist_i32, int);
forward_clist(clist_pnt, struct Point);

struct MyStruct {
    clist_i32 intlst;
    clist_pnt pntlst;
} typedef MyStruct;

#define i_val int
#define i_fwd
#define i_tag i32
#include <stc/clist.h>

struct Point { int x, y; } typedef Point;
int point_compare(const Point* a, const Point* b) {
    int c = c_default_compare(&a->x, &b->x);
    return c ? c : c_default_compare(&a->y, &b->y);
}

#define i_val Point
#define i_cmp point_compare
#define i_fwd
#define i_tag pnt
#include <stc/clist.h>

#define i_val float
#include <stc/clist.h>

#define i_val_str
#include <stc/clist.h>


int main()
{
    c_auto (clist_i32, lst)
        clist_i32_push_back(&lst, 123);

    c_auto (clist_pnt, plst) {
        c_apply(clist_pnt, push_back, &plst, {{42, 14}, {32, 94}, {62, 81}});
        clist_pnt_sort(&plst);

        c_foreach (i, clist_pnt, plst)
            printf(" (%d %d)", i.ref->x, i.ref->y);
        puts("");
    }

    c_auto (clist_float, flst) {
        c_apply(clist_float, push_back, &flst, {123.3, 321.2, -32.2, 78.2});
        c_foreach (i, clist_float, flst) printf(" %g", *i.ref);
    }

    c_auto (clist_str, slst)
        clist_str_emplace_back(&slst, "Hello, friend");
}