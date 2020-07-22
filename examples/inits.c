#include <stdio.h>
#include <stc/cmap.h>
#include <stc/cstr.h>

declare_CSet(sx, int);       // Set of int
declare_CMap(mx, int, char); // Map of int -> char
declare_CMap(ms, int, CStr, cstr_destroy); // Map of int -> CStr
declare_CMap_str(si, int);

int main(void) {
    int year = 2020;
    CMap_ms ms = cmap_ms_from((CMapInput_ms[]) {
        100, cstr_make("Hello"),
        110, cstr_make("World"),
        120, cstr_from("Howdy, -%d-", year),
    }, 3);

    c_foreach (i, cmap_ms, ms)
        printf("%d: %s\n", i.item->key, i.item->value.str);
    cmap_ms_destroy(&ms);
    // ------------------

    CMap_si si = cmap_si_from((CMapInput_si[]) {
        "Norway", 100,
        "Denmark", 50,
        "Iceland", 10
    }, 3);

    cmap_si_at(&si, "Sweden", 0)->value += 20;
    cmap_si_at(&si, "Norway", 0)->value += 20;
    cmap_si_at(&si, "Finland", 0)->value += 20;

    c_foreach (i, cmap_si, si)
        printf("%s: %d\n", i.item->key.str, i.item->value);
    cmap_si_destroy(&si);
    // ------------------

    CSet_sx s = cset_init;
    cset_sx_put(&s, 5);
    cset_sx_put(&s, 8);
    c_foreach (i, cset_sx, s) printf("set %d\n", i.item->key);
    cset_sx_destroy(&s);
    // ------------------

    CMap_mx m = cmap_mx_from((CMapInput_mx[]) {
        {5, 'a'}, {8, 'b'}, {12, 'c'}
    }, 3);

    CMapEntry_mx* e = cmap_mx_find(&m, 10); // = NULL
    char val = cmap_mx_find(&m, 5)->value;
    cmap_mx_put(&m, 5, 'd'); // update
    cmap_mx_erase(&m, 8);
    c_foreach (i, cmap_mx, m) printf("map %d: %c\n", i.item->key, i.item->value);
    cmap_mx_destroy(&m);
}