#include <stdio.h>
#include <stc/algorithm.h>

#define T IVec, int
#include <stc/stack.h>

#define T IMap, int, int
#include <stc/hashmap.h>


int main(void)
{
    puts("for c_range:");
    for (c_range(30)) printf(" xx");
    puts("");

    for (c_range(i, 30)) printf(" %d", (int)i);
    puts("");

    for (c_range(i, 30, 60)) printf(" %d", (int)i);
    puts("");

    for (c_range(i, 30, 90, 2)) printf(" %d", (int)i);

    puts("\n\nfor c_items:");
    for (c_items(i, int, {12, 23, 453, 65, 676}))
        printf(" %d", *i.ref);
    puts("");

    for (c_items(i, const char*, {"12", "23", "453", "65", "676"}))
        printf(" %s", *i.ref);
    puts("");

    IVec vec = c_make(IVec, {12, 23, 453, 65, 113, 215, 676, 34, 67, 20, 27, 66, 189, 45, 280, 199});
    IMap map = c_make(IMap, {{12, 23}, {453, 65}, {676, 123}, {34, 67}});

    puts("\n\nfor c_each:");
    for (c_each(i, IVec, vec))
        printf(" %d", *i.ref);

    puts("\n\nfor c_each in a map:");
    for (c_each(i, IMap, map))
        printf(" (%d %d)", i.ref->first, i.ref->second);

    puts("\n\nfor c_each_item:");
    for (c_each_item(e, IMap, map))
        printf(" (%d %d)", e->first, e->second);

    puts("\n\nfor c_each_kv:");
    for (c_each_kv(key, val, IMap, map))
        printf(" (%d %d)", *key, *val);

    #define f_isOdd() (*value & 1)

    puts("\n\nc_filter:");
    c_filter(IVec, vec, true
        && f_isOdd()
        && c_flt_skip(4)
        && (printf(" %d", *value), c_flt_take(4))
    );

    IVec_drop(&vec);
    IMap_drop(&map);
}
