#include "stc/cstr.h"
#include "stc/csview.h"

int main(void)
{
    // No memory allocations or string length calculations!
    const csview date = c_sv("2021/03/12");
    isize pos = 0;
    const csview year = csview_token(date, "/", &pos);
    const csview month = csview_token(date, "/", &pos);
    const csview day = csview_token(date, "/", &pos);

    printf(c_svfmt ", " c_svfmt ", " c_svfmt "\n",
           c_svarg(year), c_svarg(month), c_svarg(day));

    cstr y = cstr_from_sv(year), m = cstr_from_sv(month), d = cstr_from_sv(day);
    printf("%s, %s, %s\n", cstr_str(&y), cstr_str(&m), cstr_str(&d));
    c_drop(cstr, &y, &m, &d);
}
