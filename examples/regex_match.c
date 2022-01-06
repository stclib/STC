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
                    " Bohrradius is 5.29177210903e-11, and Avogadros number is 6.02214076e23. That's it.";

    c_auto (cregex, re)
    {
        re = cregex_compile("[+-]?([0-9]*\\.)?[0-9]+([Ee][-+]?[0-9]+)?");
        cregex_match_t m;
        if (cregex_match(re, s, &m)) {
            printf("Found digits at position %u-%u\n", m.match_begin, m.match_end);
        } else {
            printf("Could not find any digits\n");
        }

        c_auto (cregex_res, matches) {
            matches = cregex_match_all(re, s);
            csview sv = csview_from(s);
            c_foreach (i, cregex_res, matches) {
                csview r = csview_slice(sv, i.ref->match_begin, i.ref->match_end);
                printf(c_svfmt " / ", c_svarg(r));
            }
        }
        puts("");
    }
}