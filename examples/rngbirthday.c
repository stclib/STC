
#include <stdio.h>
#include <time.h>

#include <stc/crand.h>
#include <stc/cmap.h>
#include <stc/cvec.h>
#include <stc/cstr.h>

declare_cmap(ic, uint64_t, uint8_t);

const static uint64_t seed = 1234;
const static uint64_t N = 1ull << 27;
const static uint64_t mask = (1ull << 52) - 1;

void repeats(void)
{
    crand_eng64_t rng = crand_eng64(seed);
    cmap_ic m = cmap_init;
    cmap_ic_reserve(&m, N);
    clock_t now = clock();
    for (size_t i = 0; i < N; ++i) {
        uint64_t k = crand_gen_i64(&rng) & mask;
        int v = ++cmap_ic_insert(&m, k, 0)->value;
        if (v > 1) printf("%zu: %x - %d\n", i, k, v);
    }
    float diff = (float) (clock() - now) / CLOCKS_PER_SEC;
    printf("%.02f", diff);
}


declare_cmap(x, uint32_t, uint64_t);
declare_cvec(x, uint64_t);

void distribution(void)
{
    crand_eng32_t rng = crand_eng32(seed); // time(NULL), time(NULL));
    const size_t N = 1ull << 28, M = 1ull << 9; // 1ull << 10;
    cmap_x map = cmap_x_make(M);
    clock_t now = clock();
    crand_i32_uniform_t dist = crand_i32_uniform(0, M);
    for (size_t i = 0; i < N; ++i) {
        ++cmap_x_insert(&map, crand_gen_i32_uniform(&rng, dist), 0)->value;
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