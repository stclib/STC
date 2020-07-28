#include <stdio.h>
#include <stc/cstr.h>
#include <stc/cmap.h>
#include <stc/cvec.h>
#include <stc/clist.h>

declare_cmap(id, int, cstr, cstr_destroy); // Map of int -> cstr
declare_cmap_str(cnt, int);

typedef struct {int x, y;} ipair_t;
declare_cvec(ip, ipair_t, c_default_destroy, c_mem_compare);
declare_clist(ip, ipair_t, c_default_destroy, c_mem_compare);


int main(void) {
    int year = 2020;
    cmap_id idnames = cmap_init;
    c_push(&idnames, cmap_id, c_items(
        {100, cstr_make("Hello")},
        {110, cstr_make("World")},
        {120, cstr_from("Howdy, -%d-", year)},
    ));

    c_foreach (i, cmap_id, idnames)
        printf("%d: %s\n", i.item->key, i.item->value.str);
    cmap_id_destroy(&idnames);

    // ------------------

    cmap_cnt countries = cmap_init;

    cmap_cnt_insert(&countries, "Greenland", 0)->value += 20;
    c_push(&countries, cmap_cnt, c_items(
        {"Norway", 100},
        {"Denmark", 50},
        {"Iceland", 10},
    ));

    cmap_cnt_insert(&countries, "Sweden", 0)->value += 20;
    cmap_cnt_insert(&countries, "Norway", 0)->value += 20;
    cmap_cnt_insert(&countries, "Finland", 0)->value += 20;

    c_foreach (i, cmap_cnt, countries)
        printf("%s: %d\n", i.item->key.str, i.item->value);
    cmap_cnt_destroy(&countries);

    // ------------------

    cvec_ip pairs1 = cvec_init; 
    c_push(&pairs1, cvec_ip, c_items(
        {1, 2},
        {3, 4},
        {5, 6},
        {7, 8},
    ));

    c_foreach (i, cvec_ip, pairs1)
        printf("(%d %d) ", i.item->x, i.item->y);
    puts("");
    cvec_ip_destroy(&pairs1);

    // ------------------

    clist_ip pairs2 = clist_init;
    c_push(&pairs2, clist_ip, c_items(
        {1, 2},
        {3, 4},
        {5, 6},
        {7, 8},
    ));

    c_foreach (i, clist_ip, pairs2)
        printf("(%d %d) ", i.item->value.x, i.item->value.y);
    puts("");
    clist_ip_destroy(&pairs2);
}