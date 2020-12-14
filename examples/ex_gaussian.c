#include <stdio.h>
#include <time.h>
#include <math.h>
#include "stc/crandom.h"
#include "stc/cstr.h"
#include "stc/cmap.h"
#include "stc/cvec.h"

// Declare int -> int hashmap. Uses typetag 'i' for ints.
using_cmap(i, int, size_t);

// Declare int vector with map entries that can be sorted by map keys.
static int compare(cmap_i_entry_t *a, cmap_i_entry_t *b) {
    return c_default_compare(&a->first, &b->first);
}
// Vector: typetag 'e' for (map) entry
using_cvec(e, cmap_i_entry_t, c_default_del, compare);

int main()
{
    enum {N = 10000000};
    const double Mean = -12.0, StdDev = 6.0, Scale = 74;

    printf("Demo of gaussian / normal distribution of %d random samples\n", N);

    // Setup random engine with normal distribution.
    uint64_t seed = time(NULL);
    cstc64_t rng = cstc64_init(seed);
    cstc64_normalf_t dist = cstc64_normalf_init(Mean, StdDev);

    // Create histogram map
    cmap_i mhist = cmap_i_init();
    for (size_t i = 0; i < N; ++i) {
        int index = (int) round( cstc64_normalf(&rng, &dist) );
        cmap_i_emplace(&mhist, index, 0).first->second += 1;
    }

    // Transfer map to vec and sort it by map keys.
    cvec_e vhist = cvec_e_init();
    c_foreach (i, cmap_i, mhist)
        cvec_e_push_back(&vhist, *i.val);
    cvec_e_sort(&vhist);

    // Print the gaussian bar chart
    cstr_t bar = cstr_init();
    c_foreach (i, cvec_e, vhist) {
        size_t n = (size_t) (i.val->second * StdDev * Scale * 2.5 / N);
        if (n > 0) {
            cstr_resize(&bar, n, '*');
            printf("%4d %s\n", i.val->first, bar.str);
        }
    }
    // Cleanup
    cstr_del(&bar);
    cmap_i_del(&mhist);
    cvec_e_del(&vhist);
}
