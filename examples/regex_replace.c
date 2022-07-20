#define i_implement
#include <stc/cstr.h>
#include <stc/cregex.h>
#include <stc/csview.h>

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

    c_auto (cstr, str1, str2)
    {
        printf("input: %s\n", input);
        /* European date format */
        str1 = cregex_replace(input, pattern, "\\3.\\2.\\1");
        printf("euros: %s\n", cstr_str(&str1));

        /* US date format, and subtract 20 years: */
        str2 = cregex_replace_ex(input, pattern, "\\1/\\3/\\2", sub_20y, 0, 0);
        printf("us-20: %s\n", cstr_str(&str2));
    }
}

#include "../src/cregex.c"
#include "../src/utf8code.c"
