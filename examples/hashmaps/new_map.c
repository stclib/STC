#include <stc/cstr.h>
#include <stc/types.h>

declare_hashmap(hmap_pnt, struct Point, int);

typedef struct MyStruct {
    hmap_pnt pntmap;
    cstr name;
} MyStruct;

// int => int map
#define T hmap_int, int, int
#include <stc/hashmap.h>

// Point => int map
typedef struct Point { int x, y; } Point;

// Point => int map, uses default hash function
#define T hmap_pnt, struct Point, int, (c_declared)
#define i_eq c_memcmp_eq
#include <stc/hashmap.h>

// cstr => cstr map
#define i_keypro cstr
#define i_valpro cstr
#include <stc/hashmap.h>

// string set
#define i_keypro cstr
#include <stc/hashset.h>


int main(void)
{
    hmap_pnt pmap = c_make(hmap_pnt, {
        {{42, 14}, 1}, {{32, 94}, 2}, {{62, 81}, 3}
    });

    for (c_each_kv(k, v, hmap_pnt, pmap))
        printf(" (%d, %d: %d)", k->x, k->y, *v);
    puts("");

    hmap_cstr smap = c_make(hmap_cstr, {
        {"Hello, friend", "long time no see"},
        {"So long", "see you around"},
    });

    hset_cstr sset = c_make(hset_cstr, {
        "Hello, friend",
        "Nice to see you again",
        "So long",
    });

    hmap_int map = {0};
    hmap_int_insert(&map, 123, 321);
    hmap_int_insert(&map, 456, 654);
    hmap_int_insert(&map, 789, 987);

    for (c_each_item(s, hset_cstr, sset))
        printf(" %s\n", cstr_str(s));

    hmap_int_drop(&map);
    hset_cstr_drop(&sset);
    hmap_cstr_drop(&smap);
    hmap_pnt_drop(&pmap);
}
