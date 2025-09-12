#include <math.h>
#include <stdio.h>
#include <time.h>
#include <stc/random.h>

#define T hmap_ui, uint64_t, int
#include <stc/hashmap.h>

static uint64_t seed = 12345;

static void test_repeats(void)
{
    enum {BITS = 46, BITS_TEST = BITS/2 + 2};
    static const isize N = (isize)(1ull << BITS_TEST);
    static const uint64_t mask = (1ull << BITS) - 1;

    printf("birthday paradox: value range: 2^%d, testing repeats of 2^%d values\n", BITS, BITS_TEST);
    crand64 rng = crand64_from(seed);

    hmap_ui m = hmap_ui_with_capacity(N);
    for (c_range(i, N)) {
        uint64_t k = crand64_uint_r(&rng, 1) & mask;
        int v = hmap_ui_insert(&m, k, 0).ref->second += 1;
        if (v > 1) printf("repeated value %" PRIu64 " (%d) at 2^%d\n",
                          k, v, (int)log2((double)i));
    }
    hmap_ui_drop(&m);
}

#define T hmap_uu, uint32_t, uint64_t
#include <stc/hashmap.h>

void test_distribution(void)
{
    enum {BITS = 24};
    printf("distribution test: 2^%d values\n", BITS);
    crand64 rng = crand64_from(seed);
    const isize N = (isize)(1ull << BITS);

    hmap_uu map = {0};
    for (c_range(N)) {
        uint64_t k = crand64_uint_r(&rng, 1);
        hmap_uu_insert(&map, k & 0xf, 0).ref->second += 1;
    }

    uint64_t sum = 0;
    for (c_each(i, hmap_uu, map)) sum += i.ref->second;
    sum /= (uint64_t)map.size;

    for (c_each_kv(k, v, hmap_uu, map)) {
        printf("%4" PRIu32 ": %" PRIu64 " - %" PRIu64 ": %11.8f\n",
                *k, *v, sum,
                (1.0 - (double)*v / (double)sum));
    }

    hmap_uu_drop(&map);
}

int main(void)
{
    seed = (uint64_t)time(NULL);
    test_distribution();
    test_repeats();
}
