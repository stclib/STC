#include "stc/cstr.h"

int main(void)
{
    cstr hello = cstr_lit("hell😀 w😀rld");
    printf("%s\n", cstr_str(&hello));

    /* replace second smiley at utf8 codepoint pos 7 */
    cstr_u8_replace_at(&hello,
                        cstr_u8_to_index(&hello, 7),
                        1,
                        "🐨");
    printf("%s\n", cstr_str(&hello));

    c_foreach (c, cstr, hello)
        printf(c_svfmt ",", c_svarg(c.chr));

    cstr str = cstr_lit("scooby, dooby doo");
    cstr_replace(&str, "oo", "00");
    printf("\n%s\n", cstr_str(&str));

    c_drop(cstr, &hello, &str);
}
