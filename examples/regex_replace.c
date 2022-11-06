#define i_implement
#include <stc/cstr.h>
#include <stc/csview.h>
#include <stc/cregex.h>

bool add_10_years(int i, csview m, cstr* mstr) {
    if (i == 1) { // group 1 matches year
        int year;
        sscanf(m.str, "%4d", &year);
        cstr_printf(mstr, "%04d", year + 10);
        return true;
    }
    return false;
}

int main()
{
    const char* pattern = "\\b(\\d\\d\\d\\d)-(1[0-2]|0[1-9])-(3[01]|[12][0-9]|0[1-9])\\b";
    const char* input = "start date: 2015-12-31, end date: 2022-02-28";

    c_auto (cstr, str)
    {
        printf("INPUT: %s\n", input);

        /* replace with a fixed string, extended all-in-one call: */
        cstr_take(&str, cregex_replace_pattern(pattern, input, "YYYY-MM-DD", 0, NULL, cre_default));
        printf("fixed: %s\n", cstr_str(&str));

        /* US date format, and add 10 years to dates: */
        cstr_take(&str, cregex_replace_pattern(pattern, input, "$1/$3/$2", 0, add_10_years, cre_default));
        printf("us+10: %s\n", cstr_str(&str));

        /* Wrap first date inside []: */
        cstr_take(&str, cregex_replace_pattern(pattern, input, "[$0]", 1, NULL, cre_default));
        printf("brack: %s\n", cstr_str(&str));

        /* Shows how to compile RE separately */
        c_autodrop (cregex, re, cregex_from(pattern, cre_default)) {
            if (cregex_captures(&re) == 0)
                  continue; // break c_with
            /* European date format. */
            cstr_take(&str, cregex_replace(&re, input, "$3.$2.$1", 0));
            printf("euros: %s\n", cstr_str(&str));

            /* Strip out everything but the matches */
            cstr_take(&str, cregex_replace_sv(&re, csview_from(input), "$3.$2.$1;", 0, NULL, cre_r_strip));
            printf("strip: %s\n", cstr_str(&str));
        }

        /* Wrap all words in ${} */
        cstr_take(&str, cregex_replace_pattern("[a-z]+", "52 apples and 31 mangoes", "$${$0}", 0, NULL, cre_default));
        printf("curly: %s\n", cstr_str(&str));
    }
}

#include "../src/cregex.c"
#include "../src/utf8code.c"
