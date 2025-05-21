#include <stc/cstr.h>
#include <stc/types.h>

declare_sortedmap(PntMap, struct Point, int);

// Use forward declared PntMap in struct
typedef struct {
    PntMap pntmap;
    cstr name;
} MyStruct;

// Point => int map
typedef struct Point { int x, y; } Point;
int Point_cmp(const Point* a, const Point* b) {
    int c = a->x - b->x;
    return c ? c : a->y - b->y;
}

#define T PntMap, Point, int, (c_cmpclass | c_declared)
#include <stc/sortedmap.h>

// cstr => cstr map
#define T StrMap, cstr, cstr, (c_keypro | c_valpro)
#include <stc/sortedmap.h>

// cstr set
#define T StrSet, cstr, (c_keypro)
#include <stc/sortedset.h>


int main(void)
{
    PntMap pmap = c_make(PntMap, {
        {{42, 14}, 1},
        {{32, 94}, 2},
        {{62, 81}, 3},
    });

    StrMap smap = c_make(StrMap, {
        {"Hello, friend", "this is the mapped value"},
        {"The brown fox", "jumped"},
        {"This is the time", "for all good things"},
    });

    for (c_each_kv(p, i, PntMap, pmap))
        printf(" (%d,%d: %d)", p->x, p->y, *i);
    puts("");

    for (c_each_kv(i, j, StrMap, smap))
        printf(" (%s: %s)\n", cstr_str(i), cstr_str(j));

    StrSet sset = {0};
    StrSet_emplace(&sset, "Hello, friend");
    StrSet_emplace(&sset, "Goodbye, foe");
    printf("Found? %s\n", StrSet_contains(&sset, "Hello, friend") ? "true" : "false");

    PntMap_drop(&pmap);
    StrMap_drop(&smap);
    StrSet_drop(&sset);
}
