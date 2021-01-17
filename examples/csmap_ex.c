#include <stc/csmap.h>
#include <stc/cstr.h>
#include <stc/crand.h>
#include <stdio.h>

using_csmap(i, int, size_t);
using_csset_str();

#include <time.h>

int main(int argc, char **argv)
{
    csmap_i map = csmap_i_init();
    time_t seed = time(NULL);

    size_t n = 500000;
    uint64_t mask = (1ull << 20) - 1;
    csmap_i_iter_t it;

    stc64_srandom(seed);
    for (size_t i = 0; i < n; ++i) {
        uint64_t val = stc64_random() & mask;
        csmap_i_emplace(&map, val, i);
        if (!csmap_i_find(&map, val, &it)) {
            printf("Not found: %zu, %zu: ", i, val);
        }
    }
    stc64_srandom(seed);
    for (unsigned int i = 0; i < n - 20; ++i) {
        csmap_i_erase(&map, stc64_random() & mask);
    }
    
    csmap_i_emplace(&map, 500000, 5);

    c_foreach (i, csmap_i, map)
        printf("-- %d: %d\n", i.ref->first, i.ref->second);
    puts("");

    csmap_i_find(&map, 500000, &it);

    c_foreach (i, csmap_i, it, csmap_i_end(&map))
        printf("-- %d: %d\n", i.ref->first, i.ref->second);
    printf("\n%d: %d\n", 500000, *csmap_i_at(&map, 500000));

    printf("min/max: %d -- %d: %d: %zu\n\n", csmap_i_front(&map)->first,
                                             csmap_i_back(&map)->first, 
                                             csmap_i_find(&map, 500000, &it) != NULL,
                                             csmap_i_size(map));

    csmap_i_del(&map);
    puts("done\n");

    c_init (csset_str, names, {
        "Hello", "Try this", "Awesome", "Works well", "Greetings"
    });
    c_foreach (i, csset_str, names)
        printf("name: %s\n", i.ref->str);
    
    csset_str_del(&names);
    return 0;
}
