#define i_extern // add utf8 dependencies
#include <stc/cstr.h>
#include <stc/csview.h>

int main() {
    c_auto (cstr, hello, upper) {
        hello = cstr_new("hell😀 w😀rld");
        printf("%s\n", cstr_str(&hello));

        /* replace second smiley at utf8 codepoint pos 7 */
        cstr_u8_replace(&hello, cstr_u8_to_pos(&hello, 7), 1, c_sv("🐨"));
        printf("%s\n", cstr_str(&hello));

        cstr_replace(&hello, "🐨", "ø", 1);
        printf("%s\n", cstr_str(&hello));

        upper = cstr_toupper_sv(cstr_sv(&hello));

        c_foreach (c, cstr, hello)
            printf("%.*s,", c_ARGSV(c.u8.chr));
        puts("");
    }
}
