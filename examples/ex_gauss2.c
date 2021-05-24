#include <stdio.h>
#include <time.h>

#include <stc/crandom.h>
#include <stc/csmap.h>
#include <stc/cstr.h>

// Declare int -> int sorted map. Uses typetag 'i' for ints.
using_csmap(i, int, size_t);

int main()
{
    enum {N = 10000000};
    const double Mean = -12.0, StdDev = 6.0, Scale = 74;

    printf("Demo of gaussian / normal distribution of %d random samples\n", N);

    // Setup random engine with normal distribution.
    uint64_t seed = time(NULL);
    stc64_t rng = stc64_init(seed);
    stc64_normalf_t dist = stc64_normalf_init(Mean, StdDev);

    // Create and init histogram map with defered destruct
    c_forvar (csmap_i mhist = csmap_i_init(), csmap_i_del(&mhist))
    c_forvar (cstr bar = cstr_init(), cstr_del(&bar))
    {
        c_forrange (N) {
            int index = (int) round( stc64_normalf(&rng, &dist) );
            csmap_i_emplace(&mhist, index, 0).ref->second += 1;
        }

        // Print the gaussian bar chart
        c_foreach (i, csmap_i, mhist) {
            size_t n = (size_t) (i.ref->second * StdDev * Scale * 2.5 / (float)N);
            if (n > 0) {
                cstr_resize(&bar, n, '*');
                printf("%4d %s\n", i.ref->first, bar.str);
            }
        }
    }
}
