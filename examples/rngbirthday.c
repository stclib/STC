
#include <stdio.h>
#include <time.h>

#include <stc/crandom.h>
#include <stc/cmap.h>
#include <stc/cvec.h>
#include <stc/cstr.h>

declare_CMap(ic, uint64_t, uint8_t);

const static uint64_t seed = 1234;
const static uint64_t N = 1ull << 27;
const static uint64_t mask = (1ull << 52) - 1;

void repeats(void)
{
    sfc64_random_t rng = sfc64_seed(seed);
    CMap_ic m = cmap_init;
    cmap_ic_reserve(&m, N);
    clock_t now = clock();
    for (size_t i = 0; i < N; ++i) {
        uint64_t k = sfc64_random(&rng) & mask;
        int v = ++cmap_ic_at(&m, k, 0)->value;
        if (v > 1) printf("%zu: %x - %d\n", i, k, v);
    }
    float diff = (float) (clock() - now) / CLOCKS_PER_SEC;
    printf("%.02f", diff);
}


declare_CMap(x, uint32_t, uint64_t);
declare_CVec(x, uint64_t);

void distribution(void)
{
    pcg32_random_t rng = pcg32_seed(seed, seed); // time(NULL), time(NULL));
    const size_t N = 1ull << 28, M = 1ull << 9; // 1ull << 10;
    CMap_x map = cmap_x_make(M);
    clock_t now = clock();
    for (size_t i = 0; i < N; ++i) {
        ++cmap_x_at(&map, pcg32_randomBounded(&rng, M), 0)->value;
    }
    float diff = (float) (clock() - now) / CLOCKS_PER_SEC;

    uint64_t sum = 0;
    c_foreach (i, cmap_x, map) sum += i.item->value;
    sum /= map.size;

    c_foreach (i, cmap_x, map)
        printf("%zu: %zu - %zu\n", i.item->key, i.item->value, sum);

    printf("%.02f\n", diff);
}

int main()
{
    repeats();
    //distribution();
}