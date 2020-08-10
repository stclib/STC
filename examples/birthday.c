
#include <stdio.h>
#include <time.h>

#include <stc/crandom.h>
#include <stc/cmap.h>
#include <stc/cvec.h>
#include <stc/cstr.h>

declare_cmap(ic, uint64_t, uint8_t);

const static uint64_t seed = 1234;
const static uint64_t N = 1ull << 27;
const static uint64_t mask = (1ull << 52) - 1;

void repeats(void)
{
    crandom_eng64_t rng = crandom_eng64_init(seed);
    cmap_ic m = cmap_init;
    cmap_ic_reserve(&m, N);
    clock_t now = clock();
    for (size_t i = 0; i < N; ++i) {
        uint64_t k = crandom_i64(&rng) & mask;
        int v = ++cmap_ic_insert(&m, k, 0)->value;
        if (v > 1) printf("%zu: %llx - %d\n", i, k, v);
    }
    float diff = (float) (clock() - now) / CLOCKS_PER_SEC;
    printf("%.02f", diff);
}


declare_cmap(x, uint32_t, uint64_t);
declare_cvec(x, uint64_t);

void distribution(void)
{
    crandom_eng32_t rng = crandom_eng32_init(seed); // time(NULL), time(NULL));
    const size_t N = 1ull << 28, M = 1ull << 9; // 1ull << 10;
    cmap_x map = cmap_x_with_capacity(M);
    clock_t now = clock();
    crandom_distrib_i32_t dist = crandom_uniform_i32_init(0, M);
    for (size_t i = 0; i < N; ++i) {
        ++cmap_x_insert(&map, crandom_uniform_i32(&rng, dist), 0)->value;
    }
    float diff = (float) (clock() - now) / CLOCKS_PER_SEC;

    uint64_t sum = 0;
    c_foreach (i, cmap_x, map) sum += i.item->value;
    sum /= map.size;

    c_foreach (i, cmap_x, map)
        printf("%u: %zu - %zu\n", i.item->key, i.item->value, sum);

    printf("%.02f\n", diff);
}

int main()
{
    repeats();
    //distribution();
}