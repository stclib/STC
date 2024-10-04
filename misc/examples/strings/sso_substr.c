#include "stc/cstr.h"
#include "stc/csview.h"

int main(void)
{
    cstr str = cstr_lit("We think in generalities, but we live in details.");
    csview sv = cstr_sv(&str);
    csview sv1 = csview_subview_ex(sv, 3, 5);               // "think"
    isize pos = csview_find(sv, "live");                   // position of "live"
    csview sv2 = csview_subview_ex(sv, pos, 4);             // "live"
    csview sv3 = csview_slice_ex(sv, -8, -1);              // "details"
    printf(c_svfmt ", " c_svfmt ", " c_svfmt "\n",
           c_svarg(sv1), c_svarg(sv2), c_svarg(sv3));

    cstr_assign(&str, "apples are green or red");
    sv = cstr_sv(&str);
    cstr s2 = cstr_from_sv(csview_subview_ex(sv, -3, 3));    // "red"
    cstr s3 = cstr_from_sv(csview_subview_ex(sv, 0, 6));     // "apples"
    printf("%s %s: %d, %d\n", cstr_str(&s2), cstr_str(&s3),
                              cstr_is_long(&str), cstr_is_long(&s2));
    c_drop(cstr, &str, &s2, &s3);
}
