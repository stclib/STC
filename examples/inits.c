#include <stdio.h>
#include <stc/cstr.h>
#include <stc/cmap.h>
#include <stc/cpqueue.h>
#include <stc/clist.h>

declare_cmap(id, int, cstr_t, cstr_destroy); // Map of int -> cstr_t
declare_cmap_str(cnt, int);

typedef struct {int x, y;} ipair_t;
declare_cvec(ip, ipair_t, c_default_destroy, c_mem_compare);
declare_clist(ip, ipair_t, c_default_destroy, c_mem_compare);

declare_cvec(f, float);
declare_cvec_pqueue(f, >);


int main(void) {
    
    // CVEC FLOAT / PRIORITY QUEUE

    cvec_f floats = cvec_init;
    c_push(&floats, cvec_f, c_items(4.0f, 2.0f, 5.0f, 3.0f, 1.0f));

    c_foreach (i, cvec_f, floats) printf("%.1f ", *i.item);
    puts("");

    // CVEC PRIORITY QUEUE

    cvec_f_pqueue_build(&floats); // reorganise vec
    c_push(&floats, cvec_f_pqueue, c_items(40.0f, 20.0f, 50.0f, 30.0f, 10.0f));

    // sorted:
    while (cvec_size(floats) > 0) {
        printf("%.1f ", cvec_f_pqueue_top(&floats));
        cvec_f_pqueue_pop(&floats);
    }
    puts("\n");
    cvec_f_destroy(&floats);

    // CMAP ID

    int year = 2020;
    cmap_id idnames = cmap_init;
    c_push(&idnames, cmap_id, c_items(
        {100, cstr_make("Hello")},
        {110, cstr_make("World")},
        {120, cstr_from("Howdy, -%d-", year)},
    ));

    c_foreach (i, cmap_id, idnames)
        printf("%d: %s\n", i.item->key, i.item->value.str);
    puts("");
    cmap_id_destroy(&idnames);

    // CMAP CNT

    cmap_cnt countries = cmap_init;

    cmap_cnt_insert(&countries, "Greenland", 0)->value += 20;
    c_push(&countries, cmap_cnt, c_items(
        {"Norway", 100},
        {"Denmark", 50},
        {"Iceland", 10},
        {"Belgium", 10},
        {"Italy", 10},
        {"Germany", 10},
        {"Spain", 10},
        {"France", 10},
    ));

    cmap_cnt_insert(&countries, "Sweden", 0)->value += 20;
    cmap_cnt_insert(&countries, "Norway", 0)->value += 20;
    cmap_cnt_insert(&countries, "Finland", 0)->value += 20;

    c_foreach (i, cmap_cnt, countries)
        printf("%s: %d\n", i.item->key.str, i.item->value);
    puts("");
    cmap_cnt_destroy(&countries);

    // CVEC PAIR

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

    // CLIST PAIR

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