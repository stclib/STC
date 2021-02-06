#include <stc/csmap.h>
#include <stc/cstr.h>
#include <stc/crandom.h>
#include <stdio.h>

using_csmap(i, int, size_t);
using_csset_str();

#include <time.h>

int main(int argc, char **argv)
{
    size_t n = 1000000;

    csmap_i map = csmap_i_with_capacity(n);
    time_t seed = time(NULL);

    uint64_t mask = (1ull << 31) - 1;
    csmap_i_iter_t it;

    stc64_srandom(seed);
    c_forrange (i, n) {
        uint64_t val = stc64_random() & mask;
        csmap_i_emplace(&map, val, i);
    }
    printf("size1: %zu, %zu\n", csmap_i_size(map), csmap_i_capacity(map));
    stc64_srandom(seed);
    c_forrange (n - 20)
        csmap_i_erase(&map, stc64_random() & mask);

    size_t val = 500000;
    c_forrange (i, n) {
        uint64_t val = stc64_random() & mask;
        csmap_i_emplace(&map, val, i);
    }

    printf("size2: %zu, %zu\n", csmap_i_size(map), csmap_i_capacity(map));

    int k = 0;
    c_foreach (i, csmap_i, map)
        if (k < 20) printf("%2d %d: %zu\n", ++k, i.ref->first, i.ref->second);

    csmap_i_find_it(&map, val, &it);
    printf("\nmin/max: %d -- %d: found: %d. size: %zu\n", csmap_i_front(&map)->first,
                                                          csmap_i_back(&map)->first,
                                                          it.ref->first,
                                                          csmap_i_size(map));
    c_foreach (i, csmap_i, it, csmap_i_end(&map))
        printf("-- %d: %zu\n", i.ref->first, i.ref->second);

    csmap_i_del(&map);
    puts("");


    c_init (csset_str, shouts, {"Hello", "Try this", "Awesome", "Works well", "Greetings"});

    c_foreach (i, csset_str, shouts)
        printf("shout: %s\n", i.ref->str);

    csset_str_del(&shouts);
}
