#include <stc/cstr.h>
#include <stc/csview.h>

int main() {
    c_auto (cstr, hello) {
        hello = cstr_new("hell😀 w😀rld");
        printf("%s\n", cstr_str(&hello));

        cstr_replace_sv(
            &hello,
            csview_substr_u8(cstr_sv(&hello), 7, 1),
            c_sv("🐨")
        );
        printf("%s\n", cstr_str(&hello));

        cstr_replace(&hello, "🐨", "ø");
        printf("%s\n", cstr_str(&hello));

        c_foreach (c, cstr, hello)
            printf("%" c_PRIsv ",", c_ARGsv(c.chr));
        puts("");
    }
}
