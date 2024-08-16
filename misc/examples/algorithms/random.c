#include <stdio.h>
#include <time.h>
#include "stc/common.h"
#include "stc/algo/random.h"

int main(void)
{
    const long long N = 10000000, range = 1000000;
    const uint64_t seed = (uint64_t)time(NULL);
    crandom_s rng = crandom_rng(seed);
    clock_t t;

    printf("Compare speed of full and unbiased ranged random numbers...\n");
    long long sum = 0;
    t = clock();
    c_forrange (N)  {
        sum += (int32_t)crandom_r(&rng);
    }
    t = clock() - t;
    printf("full range\t\t: %f secs, %lld, avg: %f\n",
           (double)t/CLOCKS_PER_SEC, N, (double)(sum/N));

    sum = 0;
    rng = crandom_rng(seed);
    t = clock();
    c_forrange (N)  {
        sum += (int32_t)crandom_r(&rng) % (range + 1); // biased
    }
    t = clock() - t;
    printf("biased 0-%lld  \t: %f secs, %lld, avg: %f\n",
           range, (double)t/CLOCKS_PER_SEC, N, (double)(sum/N));
}
