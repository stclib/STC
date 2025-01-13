#include "stc/cstr.h"
#include "stc/cregex.h"
#include "stc/csview.h"

#define i_type Fvec, float
#include "stc/stack.h"

int main(void)
{
    // Lets find the first sequence of digits in a string
    const char *str = "Hello numeric world, there are 24 hours in a day, 3600 seconds in an hour. "
                      "Around 365.25 days a year, and 52 weeks in a year. "
                      "Boltzmann const: 1.38064852E-23, is very small. "
                      "Bohrradius is 5.29177210903e-11, and Avogadros number is 6.02214076e23.";
    cregex re = {0};
    Fvec vec = {0};

    const char* num_pattern = "[+-]?([0-9]*\\.)?\\d+([Ee][+-]?\\d+)?";
    int res = cregex_compile(&re, num_pattern);
    printf("%d: %s\n", res, num_pattern);

    // extract and convert all numbers in str to floats
    for (c_match(i, &re, str))
        Fvec_push(&vec, (float)atof(i.match[0].buf));

    for (c_each(i, Fvec, vec))
        printf("  %g\n", (double)*i.ref);

    // extracts the numbers only to a comma separated string.
    cstr nums = cregex_replace_pro(&re, csview_from(str), " $0,",
                                   INT32_MAX, NULL, CREG_STRIP);
    printf("\n%s\n", cstr_str(&nums));

    cstr_drop(&nums);
    cregex_drop(&re);
    Fvec_drop(&vec);
}
