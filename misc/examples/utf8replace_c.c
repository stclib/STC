#include <stc/cstr.h>

int main() {
    c_AUTO (cstr, hello, str) {
        hello = cstr_lit("hell😀 w😀rld");
        printf("%s\n", cstr_str(&hello));

        /* replace second smiley at utf8 codepoint pos 7 */
        cstr_u8_replace_at(&hello, 
                           cstr_u8_to_pos(&hello, 7),
                           1,
                           c_SV("🐨")
        );
        printf("%s\n", cstr_str(&hello));

        c_FOREACH (c, cstr, hello)
            printf("%.*s,", c_SVARG(c.u8.chr));
        
        str = cstr_lit("scooby, dooby doo");
        cstr_replace(&str, "oo", "00");
        printf("\n%s\n", cstr_str(&str));
    }
}
