#define i_implement
#include "stc/cstr.h"
#define i_implement
#include "stc/csview.h"

int main(void)
{
    // No memory allocations or string length calculations!
    const csview date = c_sv("2021/03/12");
    intptr_t pos = 0;
    const csview year = csview_token(date, "/", &pos);
    const csview month = csview_token(date, "/", &pos);
    const csview day = csview_token(date, "/", &pos);

    printf("%.*s, %.*s, %.*s\n", c_SVARG(year), c_SVARG(month), c_SVARG(day));

    cstr y = cstr_from_sv(year), m = cstr_from_sv(month), d = cstr_from_sv(day);
    printf("%s, %s, %s\n", cstr_str(&y), cstr_str(&m), cstr_str(&d));
    c_drop(cstr, &y, &m, &d);
}
