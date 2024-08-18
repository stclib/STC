#include <stdio.h>
#include <time.h>

#define i_implement
#include "stc/cstr.h"
#include "stc/algo/random.h"

// Declare int -> int sorted map.
#define i_key int
#define i_val int
#include "stc/smap.h"

int main(void)
{
    enum {N = 5000000};
    uint64_t seed = (uint64_t)time(NULL);
    csrandom(seed);
    const double Mean = round(crandom_real()*98.0 - 49.0),
                 StdDev = crandom_real()*10.0 + 1.0,
                 Scale = 74.0;

    printf("Demo of gaussian / normal distribution of %d random samples\n", N);
    printf("Mean %f, StdDev %f\n", Mean, StdDev);

    // Setup random engine with normal distribution.
    crandom_normal_distr dist = {.mean=Mean, .stddev=StdDev};

    // Create and init histogram map with defered destruct
    smap_int hist = {0};
    cstr bar = {0};

    c_forrange (N) {
        int index = (int)round(crandom_normal(&dist));
        smap_int_insert(&hist, index, 0).ref->second += 1;
    }

    // Print the gaussian bar chart
    c_foreach_kv (index, count, smap_int, hist) {
        int n = (int)round((double)*count * StdDev * Scale * 2.5 / (double)N);
        if (n > 0) {
            cstr_resize(&bar, n, '*');
            printf("%4d %s\n", *index, cstr_str(&bar));
        }
    }
    cstr_drop(&bar);
    smap_int_drop(&hist);
}
