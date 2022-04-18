#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stc/crandom.h>
#include <stc/cstr.h>

// Declare int -> int hashmap. Uses typetag 'ii' for ints.
#define i_key int32_t
#define i_val size_t
#define i_tag ii
#include <stc/cmap.h>

// Declare int vector with map entries that can be sorted by map keys.
struct {int first; size_t second;} typedef mapval;
static int compare(mapval *a, mapval *b) {
    return c_default_cmp(&a->first, &b->first);
}

#define i_val mapval
#define i_cmp compare
#define i_tag pair
#include <stc/cvec.h>

int main()
{
    enum {N = 10000000};
    const double Mean = -12.0, StdDev = 6.0, Scale = 74;

    printf("Demo of gaussian / normal distribution of %d random samples\n", N);

    // Setup random engine with normal distribution.
    uint64_t seed = time(NULL);
    stc64_t rng = stc64_new(seed);
    stc64_normalf_t dist = stc64_normalf_new(Mean, StdDev);

    // Create and init histogram vec and map with defered destructors:
    c_auto (cvec_pair, histvec)
    c_auto (cmap_ii, histmap)
    {
        c_forrange (N) {
            int index = (int) round( stc64_normalf(&rng, &dist) );
            cmap_ii_insert(&histmap, index, 0).ref->second += 1;
        }

        // Transfer map to vec and sort it by map keys.
        c_foreach (i, cmap_ii, histmap)
            cvec_pair_push_back(&histvec, (mapval){i.ref->first, i.ref->second});

        cvec_pair_sort(&histvec);

        // Print the gaussian bar chart
        c_auto (cstr, bar)
        c_foreach (i, cvec_pair, histvec) {
            size_t n = (size_t) (i.ref->second * StdDev * Scale * 2.5 / (float)N);
            if (n > 0) {
                cstr_resize(&bar, n, '*');
                printf("%4d %s\n", i.ref->first, cstr_str(&bar));
            }
        }
    }
}
