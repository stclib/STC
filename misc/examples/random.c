#include <stdio.h>
#include <time.h>
#include <stc/crand.h>

int main(void)
{
    const size_t N = 10000000;
    const uint64_t seed = (uint64_t)time(NULL), range = 1000000;
    crand_t rng = crand_init(seed);

    int64_t sum;
    clock_t diff, before;

    printf("Compare speed of full and unbiased ranged random numbers...\n");
    sum = 0;
    before = clock();
    c_forrange (N)  {
        sum += (uint32_t)crand_u64(&rng);
    }
    diff = clock() - before;
    printf("full range\t\t: %f secs, %d, avg: %f\n",
           (double)diff/CLOCKS_PER_SEC, N, (double)sum/N);

    crand_unif_t dist1 = crand_unif_init(0, range);
    rng = crand_init(seed);
    sum = 0;
    before = clock();
    c_forrange (N)  {
        sum += crand_unif(&rng, &dist1); // unbiased
    }
    diff = clock() - before;
    printf("unbiased 0-%" PRIu64 "\t: %f secs, %d, avg: %f\n",
            range, (double)diff/CLOCKS_PER_SEC, N, (double)sum/N);

    sum = 0;
    rng = crand_init(seed);
    before = clock();
    c_forrange (N)  {
        sum += (int64_t)(crand_u64(&rng) % (range + 1)); // biased
    }
    diff = clock() - before;
    printf("biased 0-%" PRIu64 "  \t: %f secs, %d, avg: %f\n",
            range, (double)diff/CLOCKS_PER_SEC, N, (double)sum/N);

}
