#include "cstr.h"
#include "forward.h"

forward_cmap(pnt, struct Point, int);

struct MyStruct {
    cmap_pnt pntmap;
    cstr name;
} typedef MyStruct;


#define i_KEY int
#define i_VAL int
#include "cmap.h"

struct Point { int x, y; } typedef Point;
int point_compare(const Point* a, const Point* b) {
    int c = c_default_compare(&a->x, &b->x);
    return c ? c : c_default_compare(&a->y, &b->y);
}
#define f_TAG pnt
#define i_KEY Point
#define i_VAL int
#define i_CMP point_compare
#include "cmap.h"

#define i_KEY_str
#define i_VAL_str
#include "cmap.h"


#define i_KEY_str
#include "cset.h"


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