#include <stdio.h>
#include <time.h>
#include "stc/common.h"
#include "stc/random.h"

int main(void)
{
    const long long N = 10000000, range = 1000000;
    const uint64_t seed = (uint64_t)time(NULL);
    crand64 rng = crand64_from(seed);
    clock_t t;

    printf("Compare speed of full and unbiased ranged random numbers...\n");
    long long sum = 0;
    t = clock();
    for (c_range(N))  {
        sum += (int32_t)crand64_uint_r(&rng, 1);
    }
    t = clock() - t;
    printf("full range\t\t: %f secs, %d, avg: %f\n",
           (double)t/CLOCKS_PER_SEC, (int)N, (double)(sum/N));

    sum = 0;
    rng = crand64_from(seed);
    t = clock();
    for (c_range(N))  {
        sum += (int32_t)crand64_uint_r(&rng, 1) % (range + 1); // biased
    }
    t = clock() - t;
    printf("biased 0-%d  \t: %f secs, %d, avg: %f\n",
           (int)range, (double)t/CLOCKS_PER_SEC, (int)N, (double)(sum/N));
}
