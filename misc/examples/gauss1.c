#include <time.h>
#include <math.h>

#include <stc/crandom.h>
#include <stc/cstr.h>

// Declare int -> int hashmap. Uses typetag 'ii' for ints.
#define i_key int
#define i_val int
#define i_tag ii
#include <stc/cmap.h>

// Declare int vector with entries from the cmap.
#define i_val cmap_ii_value
#define i_less(x, y) x->first < y->first
#define i_tag ii
#include <stc/cvec.h>

int main()
{
    enum {N = 10000000};
    const double Mean = -12.0, StdDev = 6.0, Scale = 74;

    printf("Demo of gaussian / normal distribution of %d random samples\n", N);

    // Setup random engine with normal distribution.
    uint64_t seed = (uint64_t)time(NULL);
    stc64_t rng = stc64_new(seed);
    stc64_normalf_t dist = stc64_normalf_new(Mean, StdDev);

    // Create and init histogram vec and map with defered destructors:
    c_AUTO (cvec_ii, histvec)
    c_AUTO (cmap_ii, histmap)
    {
        c_FORRANGE (N) {
            int index = (int)round( stc64_normalf(&rng, &dist) );
            cmap_ii_insert(&histmap, index, 0).ref->second += 1;
        }

        // Transfer map to vec and sort it by map keys.
        c_FOREACH (i, cmap_ii, histmap)
            cvec_ii_push(&histvec, (cmap_ii_value){i.ref->first, i.ref->second});

        cvec_ii_sort(&histvec);

        // Print the gaussian bar chart
        c_FOREACH (i, cvec_ii, histvec) {
            int n = (int)(i.ref->second * StdDev * Scale * 2.5 / (double)N);
            if (n > 0) {
                printf("%4d ", i.ref->first);
                c_FORRANGE (n) printf("*");
                puts("");
            }
        }
    }
}
