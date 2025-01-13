#include <stdio.h>
#include <time.h>
#include <math.h>
#include "stc/random.h"

// Declare int -> int sorted map.
#define i_type SortedMap, int, int
#include "stc/smap.h"

int main(void)
{
    enum {N = 5000000};
    uint64_t seed = (uint64_t)time(NULL);
    crand64_seed(seed);
    const double Mean = round(crand64_real()*98.0 - 49.0),
                 StdDev = crand64_real()*10.0 + 1.0,
                 Scale = 74.0;

    printf("Demo of gaussian / normal distribution of %d random samples\n", N);
    printf("Mean %f, StdDev %f\n", Mean, StdDev);

    // Setup random normal distribution.
    crand64_normal_dist d = {.mean=Mean, .stddev=StdDev};

    // Create and init histogram map with defered destruct
    SortedMap hist = {0};

    for (c_range(N)) {
        int index = (int)round(crand64_normal(&d));
        SortedMap_insert(&hist, index, 0).ref->second += 1;
    }

    // Print the gaussian bar chart
    for (c_each_kv(index, count, SortedMap, hist)) {
        int n = (int)round(2.5 * Scale * StdDev * (*count) / (isize)N);
        if (n > 0) {
            printf("%4d ", *index);
            while (n--) printf("*");
            puts("");
        }
    }
    SortedMap_drop(&hist);
}
