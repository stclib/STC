#include <stdio.h>
#include <stc/csview.h>
#include <stc/cregex.h>
#include <stc/crandom.h>
#define i_val double
#define i_type Vecu64
#include <stc/cstack.h>
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
        re = cregex_new("[+-]?([0-9]*\\.)?[0-9]+([Ee][-+]?[0-9]+)?");
        cregex_match match;
        if (cregex_find(&re, s, &match)) {
            printf("Found digits at position %zu-%zu\n", match.start, match.end);
        } else {
            printf("Could not find any digits\n");
        }

        csview sv = {0};
        while (cregex_find_next_sv(&re, s, &sv)) {
            printf(c_PRIsv " ; ", c_ARGsv(sv));
        }
        puts("");
    }
}
