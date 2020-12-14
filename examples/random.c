#include <stdio.h>
#include <time.h>
#include <stc/crandom.h>
#include <stc/cstr.h>

int main()
{
    enum {R = 30};
    const size_t N = 100000000;
    clock_t diff, before;    
    uint64_t sum = 0;
    uint64_t seed = time(NULL);

    cstc64_t stc = cstc64_init(seed);
    cstc64_uniform_t dist1 = cstc64_uniform_init(0, N);
    sum = 0;
    before = clock();    
    c_forrange (N)  {
        sum += cstc64_uniform(&stc, &dist1);
    }
    diff = clock() - before;
    printf("uniform: %zu %f: %f secs\n", N, (double) sum / N, (float) diff / CLOCKS_PER_SEC);

    cstc64_normalf_t dist2 = cstc64_normalf_init(R / 2.0, R / 6.0);
    size_t N2 = 10000000;
    int hist[R] = {0};
    sum = 0;
    before = clock();
    c_forrange (N2)  {
        int n = (int) (cstc64_normalf(&stc, &dist2) + 0.5);
        sum += n;
        if (n >= 0 && n < R) ++hist[n];
    }
    diff = clock() - before;
    printf("normal : %zu %f: %f secs\n", N, (double) sum / N2, (float) diff / CLOCKS_PER_SEC);

    cstr_t bar = cstr_inits;
    c_forrange (i, int, R)  {
        cstr_resize(&bar, hist[i] * 25ull * R / N2, '*');
        printf("%3d %s\n", i, bar.str);
    }
    cstr_del(&bar);
}