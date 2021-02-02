#include <stc/csmap.h>
#include <stc/crandom.h>
#include <stdio.h>

using_csmap(i, int, int);

int main()
{
    csmap_i map = csmap_i_init();
    stc64_srandom(1);
    c_forrange (i, 1000000) csmap_i_emplace(&map, stc64_random() & 0xffffff, i);

    size_t n = 0, sum = 0;
    c_foreach (i, csmap_i, map) {
        sum += i.ref->first;
        if (n++ < 20) printf("%d: %d\n", i.ref->first, i.ref->second);
    }
    printf("size %zu: %zu\n", csmap_i_size(map), sum);
    csmap_i_del(&map);
    puts("done");
}