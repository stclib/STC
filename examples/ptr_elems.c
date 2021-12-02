#include <stc/ccommon.h>
#include <stdio.h>

struct { double x, y; } typedef Point;

#define i_val Point*
#define i_from(val) c_new(Point, *(val))
#define i_del(pval) c_free(*(pval))
#define i_tag pnt
#include <stc/cvec.h>

#define i_key_str
#define i_val int*
#define i_valraw int
#define i_valfrom(raw) c_new(int, raw)
#define i_valto(pval) **(pval)
#define i_valdel(pval) c_free(*(pval))
#include <stc/cmap.h>

int main()
{
    c_auto (cvec_pnt, vec, cpy)
    {
        printf("Vector with pointer elements:\n");
        // c++: vec.push_back(new Point{1.2, 3.4});
        cvec_pnt_push_back(&vec, c_new(Point, {1.2, 3.4}));
        cvec_pnt_push_back(&vec, c_new(Point, {6.1, 4.7}));

        cpy = cvec_pnt_clone(vec);
        cpy.data[1]->x = 100;

        printf("vec:");
        c_foreach (i, cvec_pnt, vec)
            printf(" (%g %g)", (*i.ref)->x, (*i.ref)->y);
        printf("\ncpy:");
        c_foreach (i, cvec_pnt, cpy)
            printf(" (%g %g)", (*i.ref)->x, (*i.ref)->y);
        puts("");
    }

    c_auto (cmap_str, map)
    {
        printf("\nMap with pointer elements:\n");
        cmap_str_insert(&map, cstr_lit("testing"), c_new(int, 999));
        cmap_str_insert(&map, cstr_lit("done"), c_new(int, 111));

        // Emplace: implicit key, val construction using i_keyfrom/i_valfrom:
        cmap_str_emplace(&map, "hello", 200);
        cmap_str_emplace(&map, "goodbye", 400);

        c_foreach (i, cmap_str, map)
            printf("%s: %d\n", i.ref->first.str, *i.ref->second);
    }
}
