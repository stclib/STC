#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stc/crandom.h>
#include <stc/cstr.h>
#include <stc/cmap.h>
#include <stc/cvec.h>

// Declare int -> int hashmap. Uses typetag 'i' for ints.
cdef_cmap(i, int, size_t);

// Declare int vector with map entries that can be sorted by map keys.
static int compare(cmap_i_entry_t *a, cmap_i_entry_t *b) {
    return c_default_compare(&a->key, &b->key);
}
// Vector: typetag 'e' for (map) entry
cdef_cvec(e, cmap_i_entry_t, c_default_destroy, compare);

int main()
{
    enum {N = 10000000};
    const double Mean = -12.0, StdDev = 8.0, Mag = 12000.0 / StdDev;

    printf("Demo of gaussian / normal distribution of %d random samples\n", N);
    
    // Setup random engine with normal distribution.
    uint64_t seed = time(NULL);    
    crand_rng64_t rng = crand_rng64_init(seed);
    crand_normal_f64_t dist = crand_normal_f64_init(Mean, StdDev);
    
    // Create histogram map
    cmap_i mhist = cmap_ini;
    for (size_t i = 0; i < N; ++i) {
        int index = (int) round( crand_normal_f64(&rng, &dist) );
        cmap_i_emplace(&mhist, index, 0).first->value += 1;
    }

    // Transfer map to vec and sort it by map keys.
    cvec_e vhist = cvec_ini;
    c_foreach (i, cmap_i, mhist)
        cvec_e_push_back(&vhist, *i.get);
    cvec_e_sort(&vhist);
    
    // Print the gaussian bar chart
    cstr_t bar = cstr_ini;
    c_foreach (i, cvec_e, vhist) {
        size_t n = (size_t) (i.get->value * Mag / N);
        if (n > 0) {
            // bar string: take ownership in new str after freeing current.
            cstr_take(&bar, cstr_with_size(n, '*'));
            printf("%4d %s\n", i.get->key, bar.str);
        }
    }
    // Cleanup
    cstr_destroy(&bar);
    cmap_i_destroy(&mhist);
    cvec_e_destroy(&vhist);
}
