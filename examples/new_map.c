#include <stc/cstr.h>
#include <stc/forward.h>

declare_cmap(cmap_pnt, struct Point, int);

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
#define i_opt c_declared
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
    c_auto (cmap_int, map)
    c_auto (cmap_pnt, pmap)
    c_auto (cmap_str, smap)
    c_auto (cset_str, sset)
    {
        cmap_int_insert(&map, 123, 321);

        c_forlist (i, cmap_pnt_raw, {{{42, 14}, 1}, {{32, 94}, 2}, {{62, 81}, 3}})
            cmap_pnt_insert(&pmap, c_PAIR(i.ref));

        c_foreach (i, cmap_pnt, pmap)
            printf(" (%d, %d: %d)", i.ref->first.x, i.ref->first.y, i.ref->second);
        puts("");

        c_forlist (i, cmap_str_raw, {
            {"Hello, friend", "long time no see"},
            {"So long, friend", "see you around"},
        }) cmap_str_emplace(&smap, c_PAIR(i.ref));

        c_forlist (i, const char*, {
            "Hello, friend",
            "Nice to see you again",
            "So long, friend",
        }) cset_str_emplace(&sset, *i.ref);

        c_foreach (i, cset_str, sset)
            printf(" %s\n", cstr_str(i.ref));
    }
}
