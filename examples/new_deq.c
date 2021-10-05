#include <stc/cstr.h>
#include <stc/forward.h>

forward_cdeq(cdeq_i32, int);
forward_cdeq(cdeq_pnt, struct Point);

struct MyStruct {
    cdeq_i32 intvec;
    cdeq_pnt pntvec;
} typedef MyStruct;


#define i_val int
#define i_fwd
#define i_tag i32
#include <stc/cdeq.h>

struct Point { int x, y; } typedef Point;
int point_compare(const Point* a, const Point* b) {
    int c = c_default_compare(&a->x, &b->x);
    return c ? c : c_default_compare(&a->y, &b->y);
}

#define i_val Point
#define i_cmp point_compare
#define i_fwd
#define i_tag pnt
#include <stc/cdeq.h>

#define i_val float
#include <stc/cdeq.h>

#define i_val_str
#include <stc/cdeq.h>


int main()
{
    cdeq_i32 vec = cdeq_i32_init();
    cdeq_i32_push_back(&vec, 123);
    cdeq_i32_del(&vec);

    cdeq_float fvec = cdeq_float_init();
    cdeq_float_push_back(&fvec, 123.3);
    cdeq_float_del(&fvec);

    cdeq_pnt pvec = cdeq_pnt_init();
    cdeq_pnt_push_back(&pvec, (Point){42, 14});
    cdeq_pnt_push_back(&pvec, (Point){32, 94});
    cdeq_pnt_push_front(&pvec, (Point){62, 81});
    cdeq_pnt_sort(&pvec);
    c_foreach (i, cdeq_pnt, pvec)
        printf(" (%d %d)", i.ref->x, i.ref->y);
    puts("");
    cdeq_pnt_del(&pvec);

    cdeq_str svec = cdeq_str_init();
    cdeq_str_emplace_back(&svec, "Hello, friend");
    cdeq_str_del(&svec);
}