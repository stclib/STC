#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stc/crandom.h>
#include <stc/cstr.h>

int main()
{
    enum {R = 30};
    const size_t N = 1000000000;
    uint64_t seed = 1234; // time(NULL);
    stc64_t rng = stc64_init(seed);

    uint64_t sum = 0;

    stc64_normalf_t dist2 = stc64_normalf_init((float)R / 2.0, (float)R / 6.0);
    size_t N2 = 10000000;
    int hist[R] = {0};
    sum = 0;
    c_forrange (N2)  {
        int n = (int) round((stc64_normalf(&rng, &dist2) + 0.5));
        sum += n;
        if (n >= 0 && n < R) ++hist[n];
    }
    cstr bar = cstr_init();
    c_forrange (i, int, R)  {
        cstr_resize(&bar, hist[i] * 25ull * R / N2, '*');
        printf("%3d %s\n", i, bar.str);
    }

    clock_t diff, before;

    sum = 0;
    before = clock();
    c_forrange (N)  {
        sum += stc64_rand(&rng);
    }
    diff = clock() - before;
    printf("random : %f secs, %zu %f\n", (float) diff / CLOCKS_PER_SEC, N, (double) sum / N);

    stc64_uniform_t dist1 = stc64_uniform_init(0, 1000);
    sum = 0;
    before = clock();
    c_forrange (N)  {
        sum += stc64_uniform(&rng, &dist1);
    }
    diff = clock() - before;
    printf("uniform: %f secs, %zu %f\n", (float) diff / CLOCKS_PER_SEC, N, (double) sum / N);

    cstr_del(&bar);
}