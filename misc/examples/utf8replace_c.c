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
            printf("%.*s,", c_ARGSV(c.u8.chr));
        
        //csview sv = c_SV("If you find the time, you will find the winner");
        //str = cstr_replace_sv(sv, c_SV("find"), c_SV("match"), 0);

        str = cstr_lit("If you find the time, you will find the winner");
        cstr_replace(&str, "find", "match");
        printf("\n%s\n", cstr_str(&str));
    }
}
