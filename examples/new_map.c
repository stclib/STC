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
#define F_tag pnt  // F=forward declared
#define i_key Point
#define i_val int
#define i_cmp point_compare
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
    cmap_int map = cmap_int_init();
    cmap_int_insert(&map, 123, 321);
    cmap_int_del(&map);

    cmap_pnt pmap = cmap_pnt_init();
    cmap_pnt_insert(&pmap, (Point){42, 14}, 1);
    cmap_pnt_insert(&pmap, (Point){32, 94}, 2);
    cmap_pnt_insert(&pmap, (Point){62, 81}, 3);
    c_foreach (i, cmap_pnt, pmap)
        printf(" (%d,%d: %d)", i.ref->first.x, i.ref->first.y, i.ref->second);
    puts("");
    cmap_pnt_del(&pmap);

    cmap_str smap = cmap_str_init();
    cmap_str_emplace(&smap, "Hello, friend", "this is the mapped value");
    cmap_str_del(&smap);

    cset_str sset = cset_str_init();
    cset_str_emplace(&sset, "Hello, friend");
    cset_str_del(&sset);
}