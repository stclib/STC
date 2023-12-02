#define i_implement
#include "stc/cstr.h"
#include "stc/forward.h"

forward_hmap(hmap_pnt, struct Point, int);

typedef struct MyStruct {
    hmap_pnt pntmap;
    cstr name;
} MyStruct;

// int => int map
#define i_key int
#define i_val int
#include "stc/hmap.h"

// Point => int map
typedef struct Point { int x, y; } Point;

int point_cmp(const Point* a, const Point* b) {
    int c = a->x - b->x;
    return c ? c : a->y - b->y;
}

// Point => int map
#define i_key Point
#define i_val int
#define i_cmp point_cmp
#define i_hash c_default_hash
#define i_is_forward
#define i_tag pnt
#include "stc/hmap.h"

// cstr => cstr map
#define i_key_str
#define i_val_str
#include "stc/hmap.h"

// string set
#define i_key_str
#include "stc/hset.h"


int main(void)
{
    hmap_pnt pmap = c_init(hmap_pnt, {{{42, 14}, 1}, {{32, 94}, 2}, {{62, 81}, 3}});

    c_foreach (i, hmap_pnt, pmap)
        printf(" (%d, %d: %d)", i.ref->first.x, i.ref->first.y, i.ref->second);
    puts("");

    hmap_str smap = c_init(hmap_str, {
        {"Hello, friend", "long time no see"},
        {"So long", "see you around"},
    });

    hset_str sset = c_init(hset_str, {
        "Hello, friend",
        "Nice to see you again",
        "So long",
    });

    hmap_int map = {0};
    hmap_int_insert(&map, 123, 321);
    hmap_int_insert(&map, 456, 654);
    hmap_int_insert(&map, 789, 987);

    c_foreach (i, hset_str, sset)
        printf(" %s\n", cstr_str(i.ref));

    hmap_int_drop(&map);
    hset_str_drop(&sset);
    hmap_str_drop(&smap);
    hmap_pnt_drop(&pmap);
}
