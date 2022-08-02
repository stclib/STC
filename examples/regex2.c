#define i_implement
#include <stc/cstr.h>
#include <stc/csview.h>
#include <stc/cregex.h>

int main()
{
    struct { const char *pattern, *input; } s[] = {
        {"(\\d\\d\\d\\d)[-_](1[0-2]|0[1-9])[-_](3[01]|[12][0-9]|0[1-9])",
         "date: 2024-02-29 leapyear day, christmas eve is on 2022-12-24."},
        {"(https?://|ftp://|www\\.)([0-9A-Za-z@:%_+~#=-]+\\.)+([a-z][a-z][a-z]?)(/[/0-9A-Za-z\\.@:%_+~#=\\?&-]*)?",
         "https://en.cppreference.com/w/cpp/regex/regex_search"},
        {"!((abc|123)+)!", "!123abcabc!"}
    };

    c_forrange (i, c_arraylen(s))
    {
        c_auto (cregex, re)
        {
            int res = cregex_compile(&re, s[i].pattern, 0);
            if (res < 0) {
                printf("error in regex pattern: %d\n", res);
                continue;
            }
            printf("input: %s\n", s[i].input);

            c_foreach_match (m, &re, s[i].input) {
                c_forrange (int, j, cregex_captures(&re))
                    printf("  submatch %d: %.*s\n", j, c_ARGsv(m[j]));
                puts("");
            }
        }
    }
}

#include "../src/cregex.c"
#include "../src/utf8code.c"
