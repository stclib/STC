#include <stc/cstr.h>
#include <stc/forward.h>

forward_csmap(PMap, struct Point, int);

// Use forward declared PMap in struct
struct MyStruct {
    PMap pntmap;
    cstr name;
} typedef MyStruct;

// int => int map
#define i_key int
#define i_val int
#include <stc/csmap.h>

// Point => int map
struct Point { int x, y; } typedef Point;
int point_cmp(const Point* a, const Point* b) {
    int c = a->x - b->x;
    return c ? c : a->y - b->y;
}

#define i_type PMap
#define i_key Point
#define i_val int
#define i_cmp point_cmp
#define i_opt c_is_fwd
#include <stc/csmap.h>

// cstr => cstr map
#define i_type SMap
#define i_key_str
#define i_val_str
#include <stc/csmap.h>

// cstr set
#define i_type SSet
#define i_key_str
#include <stc/csset.h>


int main()
{
    c_auto (csmap_int, imap) {
        csmap_int_insert(&imap, 123, 321);
    }

    c_auto (PMap, pmap) {
        c_apply(v, PMap_insert(&pmap, c_pair(v)), PMap_value, {
            {{42, 14}, 1},
            {{32, 94}, 2},
            {{62, 81}, 3},
        });
        c_forpair (p, i, PMap, pmap)
            printf(" (%d,%d: %d)", _.p.x, _.p.y, _.i);
        puts("");
    }

    c_auto (SMap, smap) {
        c_apply(v, SMap_emplace(&smap, c_pair(v)), SMap_raw, {
            {"Hello, friend", "this is the mapped value"},
            {"The brown fox", "jumped"},
            {"This is the time", "for all good things"},
        });
        c_forpair (i, j, SMap, smap)
            printf(" (%s: %s)\n", cstr_str(&_.i), cstr_str(&_.j));
    }

    c_auto (SSet, sset) {
        SSet_emplace(&sset, "Hello, friend");
        SSet_emplace(&sset, "Goodbye, foe");
        printf("Found? %s\n", SSet_contains(&sset, "Hello, friend") ? "true" : "false");
    }
}
