#include <stc/ccommon.h>
#include <stdio.h>

struct { double x, y; } typedef Point;

// Set of Point pointers: define all template parameters "in-line"
// Note it may be simpler to use a cbox for this.
#define i_key Point*
#define i_keydrop(x) c_free(*(x))
#define i_keyclone(x) c_new(Point, *(x))
#define i_hash(x) c_default_hash(*(x))
#define i_cmp(x, y)  memcmp(*(x), *(y), sizeof **(x)) // not good!
#define i_tag pnt
#include <stc/cset.h>

#include <stc/cstr.h>
// Map of int64 pointers: Define i_valraw as int64_t for easy emplace calls!
typedef int64_t inttype;
#define i_key_str
#define i_val inttype*
#define i_valraw inttype
#define i_valfrom(raw) (puts("from"), c_new(inttype, raw))
#define i_valto(x) (puts("to"), **(x))
#define i_valclone c_derived_valclone // enables clone via valto+valfrom
#define i_valdrop(x) c_free(*(x))
#include <stc/cmap.h>

int main()
{
    c_auto (cset_pnt, set, cpy)
    {
        printf("Set with pointer elements:\n");
        // c++: set.insert(new Point{1.2, 3.4});
        cset_pnt_insert(&set, c_new(Point, {1.2, 3.4}));
        Point* q = *cset_pnt_insert(&set, c_new(Point, {6.1, 4.7})).ref;
        cset_pnt_insert(&set, c_new(Point, {5.7, 2.3}));

        cpy = cset_pnt_clone(set);
        cset_pnt_erase(&cpy, q);

        printf("set:");
        c_foreach (i, cset_pnt, set)
            printf(" (%g %g)", i.ref[0]->x, i.ref[0]->y);

        printf("\ncpy:");
        c_foreach (i, cset_pnt, cpy)
            printf(" (%g %g)", i.ref[0]->x, i.ref[0]->y);
        puts("");
    }

    c_auto (cmap_str, map, m2)
    {
        printf("\nMap with pointer elements:\n");
        cmap_str_insert(&map, cstr_new("testing"), c_new(inttype, 999));
        cmap_str_insert(&map, cstr_new("done"), c_new(inttype, 111));

        // Emplace: implicit key, val construction using i_keyfrom/i_valfrom:
        cmap_str_emplace(&map, "hello", 200);
        cmap_str_emplace(&map, "goodbye", 400);

        // default uses i_valfrom+i_valto when no i_valclone defined:
        m2 = cmap_str_clone(map);

        c_forpair (name, number, cmap_str, m2)
            printf("%s: %" PRIdMAX "\n", cstr_str(_.name), **_.number);
    }
}
