#include <stc/cstr.h>
#include <stc/forward.h>

forward_csmap(csmap_pnt, struct Point, int);

struct MyStruct {
    csmap_pnt pntmap;
    cstr name;
} typedef MyStruct;

// int => int map
#define i_key int
#define i_val int
#include <stc/csmap.h>

// Point => int map
struct Point { int x, y; } typedef Point;
int point_compare(const Point* a, const Point* b) {
    int c = c_default_compare(&a->x, &b->x);
    return c ? c : c_default_compare(&a->y, &b->y);
}

#define i_key Point
#define i_val int
#define i_cmp point_compare
#define i_fwd
#define i_tag pnt
#include <stc/csmap.h>

// int => int map
#define i_key_str
#define i_val_str
#include <stc/csmap.h>

// string set
#define i_key_str
#include <stc/csset.h>


int main()
{
    c_auto (csmap_int, map)
        csmap_int_insert(&map, 123, 321);

    c_auto (csmap_pnt, pmap) {
        csmap_pnt_insert(&pmap, (Point){42, 14}, 1);
        csmap_pnt_insert(&pmap, (Point){32, 94}, 2);
        csmap_pnt_insert(&pmap, (Point){62, 81}, 3);
        c_foreach (i, csmap_pnt, pmap)
            printf(" (%d,%d: %d)", i.ref->first.x, i.ref->first.y, i.ref->second);
        puts("");
    }

    c_auto (csmap_str, smap) {
        csmap_str_emplace(&smap, "Hello, friend", "this is the mapped value");
        csmap_str_emplace(&smap, "The brown fox", "jumped");
        csmap_str_emplace(&smap, "This is the time", "for all good things");
        c_foreach (i, csmap_str, smap)
            printf(" (%s: %s)\n", i.ref->first.str, i.ref->second.str);
    }

    c_auto (csset_str, sset) {
        csset_str_emplace(&sset, "Hello, friend");
        csset_str_emplace(&sset, "Goodbye, foe");
        printf("Found? %s\n", csset_str_contains(&sset, "Hello, friend") ? "true" : "false");
    }
}