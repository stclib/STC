#define i_implement
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
        csview m[5];
        if (cregex_find(s, &re, m, 0) == 1) {
            printf("Found digits at position %" PRIuMAX "-%" PRIuMAX "\n", m[0].str - s, m[0].str - s + m[0].size);
        } else {
            printf("Could not find any digits\n");
        }

        while (cregex_find(s, &re, m, cre_m_next) == 1) {
            printf("%.*s ; ", c_ARGsv(m[0]));
        }
        puts("");

        res = cregex_compile(&re, "(.+)\\b(.+)", 0);
        printf("groups: %d\n", res);
        if ((res = cregex_find("hello@wørld", &re, m, 0)) == 1) {
            c_forrange (i, res)
                printf("match: [%.*s]\n", c_ARGsv(m[i]));
        } else
            printf("err: %d\n", res);
    }
}

#include "../src/cregex.c"
#include "../src/utf8code.c"
