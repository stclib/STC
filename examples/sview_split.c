#include <stc/csview.h>

int  main()
{
    // No memory allocations or string length calculations!
    const csview date = c_sv("2021/03/12");
    size_t pos = 0;
    const csview year = csview_token(date, c_sv("/"), &pos);
    const csview month = csview_token(date, c_sv("/"), &pos);
    const csview day = csview_token(date, c_sv("/"), &pos);

    printf(c_svfmt ", " c_svfmt ", " c_svfmt "\n", 
           c_svarg(year), c_svarg(month), c_svarg(day));

    c_auto (cstr, y, m, d) {
        y = cstr_from_v(year), m = cstr_from_v(month), d = cstr_from_v(day);
        printf("%s, %s, %s\n", y.str, m.str, d.str);
    }
}
