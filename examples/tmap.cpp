#include <stc/csmap.h>
#include <stc/cstr.h>
#include <stc/crand.h>
#include <stdio.h>
#include <map>

using_csmap(i, int, size_t);
//using_csset_str();

#include <time.h>

int main(int argc, char **argv)
{
    std::map<int, size_t> tmap;
    csmap_i map = csmap_i_init();
    csmap_i_iter_t it;
    time_t seed = time(NULL);

    size_t n = 2000000;
    uint64_t mask = (1ull << 22) - 1;

    clock_t t1 = clock();
    stc64_srandom(seed);
    for (size_t i = 0; i < n; ++i) {
        uint64_t x = stc64_random() & mask;
        tmap.emplace(x, i);
    }
    size_t s1 = tmap.size();
    stc64_srandom(seed);
    for (size_t i = 0; i < n - 50; ++i) {
        uint64_t x = stc64_random() & mask;
        tmap.erase(x);
    }
    clock_t t2 = clock();

    stc64_srandom(seed);
    for (size_t i = 0; i < n; ++i) {
        uint64_t x = stc64_random() & mask;
        csmap_i_emplace(&map, x, i);
    }
    size_t s2 = csmap_i_size(map);
    stc64_srandom(seed);
    for (size_t i = 0; i < n - 50; ++i) {
        uint64_t x = stc64_random() & mask;
        csmap_i_erase(&map, x);
    }
    clock_t t3 = clock();

    printf("%zu %zu\n", s1, s2);
    printf("%zu %zu\n", tmap.size(), csmap_i_size(map));

    printf("time: %f %f\n", ((float)t2-t1) / CLOCKS_PER_SEC, ((float)t3-t2) / CLOCKS_PER_SEC);
}
