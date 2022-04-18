#include <math.h>
#include <stdio.h>
#include <time.h>
#include <stc/crandom.h>

#define i_tag ic
#define i_key uint64_t
#define i_val uint8_t
#include <stc/cmap.h>

static uint64_t seed = 12345;

static void test_repeats(void)
{
    enum {BITS = 46, BITS_TEST = BITS/2 + 2};
    const static uint64_t N = 1ull << BITS_TEST;
    const static uint64_t mask = (1ull << BITS) - 1;

    printf("birthday paradox: value range: 2^%d, testing repeats of 2^%d values\n", BITS, BITS_TEST);
    stc64_t rng = stc64_new(seed);
    c_auto (cmap_ic, m)
    {
        cmap_ic_reserve(&m, N);
        c_forrange (i, N) {
            uint64_t k = stc64_rand(&rng) & mask;
            int v = cmap_ic_insert(&m, k, 0).ref->second += 1;
            if (v > 1) printf("repeated value %" PRIuMAX " (%d) at 2^%d\n", k, v, (int) log2((double) i));
        }
    }
}

#define i_key uint32_t
#define i_val uint64_t
#define i_tag x
#include <stc/cmap.h>

void test_distribution(void)
{
    enum {BITS = 26};
    printf("distribution test: 2^%d values\n", BITS);
    stc64_t rng = stc64_new(seed);
    const size_t N = 1ull << BITS ;

    c_auto (cmap_x, map) {
        c_forrange (N) {
            uint64_t k = stc64_rand(&rng);
            cmap_x_insert(&map, k & 0xf, 0).ref->second += 1;
        }

        uint64_t sum = 0;
        c_foreach (i, cmap_x, map) sum += i.ref->second;
        sum /= map.size;

        c_foreach (i, cmap_x, map) {
            printf("%4u: %" PRIuMAX " - %" PRIuMAX ": %11.8f\n", i.ref->first, i.ref->second, sum, (1 - (double) i.ref->second / sum));
        }
    }
}

int main()
{
    seed = time(NULL);
    test_distribution();
    test_repeats();
}
