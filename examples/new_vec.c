#include <stc/cstr.h>
#include <stc/forward.h>

forward_cvec(cvec_i32, int);
forward_cvec(cvec_pnt, struct Point);

struct MyStruct {
    cvec_i32 intvec;
    cvec_pnt pntvec;
} typedef MyStruct;

#define i_fwd pnt
#define i_tag i32
#define i_val int
#include <stc/cvec.h>

struct Point { int x, y; } typedef Point;
int point_compare(const Point* a, const Point* b) {
    int c = c_default_compare(&a->x, &b->x);
    return c ? c : c_default_compare(&a->y, &b->y);
}

#define i_val Point
#define i_cmp point_compare
#define i_fwd pnt
#define i_tag pnt
#include <stc/cvec.h>

#define i_val float
#include <stc/cvec.h>

#define i_val_str
#include <stc/cvec.h>


int main()
{
    cvec_i32 vec = cvec_i32_init();
    cvec_i32_push_back(&vec, 123);
    cvec_i32_del(&vec);

    cvec_float fvec = cvec_float_init();
    cvec_float_push_back(&fvec, 123.3);
    cvec_float_del(&fvec);

    cvec_pnt pvec = cvec_pnt_init();
    cvec_pnt_push_back(&pvec, (Point){42, 14});
    cvec_pnt_push_back(&pvec, (Point){32, 94});
    cvec_pnt_push_back(&pvec, (Point){62, 81});
    cvec_pnt_sort(&pvec);
    c_foreach (i, cvec_pnt, pvec)
        printf(" (%d %d)", i.ref->x, i.ref->y);
    puts("");
    cvec_pnt_del(&pvec);

    cvec_str svec = cvec_str_init();
    cvec_str_emplace_back(&svec, "Hello, friend");
    cvec_str_del(&svec);
}