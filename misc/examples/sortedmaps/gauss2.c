#include <stdio.h>
#include <time.h>

#define i_implement
#include <stc/cstr.h>
#include <stc/crand.h>

// Declare int -> int sorted map.
#define i_key int
#define i_val int
#include <stc/csmap.h>

int main(void)
{
    enum {N = 5000000};
    uint64_t seed = (uint64_t)time(NULL);
    crand_t rng = crand_init(seed);
    const double Mean = round(crand_f64(&rng)*98.0 - 49.0), StdDev = crand_f64(&rng)*10.0 + 1.0, Scale = 74.0;

    printf("Demo of gaussian / normal distribution of %d random samples\n", N);
    printf("Mean %f, StdDev %f\n", Mean, StdDev);

    // Setup random engine with normal distribution.
    crand_normal_t dist = crand_normal_init(Mean, StdDev);

    // Create and init histogram map with defered destruct
    csmap_int hist = {0};
    cstr bar = {0};

    c_forrange (N) {
        int index = (int)round(crand_normal(&rng, &dist));
        csmap_int_insert(&hist, index, 0).ref->second += 1;
    }

    // Print the gaussian bar chart
    c_forpair (index, count, csmap_int, hist) {
        int n = (int)round((double)*_.count * StdDev * Scale * 2.5 / (double)N);
        if (n > 0) {
            cstr_resize(&bar, n, '*');
            printf("%4d %s\n", *_.index, cstr_str(&bar));
        }
    }
    cstr_drop(&bar);
    csmap_int_drop(&hist);
}
