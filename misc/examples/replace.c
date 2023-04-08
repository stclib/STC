#include <stc/cstr.h>

int main ()
{
    const char *base = "this is a test string.";
    const char *s2 = "n example";
    const char *s3 = "sample phrase";

    // replace signatures used in the same order as described above:

    // Ustring positions:                                  0123456789*123456789*12345
    cstr s = cstr_from(base);                          // "this is a test string."
    cstr m = cstr_clone(s);

    cstr_append(&m, cstr_str(&m));
    cstr_append(&m, cstr_str(&m));
    printf("%s\n", cstr_str(&m));

    cstr_replace_at(&s, 9, 5, s2);                     // "this is an example string." (1)
    printf("(1) %s\n", cstr_str(&s));

    cstr_replace_at_sv(&s, 19, 6, c_sv(s3+7, 6));      // "this is an example phrase." (2)
    printf("(2) %s\n", cstr_str(&s));

    cstr_replace_at(&s, 8, 10, "just a");              // "this is just a phrase."     (3)
    printf("(3) %s\n", cstr_str(&s));

    cstr_replace_at_sv(&s, 8, 6, c_sv("a shorty", 7)); // "this is a short phrase."    (4)
    printf("(4) %s\n", cstr_str(&s));

    cstr_replace_at(&s, 22, 1, "!!!");                 // "this is a short phrase!!!"  (5)
    printf("(5) %s\n", cstr_str(&s));

    c_drop(cstr, &s, &m);
}
