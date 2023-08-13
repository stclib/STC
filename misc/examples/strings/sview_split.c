#define i_implement
#include <stc/cstr.h>
#define i_implement
#include <stc/csubstr.h>

int main(void)
{
    // No memory allocations or string length calculations!
    const csubstr date = c_ss("2021/03/12");
    intptr_t pos = 0;
    const csubstr year = csubstr_token(date, "/", &pos);
    const csubstr month = csubstr_token(date, "/", &pos);
    const csubstr day = csubstr_token(date, "/", &pos);

    printf("%.*s, %.*s, %.*s\n", c_SS(year), c_SS(month), c_SS(day));

    cstr y = cstr_from_ss(year), m = cstr_from_ss(month), d = cstr_from_ss(day);
    printf("%s, %s, %s\n", cstr_str(&y), cstr_str(&m), cstr_str(&d));
    c_drop(cstr, &y, &m, &d);
}
