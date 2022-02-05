#include <stdio.h>
#include <stc/csview.h>
#include <stc/cregex.h>
#include <stc/crandom.h>
#define i_val double
#include <time.h>


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
        cregmatch m[10];
        if (cregex_find(&re, s, 10, m, 0) > 0) {
            printf("Found digits at position %zu-%zu\n", m[0].str - s, m[0].str - s + m[0].len);
        } else {
            printf("Could not find any digits\n");
        }

        while (cregex_find(&re, s, 1, m, creg_next) > 0) {
            printf("%.*s ; ", m[0].len, m[0].str);
        }
        puts("");
    }
}

#include "../src/cregex.c"
