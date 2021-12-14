#include <stc/csview.h>
#define c_arg(a) a
int  main()
{
    // No memory allocations or string length calculations!
    const csview date = c_sv("2021/03/12");
    const csview year = csview_first_token(date, c_sv("/"));
    const csview month = csview_next_token(date, c_sv("/"), year);
    const csview day = csview_next_token(date, c_sv("/"), month);

    printf(c_svfmt ", " c_svfmt ", " c_svfmt "\n", c_svarg(year), c_svarg(month), c_svarg(day));

    c_auto (cstr, y, m, d) {
        y = cstr_from_v(year), m = cstr_from_v(month), d = cstr_from_v(day);
        printf("%s, %s, %s\n", y.str, m.str, d.str);
    }
}