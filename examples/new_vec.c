#include <stc/cstr.h>
#include <stc/forward.h>

forward_cvec(cvec_i32, int);
forward_cvec(cvec_pnt, struct Point);

struct MyStruct {
    cvec_i32 intvec;
    cvec_pnt pntvec;
} typedef MyStruct;

#define i_val int
#define i_opt c_is_fwd
#define i_tag i32
#include <stc/cvec.h>

struct Point { int x, y; } typedef Point;
int point_cmp(const Point* a, const Point* b) {
    int c = c_default_cmp(&a->x, &b->x);
    return c ? c : c_default_cmp(&a->y, &b->y);
}

#define i_val Point
#define i_cmp point_cmp
#define i_opt c_is_fwd
#define i_tag pnt
#include <stc/cvec.h>

#define i_val float
#include <stc/cvec.h>

#define i_val_str
#include <stc/cvec.h>


int main()
{
    c_auto (cvec_i32, vec)
    c_auto (cvec_float, fvec)
    c_auto (cvec_pnt, pvec)
    c_auto (cvec_str, svec)
    {
        cvec_i32_push_back(&vec, 123);
        cvec_float_push_back(&fvec, 123.3);

        cvec_pnt_push_back(&pvec, (Point){42, 14});
        cvec_pnt_push_back(&pvec, (Point){32, 94});
        cvec_pnt_push_back(&pvec, (Point){62, 81});
        cvec_pnt_sort(&pvec);
        c_foreach (i, cvec_pnt, pvec)
            printf(" (%d %d)", i.ref->x, i.ref->y);
        puts("");

        cvec_str_emplace_back(&svec, "Hello, friend");
    }
}
