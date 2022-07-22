#define i_implement
#include <stc/cstr.h>
#include <stc/cregex.h>

cstr sub_20y(int i, csview m) {
    if (i == 1) { // year
        int year;
        sscanf(m.str, "%4d", &year);
        return cstr_from_fmt("%04d", year - 20);
    }
    return cstr_from_sv(m);
}

int main()
{
    const char* pattern = "\\b(\\d\\d\\d\\d)-(1[0-2]|0[1-9])-(3[01]|[12][0-9]|0[1-9])\\b";
    const char* input = "start date: 2015-12-31, end date: 2022-02-28";

    c_auto (cstr, str)
    {
        printf("input: %s\n\n", input);

        /* replace with a fixed string, extended all-in-one call: */
        cstr_take(&str, cregex_replace_p(input, pattern, "YYYY-MM-DD"));
        printf("fixed: %s\n", cstr_str(&str));

        /* US date format, and subtract 20 years: */
        cstr_take(&str, cregex_replace_pe(input, pattern, "\\1/\\3/\\2", sub_20y, 0, 0));
        printf("us-20: %s\n", cstr_str(&str));

        /* Wrap first date inside []: */
        cstr_take(&str, cregex_replace_pe(input, pattern, "[\\0]", NULL, 1, 0));
        printf("brack: %s\n", cstr_str(&str));

        /* Wrap all words in {} */
        cstr_take(&str, cregex_replace_p("[52] apples and [31] mangoes", "[a-z]+", "{\\0}"));
        printf("curly: %s\n", cstr_str(&str));

        /* European date format, compile RE separately */
        cregex re = cregex_from(pattern, 0);
        if (cregex_captures(&re) == 0)
              continue;
        cstr_take(&str, cregex_replace(input, &re, "\\3.\\2.\\1", NULL, 0));
        cregex_drop(&re);
        printf("euros: %s\n", cstr_str(&str));
    }
}

#include "../src/cregex.c"
#include "../src/utf8code.c"
