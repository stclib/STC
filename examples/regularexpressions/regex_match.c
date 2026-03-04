#include <stc/cstr.h>
#include <stc/cregex.h>
#include <stc/csview.h>

#define T Fvec, double
#include <stc/stack.h>

int main(void)
{
    // Lets find the first sequence of digits in a string
    const char *str = "Hello numeric world, there are 24 hours in a day, 3600 seconds in an hour. "
                      "Around 365.25 days a year, and 52 weeks in a year. "
                      "Boltzmann const: 1.38064852E-23, is very small. "
                      "Bohrradius is 5.29177210903e-11, and Avogadros number is 6.02214076e23.";
    const char* num_pattern = "[+-]?([0-9]*\\.)?\\d+([Ee][+-]?\\d+)?";

    cregex re = cregex_from(num_pattern);
    printf("res: %d; %s\n", re.error, num_pattern);

    // extract and convert all numbers in str to floats
    Fvec vec = {0};
    for (c_match(i, &re, str)) {
        Fvec_push(&vec, atof(i.match[0].buf));
    }

    for (c_each(i, Fvec, vec)) {
        printf("  %g\n", *i.ref);
    }

    // extracts the numbers only to a comma separated string.
    cstr nums = cregex_replace_sv(&re, csview_from(str), " $0,", .flags=CREG_STRIP);
    printf("\n%s\n", cstr_str(&nums));

    cstr_drop(&nums);
    cregex_drop(&re);
    Fvec_drop(&vec);
}
