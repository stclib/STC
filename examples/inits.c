#include <stdio.h>
#include <stc/cstr.h>
#include <stc/cmap.h>
#include <stc/cvec.h>
#include <stc/cpqueue.h>
#include <stc/clist.h>

declare_cmap(id, int, cstr_t, cstr_destroy); // Map of int -> cstr_t
declare_cmap_strkey(cnt, int);

typedef struct {int x, y;} ipair_t;
inline static int ipair_compare(const ipair_t* a, const ipair_t* b) {
    int c = c_default_compare(&a->x, &b->x);
    return c != 0 ? c : c_default_compare(&a->y, &b->y);
}
declare_cvec(ip, ipair_t, c_default_destroy, ipair_compare);
declare_clist(ip, ipair_t, c_default_destroy, ipair_compare);

declare_cvec(f, float);
declare_cpqueue(f, cvec_f, >);


int main(void) {
    
    // CVEC FLOAT / PRIORITY QUEUE

    c_init(cvec_f, floats, c_items(4.0f, 2.0f, 5.0f, 3.0f, 1.0f));

    c_foreach (i, cvec_f, floats) printf("%.1f ", *i.item);
    puts("");

    // CVEC PRIORITY QUEUE

    cpqueue_f_build(&floats); // reorganise vec
    c_push(&floats, cpqueue_f, c_items(40.0f, 20.0f, 50.0f, 30.0f, 10.0f));

    // sorted:
    while (cvec_size(floats) > 0) {
        printf("%.1f ", *cpqueue_f_top(&floats));
        cpqueue_f_pop(&floats);
    }
    puts("\n");
    cpqueue_f_destroy(&floats);

    // CMAP ID

    int year = 2020;
    c_init(cmap_id, idnames, c_items(
        {100, cstr_make("Hello")},
        {110, cstr_make("World")},
        {120, cstr_from("Howdy, -%d-", year)},
    ));

    c_foreach (i, cmap_id, idnames)
        printf("%d: %s\n", i.item->key, i.item->value.str);
    puts("");
    cmap_id_destroy(&idnames);

    // CMAP CNT

    c_init(cmap_cnt, countries, c_items(
        {"Norway", 100},
        {"Denmark", 50},
        {"Iceland", 10},
        {"Belgium", 10},
        {"Italy", 10},
        {"Germany", 10},
        {"Spain", 10},
        {"France", 10},
    ));
    cmap_cnt_insert(&countries, "Greenland", 0)->value += 20;
    cmap_cnt_insert(&countries, "Sweden", 0)->value += 20;
    cmap_cnt_insert(&countries, "Norway", 0)->value += 20;
    cmap_cnt_insert(&countries, "Finland", 0)->value += 20;

    c_foreach (i, cmap_cnt, countries)
        printf("%s: %d\n", i.item->key.str, i.item->value);
    puts("");
    cmap_cnt_destroy(&countries);

    // CVEC PAIR

    c_init(cvec_ip, pairs1, c_items(
        {5, 6},
        {3, 4},
        {1, 2},
        {7, 8},
    ));
    cvec_ip_sort(&pairs1);

    c_foreach (i, cvec_ip, pairs1)
        printf("(%d %d) ", i.item->x, i.item->y);
    puts("");
    cvec_ip_destroy(&pairs1);

    // CLIST PAIR

    c_init(clist_ip, pairs2, c_items(
        {5, 6},
        {3, 4},
        {1, 2},
        {7, 8},
    ));
    clist_ip_sort(&pairs2);
    
    c_foreach (i, clist_ip, pairs2)
        printf("(%d %d) ", i.item->value.x, i.item->value.y);
    puts("");
    clist_ip_destroy(&pairs2);
}