#include <stc/cstr.h>
#include <stc/forward.h>

forward_cmap(cmap_pnt, struct Point, int);

struct MyStruct {
    cmap_pnt pntmap;
    cstr name;
} typedef MyStruct;

// int => int map
#define i_key int
#define i_val int
#include <stc/cmap.h>

// Point => int map
struct Point { int x, y; } typedef Point;

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
#include <stc/cmap.h>

// cstr => cstr map
#define i_key_str
#define i_val_str
#include <stc/cmap.h>

// string set
#define i_key_str
#include <stc/cset.h>


int main()
{
    cmap_pnt pmap = c_make(cmap_pnt, {{{42, 14}, 1}, {{32, 94}, 2}, {{62, 81}, 3}});

    c_foreach (i, cmap_pnt, pmap)
        printf(" (%d, %d: %d)", i.ref->first.x, i.ref->first.y, i.ref->second);
    puts("");

    cmap_str smap = c_make(cmap_str, {
        {"Hello, friend", "long time no see"},
        {"So long", "see you around"},
    });

    cset_str sset = c_make(cset_str, {
        "Hello, friend",
        "Nice to see you again",
        "So long",
    });

    cmap_int map = {0};
    cmap_int_insert(&map, 123, 321);
    cmap_int_insert(&map, 456, 654);
    cmap_int_insert(&map, 789, 987);

    c_foreach (i, cset_str, sset)
        printf(" %s\n", cstr_str(i.ref));

    cmap_int_drop(&map);
    cset_str_drop(&sset);
    cmap_str_drop(&smap);
    cmap_pnt_drop(&pmap);
}
