#include <stdio.h>
#include <stc/csview.h>
#include <stc/cregex.h>
#include <stc/crandom.h>
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
        if (cregex_find(re, s, &match)) {
            printf("Found digits at position %u-%u\n", match.start, match.end);
        } else {
            printf("Could not find any digits\n");
        }

        c_auto (cregex_result, matches) {
            matches = cregex_find_all(re, s);
            csview sv = csview_from(s);
            c_foreach (i, cregex_result, matches) {
                csview r = csview_slice(sv, i.ref->start, i.ref->end);
                printf(c_svfmt " / ", c_svarg(r));
            }
        }
        puts("");
    }
}