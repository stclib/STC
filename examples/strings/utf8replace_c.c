#include "stc/cstr.h"
#include "stc/zsview.h"

int main(void)
{
    cstr hello = cstr_lit("hell😀 w😀rld");
    printf("%s, %d:%d\n", cstr_str(&hello), (int)cstr_u8_size(&hello), (int)cstr_size(&hello));

    zsview sub5 = cstr_u8_tail(&hello, 5);
    printf("%s, %d:%d\n", sub5.str, (int)zsview_u8_size(sub5), (int)sub5.size);

    /* replace second smiley at utf8 codepoint pos 7 */

    cstr_u8_insert(&hello, 8, "🐨");
    printf("%s, %d:%d\n", cstr_str(&hello), (int)cstr_u8_size(&hello), (int)cstr_size(&hello));

    cstr_u8_erase(&hello, 7, 1);
    printf("%s, %d:%d\n", cstr_str(&hello), (int)cstr_u8_size(&hello), (int)cstr_size(&hello));

    cstr_u8_replace(&hello, 9, 1, "😀");
    printf("%s, %d:%d\n", cstr_str(&hello), (int)cstr_u8_size(&hello), (int)cstr_size(&hello));

    for (c_each(c, cstr, hello))
        printf(c_svfmt ",", c_svarg(c.chr));

    cstr str = cstr_lit("scooby, dooby doo");
    cstr_replace(&str, "oo", "00");
    printf("\n%s\n", cstr_str(&str));

    c_drop(cstr, &hello, &str);
}
