#define i_implement
#include <stc/cstr.h>
#define i_implement
#include <stc/csview.h>

int main(void)
{
    cstr str = cstr_from("We think in generalities, but we live in details.");
    csview sv1 = cstr_substr_ex(&str, 3, 5);               // "think"
    intptr_t pos = cstr_find(&str, "live");                 // position of "live"
    csview sv2 = cstr_substr_ex(&str, pos, 4);             // "live"
    csview sv3 = cstr_slice_ex(&str, -8, -1);              // "details"
    printf("%.*s, %.*s, %.*s\n", c_SV(sv1), c_SV(sv2), c_SV(sv3));

    cstr_assign(&str, "apples are green or red");
    cstr s2 = cstr_from_sv(cstr_substr_ex(&str, -3, 3));    // "red"
    cstr s3 = cstr_from_sv(cstr_substr_ex(&str, 0, 6));     // "apples"
    printf("%s %s: %d, %d\n", cstr_str(&s2), cstr_str(&s3), 
                              cstr_is_long(&str), cstr_is_long(&s2));
    c_drop (cstr, &str, &s2, &s3);
}
