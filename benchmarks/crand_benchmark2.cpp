#include <stdio.h>
#include <time.h>
#include <stc/crand.h>
#include <random>

void test1(void)
{
    enum {N = 1000000000};
    clock_t diff, before;
    uint64_t sum;

    std::random_device device;
    std::mt19937 rng(device());
    std::uniform_int_distribution<int> idist(1, 10);
    std::uniform_real_distribution<double> fdist(1, 10);

    before = clock();
    sum = 0;
    c_forrange (N) {
        sum += rng();
    }
    diff = clock() - before;
    printf("std::random:\t\t%.02f, %zu\n", (float) diff / CLOCKS_PER_SEC, sum);

    before = clock();
    sum = 0;
    c_forrange (N) {
        sum += idist(rng);
    }
    diff = clock() - before;
    printf("std::uniform:\t\t%.02f, %zu\n\n", (float) diff / CLOCKS_PER_SEC, sum);

    c_forrange (30) printf("%02zd ", idist(rng));
    puts("");
    c_forrange (8) printf("%f ", fdist(rng));
    puts("\n");
}

void test2(void)
{
    enum {N = 1000000000};
    clock_t diff, before;
    uint64_t sum;

    crand_t rng = crand_init(time(NULL));
    crand_uniform_t idist = crand_uniform_init(1, 10);
    crand_uniformf_t fdist = crand_uniformf_init(1, 10);

    before = clock();
    sum = 0;
    c_forrange (N) {
        sum += crand_next(&rng);
    }
    diff = clock() - before;
    printf("crand_next:\t\t%.02f, %zu\n", (float) diff / CLOCKS_PER_SEC, sum);

    before = clock();
    sum = 0;
    c_forrange (N) {
        sum += crand_uniform(&rng, &idist);
    }
    diff = clock() - before;
    printf("crand_uniform:\t\t%.02f, %zu\n\n", (float) diff / CLOCKS_PER_SEC, sum);

    c_forrange (30) printf("%02zd ", crand_uniform(&rng, &idist));
    puts("");
    c_forrange (8) printf("%f ", crand_uniformf(&rng, &fdist));
    puts("\n");
}

int main()
{
    test1();
    test2();
}