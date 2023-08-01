#include <stdio.h>
#include <time.h>
#include <stc/crand.h>

int main(void)
{
    const long long N = 10000000, range = 1000000;
    const uint64_t seed = (uint64_t)time(NULL);
    crand_t rng = crand_init(seed);
    clock_t t;

    printf("Compare speed of full and unbiased ranged random numbers...\n");
    long long sum = 0;
    t = clock();
    c_forrange (N)  {
        sum += (int32_t)crand_u64(&rng);
    }
    t = clock() - t;
    printf("full range\t\t: %f secs, %lld, avg: %f\n",
           (double)t/CLOCKS_PER_SEC, N, (double)(sum/N));

    crand_uniform_t dist1 = crand_uniform_init(0, range);
    rng = crand_init(seed);
    sum = 0;
    t = clock();
    c_forrange (N)  {
        sum += crand_uniform(&rng, &dist1); // unbiased
    }
    t = clock() - t;
    printf("unbiased 0-%lld\t: %f secs, %lld, avg: %f\n",
           range, (double)t/CLOCKS_PER_SEC, N, (double)(sum/N));

    sum = 0;
    rng = crand_init(seed);
    t = clock();
    c_forrange (N)  {
        sum += (int32_t)crand_u64(&rng) % (range + 1); // biased
    }
    t = clock() - t;
    printf("biased 0-%lld  \t: %f secs, %lld, avg: %f\n",
           range, (double)t/CLOCKS_PER_SEC, N, (double)(sum/N));
}
