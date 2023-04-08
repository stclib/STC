#define i_extern
#include <stc/cregex.h>
#include <stc/csview.h>

bool add_10_years(int i, csview match, cstr* out) {
    if (i == 1) { // group 1 matches year
        int year;
        sscanf(match.str, "%4d", &year); // scan 4 chars only
        cstr_printf(out, "%04d", year + 10);
        return true;
    }
    return false;
}

int main()
{
    const char* pattern = "\\b(\\d\\d\\d\\d)-(1[0-2]|0[1-9])-(3[01]|[12][0-9]|0[1-9])\\b";
    const char* input = "start date: 2015-12-31, end date: 2022-02-28";
    cstr str = {0};
    cregex re = {0};

    c_defer(
        cregex_drop(&re),
        cstr_drop(&str)
    ){
        printf("INPUT: %s\n", input);

        /* replace with a fixed string, extended all-in-one call: */
        cstr_take(&str, cregex_replace_pattern(pattern, input, "YYYY-MM-DD"));
        printf("fixed: %s\n", cstr_str(&str));

        /* US date format, and add 10 years to dates: */
        cstr_take(&str, cregex_replace_pattern(pattern, input, "$1/$3/$2", 0, add_10_years, CREG_DEFAULT));
        printf("us+10: %s\n", cstr_str(&str));

        /* Wrap first date inside []: */
        cstr_take(&str, cregex_replace_pattern(pattern, input, "[$0]"));
        printf("brack: %s\n", cstr_str(&str));

        /* Shows how to compile RE separately */
        re = cregex_from(pattern);
        if (cregex_captures(&re) == 0)
            continue; /* break c_defer */

        /* European date format. */
        cstr_take(&str, cregex_replace(&re, input, "$3.$2.$1"));
        printf("euros: %s\n", cstr_str(&str));

        /* Strip out everything but the matches */
        cstr_take(&str, cregex_replace_sv(&re, csview_from(input), "$3.$2.$1;", 0, NULL, CREG_R_STRIP));
        printf("strip: %s\n", cstr_str(&str));

        /* Wrap all words in ${} */
        cstr_take(&str, cregex_replace_pattern("[a-z]+", "52 apples and 31 mangoes", "$${$0}"));
        printf("curly: %s\n", cstr_str(&str));
    }
}
