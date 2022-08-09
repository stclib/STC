#define i_implement
#include <stc/cstr.h>
#include <stc/csview.h>

int main() {
    c_auto (cstr, hello) {
        hello = cstr_new("hell😀 w😀rld");
        printf("%s\n", cstr_str(&hello));

        /* replace second smiley at utf8 codepoint pos 7 */
        cstr_u8_replace_at(&hello, 7, 1, c_sv("🐨"));

        printf("%s\n", cstr_str(&hello));

        cstr_replace(&hello, "🐨", "ø", 1);
        printf("%s\n", cstr_str(&hello));

        c_foreach (c, cstr, hello)
            printf("%.*s,", c_ARGsv(c.u8.chr));
        puts("");
    }
}
