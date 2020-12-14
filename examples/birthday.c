
#include <stdio.h>
#include <time.h>

#include <stc/crandom.h>
#include <stc/cmap.h>
#include <stc/cvec.h>

using_cmap(ic, uint64_t, uint8_t);

const static uint64_t seed = 1234;
const static uint64_t N = 1ull << 27;
const static uint64_t mask = (1ull << 52) - 1;

void repeats(void)
{
    cstc64_t rng = cstc64_init(seed);
    cmap_ic m = cmap_ic_init();
    cmap_ic_reserve(&m, N);
    clock_t now = clock();
    c_forrange (i, N) {
        uint64_t k = cstc64_rand(&rng) & mask;
        int v = ++cmap_ic_emplace(&m, k, 0).first->second;
        if (v > 1) printf("%zu: %llx - %d\n", i, k, v);
    }
    float diff = (float) (clock() - now) / CLOCKS_PER_SEC;
    printf("%.02f", diff);
}


using_cmap(x, uint32_t, uint64_t);
using_cvec(x, uint64_t);

void distribution(void)
{
    cstc64_t rng = cstc64_init(seed); // time(NULL), time(NULL));
    const size_t N = 1ull << 28, M = 1ull << 9; // 1ull << 10;
    cmap_x map = cmap_x_with_capacity(M);
    clock_t now = clock();
    cstc64_uniform_t dist = cstc64_uniform_init(0, M);

    c_forrange (N) {
        ++cmap_x_emplace(&map, cstc64_uniform(&rng, &dist), 0).first->second;
    }
    float diff = (float) (clock() - now) / CLOCKS_PER_SEC;

    uint64_t sum = 0;
    c_foreach (i, cmap_x, map) sum += i.val->second;
    sum /= map.size;

    c_foreach (i, cmap_x, map)
        printf("%u: %zu - %zu\n", i.val->first, i.val->second, sum);

    printf("%.02f\n", diff);
}

int main()
{
    repeats();
    //distribution();
}