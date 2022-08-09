#define i_implement
#include <stc/cstr.h>
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
        cstr_take(&str, cregex_replace_pt(input, pattern, "YYYY-MM-DD", 0));
        printf("fixed: %s\n", cstr_str(&str));

        /* US date format, and add 10 years to dates: */
        cstr_take(&str, cregex_replace_pe(input, pattern, "$1/$3/$2", 0, 0, add_10_years));
        printf("us+10: %s\n", cstr_str(&str));

        /* Wrap first date inside []: */
        cstr_take(&str, cregex_replace_pt(input, pattern, "[$0]", 1));
        printf("brack: %s\n", cstr_str(&str));

        /* Shows how to compile RE separately */
        c_autovar (cregex re = cregex_from(pattern, 0), cregex_drop(&re)) {
            if (cregex_captures(&re) == 0)
                  c_breakauto;
            /* European date format. */
            cstr_take(&str, cregex_replace(input, &re, "$3.$2.$1", 0));
            printf("euros: %s\n", cstr_str(&str));

            /* Strip out everything but the matches */
            cstr_take(&str, cregex_replace_ex(input, &re, "$3.$2.$1;", 0, cre_r_strip, NULL));
            printf("strip: %s\n", cstr_str(&str));
        }

        /* Wrap all words in ${} */
        cstr_take(&str, cregex_replace_pt("[52] apples and [31] mangoes", "[a-z]+", "$${$0}", 0));
        printf("curly: %s\n", cstr_str(&str));
    }
}

#include "../src/cregex.c"
#include "../src/utf8code.c"
