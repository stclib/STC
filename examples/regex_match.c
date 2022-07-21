#include <stc/cstr.h>
#include <stc/cregex.h>

int main()
{
    // Lets find the first sequence of digits in a string
    const char *s = "Hello numeric world, there are 24 hours in a day, 3600 seconds in an hour."
                    " Around 365.25 days a year, and 52 weeks in a year."
                    " Boltzmann const: 1.38064852E-23, is very small."
                    " Bohrradius is 5.29177210903e-11, and Avogadros number is 6.02214076e23.";

    c_auto (cregex, re)
    {
        int res = cregex_compile(&re, "[+-]?([0-9]*\\.)?\\d+([Ee][+-]?\\d+)?", 0);
        printf("%d\n", res);
        csview m[10];
        if (cregex_match_re(s, &re, 10, m, 0) > 0) {
            printf("Found digits at position %" PRIuMAX "-%" PRIuMAX "\n", m[0].str - s, m[0].str - s + m[0].size);
        } else {
            printf("Could not find any digits\n");
        }

        while (cregex_match_re(s, &re, 10, m, cre_NEXT) > 0) {
            printf("%" c_PRIsv " ; ", c_ARGsv(m[0]));
        }
        puts("");

        res = cregex_compile(&re, "(.+)\\b(.+)", 0);
        printf("groups: %d\n", res);
        if ((res = cregex_match_re("hello@wørld", &re, 10, m, 0)) > 0) {
            c_forrange (i, res)
                printf("match: [%" c_PRIsv "]\n", c_ARGsv(m[i]));
        } else
            printf("err: %d\n", res);
    }
}

#include "../src/cregex.c"
#include "../src/utf8code.c"
