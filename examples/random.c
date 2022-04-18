#include <stdio.h>
#include <time.h>
#include <stc/crandom.h>

int main()
{
    const size_t N = 1000000000;
    const uint64_t seed = time(NULL), range = 1000000;
    stc64_t rng = stc64_new(seed);

    uint64_t sum;
    clock_t diff, before;

    printf("Compare speed of full and unbiased ranged random numbers...\n");
    sum = 0;
    before = clock();
    c_forrange (N)  {
        sum += (uint32_t) stc64_rand(&rng);
    }
    diff = clock() - before;
    printf("full range\t\t: %f secs, %" PRIuMAX ", avg: %f\n", (float) diff / CLOCKS_PER_SEC, N, (double) sum / N);

    stc64_uniform_t dist1 = stc64_uniform_new(0, range);
    rng = stc64_new(seed);
    sum = 0;
    before = clock();
    c_forrange (N)  {
        sum += stc64_uniform(&rng, &dist1); // unbiased
    }
    diff = clock() - before;
    printf("unbiased 0-%" PRIuMAX "\t: %f secs, %" PRIuMAX ", avg: %f\n", range, (float) diff / CLOCKS_PER_SEC, N, (double) sum / N);

    sum = 0;
    rng = stc64_new(seed);
    before = clock();
    c_forrange (N)  {
        sum += stc64_rand(&rng) % (range + 1); // biased
    }
    diff = clock() - before;
    printf("biased 0-%" PRIuMAX "  \t: %f secs, %" PRIuMAX ", avg: %f\n", range, (float) diff / CLOCKS_PER_SEC, N, (double) sum / N);

}
