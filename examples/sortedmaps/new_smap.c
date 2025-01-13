#include "stc/cstr.h"
#include "stc/types.h"

declare_smap(PMap, struct Point, int);

// Use forward declared PMap in struct
typedef struct {
    PMap pntmap;
    cstr name;
} MyStruct;

// Point => int map
typedef struct Point { int x, y; } Point;
int point_cmp(const Point* a, const Point* b) {
    int c = a->x - b->x;
    return c ? c : a->y - b->y;
}

#define i_type PMap,Point,int
#define i_cmp point_cmp
#define i_declared
#include "stc/smap.h"

// cstr => cstr map
#define i_type SMap
#define i_keypro cstr
#define i_valpro cstr
#include "stc/smap.h"

// cstr set
#define i_type SSet
#define i_keypro cstr
#include "stc/sset.h"


int main(void)
{
    PMap pmap = c_make(PMap, {
        {{42, 14}, 1},
        {{32, 94}, 2},
        {{62, 81}, 3},
    });
    SMap smap = c_make(SMap, {
        {"Hello, friend", "this is the mapped value"},
        {"The brown fox", "jumped"},
        {"This is the time", "for all good things"},
    });
    SSet sset = {0};

    for (c_each_kv(p, i, PMap, pmap))
        printf(" (%d,%d: %d)", p->x, p->y, *i);
    puts("");

    for (c_each_kv(i, j, SMap, smap))
        printf(" (%s: %s)\n", cstr_str(i), cstr_str(j));

    SSet_emplace(&sset, "Hello, friend");
    SSet_emplace(&sset, "Goodbye, foe");
    printf("Found? %s\n", SSet_contains(&sset, "Hello, friend") ? "true" : "false");

    PMap_drop(&pmap);
    SMap_drop(&smap);
    SSet_drop(&sset);
}
