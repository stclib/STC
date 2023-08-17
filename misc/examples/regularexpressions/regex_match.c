#define i_import
#include <stc/cregex.h>
#define i_implement
#include <stc/csview.h>

#define i_key float
#include <stc/cstack.h>

int main(void)
{
    // Lets find the first sequence of digits in a string
    const char *str = "Hello numeric world, there are 24 hours in a day, 3600 seconds in an hour."
                      " Around 365.25 days a year, and 52 weeks in a year."
                      " Boltzmann const: 1.38064852E-23, is very small."
                      " Bohrradius is 5.29177210903e-11, and Avogadros number is 6.02214076e23.";
    cregex re = {0};
    cstack_float vec = {0};

    const char* pattern = "[+-]?([0-9]*\\.)?\\d+([Ee][+-]?\\d+)?";
    int res = cregex_compile(&re, pattern);
    printf("%d: %s\n", res, pattern);

    // extract and convert all numbers in str to floats
    c_formatch (i, &re, str)
        cstack_float_push(&vec, (float)atof(i.match[0].buf));

    c_foreach (i, cstack_float, vec)
        printf("  %g\n", (double)*i.ref);

    // extracts the numbers only to a comma separated string.
    cstr nums = cregex_replace_sv(&re, csview_from(str), " $0,", 0, NULL, CREG_STRIP);
    printf("\n%s\n", cstr_str(&nums));

    cstr_drop(&nums);
    cregex_drop(&re);
    cstack_float_drop(&vec);
}
