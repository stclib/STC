#define STC_IMPLEMENT
#include <stc/cstr.h>
#include <stc/csview.h>

int  main()
{
    // No memory allocations or string length calculations!
    const csview date = c_sv("2021/03/12");
    size_t pos = 0;
    const csview year = csview_token(date, "/", &pos);
    const csview month = csview_token(date, "/", &pos);
    const csview day = csview_token(date, "/", &pos);

    printf("%.*s, %.*s, %.*s\n", c_ARGSV(year), c_ARGSV(month), c_ARGSV(day));

    c_auto (cstr, y, m, d) {
        y = cstr_from_sv(year), m = cstr_from_sv(month), d = cstr_from_sv(day);
        printf("%s, %s, %s\n", cstr_str(&y), cstr_str(&m), cstr_str(&d));
    }
}
