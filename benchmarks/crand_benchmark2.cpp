#include <stdio.h>
#include <time.h>
#include <random>
#include "stc/crand.h"
#include "others/pcg_random.hpp"

static struct stc32_state { stc64_t rng; uint64_t spare; unsigned n; } stc32_global =
    {{0x7a5fed, 0x8e3f52, 0x9bc713, 0x6a09e667a7541669}, 0, 0};

STC_INLINE void stc32_srandom(uint64_t seed) { stc32_global.rng = stc64_init(seed); }
STC_INLINE uint32_t stc32_random(void) {
    return (uint32_t) (++stc32_global.n & 1 ? (stc32_global.spare = stc64_rand(&stc32_global.rng))
                                            : (stc32_global.spare >> 32));
}

static unsigned long myrand_next = 1;

/* RAND_MAX assumed to be 32767 */
int myrand(void) {
    myrand_next = myrand_next * 214013 + 2531011;
    return (myrand_next >> 16) & 0x7fff;
}

void mysrand(unsigned seed) {
    myrand_next = seed;
}


enum {N = 1000000000};

void test1(void)
{
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
    printf("std::random:\t\t%.02f, %zu, sz:%zu\n", (float) diff / CLOCKS_PER_SEC, sum, sizeof rng);

    before = clock();
    sum = 0;
    c_forrange (N) {
        sum += idist(rng);
    }
    diff = clock() - before;
    printf("std::uniform:\t\t%.02f, %zu\n\n", (float) diff / CLOCKS_PER_SEC, sum);

    c_forrange (30) printf("%02d ", idist(rng));
    puts("");
    c_forrange (8) printf("%f ", fdist(rng));
    puts("\n");
}

void test2()
{
    clock_t diff, before;
    uint64_t sum;

    // Seed with a real random value, if available
    pcg_extras::seed_seq_from<std::random_device> seed_source;

    // Make a random number engine
    pcg64 rng(seed_source);

    // Choose a random mean between 1 and 10
    std::uniform_int_distribution<int> idist(1, 10);
    std::uniform_real_distribution<double> fdist(1, 10);

   before = clock();
    sum = 0;
    c_forrange (N) {
        sum += rng();
    }
    diff = clock() - before;
    printf("pcg64::random:\t\t%.02f, %zu, sz:%zu\n", (float) diff / CLOCKS_PER_SEC, sum, sizeof rng);

    before = clock();
    sum = 0;
    c_forrange (N) {
        sum += idist(rng);
    }
    diff = clock() - before;
    printf("pcg64::uniform:\t\t%.02f, %zu\n\n", (float) diff / CLOCKS_PER_SEC, sum);

    c_forrange (30) printf("%02d ", idist(rng));
    puts("");
    c_forrange (8) printf("%f ", fdist(rng));
    puts("\n");
}


void test3(void)
{
    clock_t diff, before;
    uint64_t sum;

    stc64_t rng = stc64_init(time(NULL));
    stc64_uniform_t idist = stc64_uniform_init(1, 10);
    stc64_uniformf_t fdist = stc64_uniformf_init(1, 10);

    before = clock();
    sum = 0;
    c_forrange (N) {
        //sum += stc64_rand(&rng);
        sum += rand();
    }
    diff = clock() - before;
    printf("stc64_random:\t\t%.02f, %zu sz:%zu\n", (float) diff / CLOCKS_PER_SEC, sum, sizeof rng);

    before = clock();
    sum = 0;
    c_forrange (N) {
        sum += stc64_uniform(&rng, &idist);
    }
    diff = clock() - before;
    printf("stc64_uniform:\t\t%.02f, %zu\n\n", (float) diff / CLOCKS_PER_SEC, sum);

    c_forrange (30) printf("%02zd ", stc64_uniform(&rng, &idist));
    puts("");
    c_forrange (8) printf("%f ", stc64_uniformf(&rng, &fdist));
    puts("\n");
}

int main()
{
    test1();
    test2();
    test3();
}