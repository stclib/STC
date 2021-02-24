#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stc/crandom.h>
#include <stc/cstr.h>
#include <stc/cmap.h>
#include <stc/cvec.h>

// Declare int -> int hashmap. Uses typetag 'i' for ints.
using_cmap(i, int, size_t);

// Declare int vector with map entries that can be sorted by map keys.
static int compare(cmap_i_value_t *a, cmap_i_value_t *b) {
    return c_default_compare(&a->first, &b->first);
}
using_cvec(e, cmap_i_value_t, compare);

int main()
{
    enum {N = 10000000};
    const double Mean = -12.0, StdDev = 6.0, Scale = 74;

    printf("Demo of gaussian / normal distribution of %d random samples\n", N);

    // Setup random engine with normal distribution.
    uint64_t seed = time(NULL);
    stc64_t rng = stc64_init(seed);
    stc64_normalf_t dist = stc64_normalf_init(Mean, StdDev);

    // Create histogram map
    cmap_i mhist = cmap_i_init();
    c_forrange (N) {
        int index = (int) round( stc64_normalf(&rng, &dist) );
        ++ cmap_i_emplace(&mhist, index, 0).first->second;
    }

    // Transfer map to vec and sort it by map keys.
    cvec_e vhist = cvec_e_init();
    c_foreach (i, cmap_i, mhist)
        cvec_e_push_back(&vhist, *i.ref);
    cvec_e_sort(&vhist);

    // Print the gaussian bar chart
    cstr bar = cstr_init();
    c_foreach (i, cvec_e, vhist) {
        size_t n = (size_t) (i.ref->second * StdDev * Scale * 2.5 / N);
        if (n > 0) {
            cstr_resize(&bar, n, '*');
            printf("%4d %s\n", i.ref->first, bar.str);
        }
    }
    // Cleanup
    cstr_del(&bar);
    cmap_i_del(&mhist);
    cvec_e_del(&vhist);
}
