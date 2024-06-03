#define i_import
#include "stc/cregex.h"

int main(void)
{
    struct { const char *pattern, *input; } s[] = {
        {"(\\d\\d\\d\\d)[-_](1[0-2]|0[1-9])[-_](3[01]|[12][0-9]|0[1-9])",
         "date: 2024-02-29 leapyear day, christmas eve is on 2022-12-24."
        },
        {"(https?://|ftp://|www\\.)([0-9A-Za-z@:%_+~#=-]+\\.)+([a-z][a-z][a-z]?)(/[/0-9A-Za-z\\.@:%_+~#=\\?&-]*)?",
         "https://en.cppreference.com/w/cpp/regex/regex_search"
        },
        {"!((abc|123)+)!", "!123abcabc!"},
        {"(\\p{Alpha}+ )+(\\p{Nd}+)", "Großpackung süßigkeiten 199"},
        {"\\p{Han}+", "This is Han: 王明：那是杂志吗？"},
    };

    cregex re = {0};
    c_forrange (i, c_arraylen(s))
    {
        int res = cregex_compile(&re, s[i].pattern);
        if (res < 0) {
            printf("error in regex pattern: %d\n", res);
            continue;
        }
        printf("\ninput: %s\n", s[i].input);

        c_formatch (j, &re, s[i].input) {
            c_forrange (k, cregex_captures(&re) + 1)
                printf("  submatch %d: %.*s\n", (int)k, c_SVARG(j.match[k]));
        }
    }
    cregex_drop(&re);
}
