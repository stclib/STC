#define i_implement
#include "stc/cstr.h"
#include "stc/types.h"

forward_hmap(hmap_pnt, struct Point, int);

typedef struct MyStruct {
    hmap_pnt pntmap;
    cstr name;
} MyStruct;

// int => int map
#define i_TYPE hmap_int, int, int
#include "stc/hmap.h"

// Point => int map
typedef struct Point { int x, y; } Point;

// Point => int map, uses default hash function
#define i_TYPE hmap_pnt, struct Point, int
#define i_eq c_memcmp_eq
#define i_is_forward
#include "stc/hmap.h"

// cstr => cstr map
#define i_key_cstr
#define i_val_cstr
#include "stc/hmap.h"

// string set
#define i_key_cstr
#include "stc/hset.h"


int main(void)
{
    hmap_pnt pmap = c_init(hmap_pnt, {{{42, 14}, 1}, {{32, 94}, 2}, {{62, 81}, 3}});

    c_foreach (i, hmap_pnt, pmap)
        printf(" (%d, %d: %d)", i.ref->first.x, i.ref->first.y, i.ref->second);
    puts("");

    hmap_cstr smap = c_init(hmap_cstr, {
        {"Hello, friend", "long time no see"},
        {"So long", "see you around"},
    });

    hset_cstr sset = c_init(hset_cstr, {
        "Hello, friend",
        "Nice to see you again",
        "So long",
    });

    hmap_int map = {0};
    hmap_int_insert(&map, 123, 321);
    hmap_int_insert(&map, 456, 654);
    hmap_int_insert(&map, 789, 987);

    c_foreach (i, hset_cstr, sset)
        printf(" %s\n", cstr_str(i.ref));

    hmap_int_drop(&map);
    hset_cstr_drop(&sset);
    hmap_cstr_drop(&smap);
    hmap_pnt_drop(&pmap);
}
