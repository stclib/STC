#include <stdio.h>
#include <time.h>
#include <stc/crandom.h>
#include <stc/cstr.h>

int main()
{
    enum {R = 30};
    const size_t N = 1000000000;
    clock_t difference, before;    
    uint64_t sum = 0;

    uint64_t seed = time(NULL);
    crand_rng32_t pcg = crand_rng32_init(seed);
    uint32_t range = crand_i32(&pcg) & ((1u << 28) - 1);
    crand_uniform_i32_t dist0 = crand_uniform_i32_init(0, range);

    printf("32 uniform: %u\n", dist0.range);
    double fsum = 0;
    before = clock();
    for (size_t i = 0; i < N; ++i) {
        fsum += (double) crand_uniform_i32(&pcg, &dist0) / dist0.range;
    }
    difference = clock() - before;
    printf("%zu %f: %f secs\n", N, fsum / N, (float) difference / CLOCKS_PER_SEC);

    pcg = crand_rng32_init(seed);
    dist0 = crand_uniform_i32_init(0, range);
    puts("32 unbiased");
    fsum = 0;
    before = clock();
    for (size_t i = 0; i < N; ++i) {
        fsum += (double) crand_unbiased_i32(&pcg, &dist0) / dist0.range;
    }
    difference = clock() - before;
    printf("%zu %f: %f secs\n", N, fsum / N, (float) difference / CLOCKS_PER_SEC);

    puts("64 uniform");
    crand_rng64_t stc = crand_rng64_init(seed);
    crand_uniform_i64_t dist1 = crand_uniform_i64_init(0, N);
    sum = 0;
    before = clock();    
    for (size_t i = 0; i < N; ++i) {
        sum += crand_uniform_i64(&stc, &dist1);
    }
    difference = clock() - before;
    printf("%zu %f: %f secs\n", N, (double) sum / N, (float) difference / CLOCKS_PER_SEC);


    puts("normal distribution");
    crand_normal_f64_t dist2 = crand_normal_f64_init(R / 2.0, R / 6.0);
    size_t N2 = 10000000;
    int hist[R] = {0};
    sum = 0;
    for (size_t i = 0; i < N2; ++i) {
        int n = (int) (crand_normal_f64(&stc, &dist2) + 0.5);
        sum += n;
        if (n >= 0 && n < R) ++hist[n];
    }
    
    cstr_t bar = cstr_ini;
    for (int i=0; i < R; ++i) {
        cstr_take(&bar, cstr_with_size(hist[i] * 25ull * R / N2, '*'));
        printf("%2d %s\n", i, bar.str);
    }
    cstr_destroy(&bar);
}