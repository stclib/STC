#define STC_USE_SSO 1
#include <stc/cstr.h>
#include <stc/csview.h>

int main ()
{
    cstr str = cstr_new("We think in generalities, but we live in details.");
    csview sv1 = cstr_substr(&str, 3, 5);               // "think"
    size_t pos = cstr_find(str, "live");                // position of "live"
    csview sv2 = cstr_substr(&str, pos, 4);             // "live"
    csview sv3 = cstr_slice(&str, -8, -1);              // "details"
    printf(c_PRIsv ", " c_PRIsv ", " c_PRIsv "\n", 
           c_ARGsv(sv1), c_ARGsv(sv2), c_ARGsv(sv3));

    cstr_assign(&str, "apples are green or red");
    cstr s2 = cstr_from_sv(cstr_substr(&str, -3, 3));    // "red"
    cstr s3 = cstr_from_sv(cstr_substr(&str, 0, 6));     // "apples"
    printf("%s %s: %d, %d\n", cstr_str(&s2), cstr_str(&s3), 
                              cstr_is_long(&str), cstr_is_long(&s2));

    c_drop (cstr, &str, &s2, &s3);
}
