#include <stc/cstr.h>
#include <stc/types.h>

declare_hashmap(Pntmap, struct Point, int);

typedef struct MyStruct {
    Pntmap pntmap;
    cstr name;
} MyStruct;

// int => int map
#define T Intmap, int, int
#include <stc/hashmap.h>

// Point => int map
typedef struct Point { int x, y; } Point;

// Point => int map, uses default hash function
#define T Pntmap, struct Point, int, (c_declared)
#define i_eq c_memcmp_eq
#include <stc/hashmap.h>

// cstr => cstr map
#define T Strmap, cstr, cstr, (c_pro_key|c_pro_val)
#include <stc/hashmap.h>

// string set
#define T Strset, cstr, (c_pro_key)
#include <stc/hashset.h>


int main(void)
{
    Pntmap pmap = c_make(Pntmap, {
        {{42, 14}, 1}, {{32, 94}, 2}, {{62, 81}, 3}
    });

    for (c_each_kv(k, v, Pntmap, pmap))
        printf(" (%d, %d: %d)", k->x, k->y, *v);
    puts("");

    Strmap smap = c_make(Strmap, {
        {"Hello, friend", "long time no see"},
        {"So long", "see you around"},
    });

    Strset sset = c_make(Strset, {
        "Hello, friend",
        "Nice to see you again",
        "So long",
    });

    Intmap map = {0};
    Intmap_insert(&map, 123, 321);
    Intmap_insert(&map, 456, 654);
    Intmap_insert(&map, 789, 987);

    for (c_each_ref(s, Strset, sset))
        printf(" %s\n", cstr_str(s));

    Intmap_drop(&map);
    Strset_drop(&sset);
    Strmap_drop(&smap);
    Pntmap_drop(&pmap);
}
