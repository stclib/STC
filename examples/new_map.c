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

int point_compare(const Point* a, const Point* b) {
    int c = c_default_compare(&a->x, &b->x);
    return c ? c : c_default_compare(&a->y, &b->y);
}

#define i_key Point
#define i_val int
#define i_cmp point_compare
#define i_fwd // forward declared
#define i_tag pnt
#include <stc/cmap.h>

// int => int map
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

        c_apply_pair(cmap_pnt, insert, &pmap, {
            {{42, 14}, 1}, {{32, 94}, 2}, {{62, 81}, 3}
        });
        c_foreach (i, cmap_pnt, pmap)
            printf(" (%d, %d: %d)", i.ref->first.x, i.ref->first.y, i.ref->second);
        puts("");

        c_apply_pair(cmap_str, emplace, &smap, {
            {"Hello, friend", "long time no see"},
            {"So long, friend", "see you around"},
        });

        c_apply(cset_str, emplace, &sset, {
            "Hello, friend",
            "Nice to see you again",
            "So long, friend",
        });
        c_foreach (i, cset_str, sset)
            printf(" %s\n", i.ref->str);
    }
}