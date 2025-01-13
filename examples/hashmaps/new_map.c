#include "stc/cstr.h"
#include "stc/types.h"

declare_hmap(hmap_pnt, struct Point, int);

typedef struct MyStruct {
    hmap_pnt pntmap;
    cstr name;
} MyStruct;

// int => int map
#define i_type hmap_int, int, int
#include "stc/hmap.h"

// Point => int map
typedef struct Point { int x, y; } Point;

// Point => int map, uses default hash function
#define i_type hmap_pnt, struct Point, int
#define i_eq c_memcmp_eq
#define i_declared
#include "stc/hmap.h"

// cstr => cstr map
#define i_keypro cstr
#define i_valpro cstr
#include "stc/hmap.h"

// string set
#define i_keypro cstr
#include "stc/hset.h"


int main(void)
{
    hmap_pnt pmap = c_make(hmap_pnt, {{{42, 14}, 1}, {{32, 94}, 2}, {{62, 81}, 3}});

    for (c_each(i, hmap_pnt, pmap))
        printf(" (%d, %d: %d)", i.ref->first.x, i.ref->first.y, i.ref->second);
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

    for (c_each(i, hset_cstr, sset))
        printf(" %s\n", cstr_str(i.ref));

    hmap_int_drop(&map);
    hset_cstr_drop(&sset);
    hmap_cstr_drop(&smap);
    hmap_pnt_drop(&pmap);
}
