#include <stdio.h>
#include <stc/cstr.h>
#include <stc/cmap.h>
#include <stc/cvec.h>
#include <stc/cpqueue.h>
#include <stc/clist.h>

using_cmap(id, int, cstr_t, cstr_del); // Map of int -> cstr_t
using_cmap_strkey(cnt, int);

typedef struct {int x, y;} ipair_t;
inline static int ipair_compare(const ipair_t* a, const ipair_t* b) {
    int cx = c_default_compare(&a->x, &b->x);
    return cx == 0 ? c_default_compare(&a->y, &b->y) : cx;
}

using_cvec(ip, ipair_t, c_default_del, ipair_compare);
using_clist(ip, ipair_t, c_default_del, ipair_compare);
using_cvec(f, float);
using_cpqueue(f, cvec_f, >);

int main(void)
{
    // CVEC FLOAT / PRIORITY QUEUE

    cvec_f floats = cvec_inits;
    c_push_items(&floats, cvec_f, {4.0f, 2.0f, 5.0f, 3.0f, 1.0f});

    c_foreach (i, cvec_f, floats) printf("%.1f ", *i.val);
    puts("");

    // CVEC PRIORITY QUEUE

    cpqueue_f_make_heap(&floats);
    c_push_items(&floats, cpqueue_f, {40.0f, 20.0f, 50.0f, 30.0f, 10.0f});

    // sorted:
    while (! cpqueue_f_empty(floats)) {
        printf("%.1f ", *cpqueue_f_top(&floats));
        cpqueue_f_pop(&floats);
    }
    puts("\n");
    cpqueue_f_del(&floats);

    // CMAP ID

    int year = 2020;
    cmap_id idnames = cmap_inits;
    c_push_items(&idnames, cmap_id, {
        {100, cstr_from("Hello")},
        {110, cstr_from("World")},
        {120, cstr_from_fmt("Howdy, -%d-", year)},
    });

    c_foreach (i, cmap_id, idnames)
        printf("%d: %s\n", i.val->first, i.val->second.str);
    puts("");
    cmap_id_del(&idnames);

    // CMAP CNT

    cmap_cnt countries = cmap_inits;
    c_push_items(&countries, cmap_cnt, {
        {"Norway", 100},
        {"Denmark", 50},
        {"Iceland", 10},
        {"Belgium", 10},
        {"Italy", 10},
        {"Germany", 10},
        {"Spain", 10},
        {"France", 10},
    });
    cmap_cnt_emplace(&countries, "Greenland", 0).first->second += 20;
    cmap_cnt_emplace(&countries, "Sweden", 0).first->second += 20;
    cmap_cnt_emplace(&countries, "Norway", 0).first->second += 20;
    cmap_cnt_emplace(&countries, "Finland", 0).first->second += 20;

    c_foreach (i, cmap_cnt, countries)
        printf("%s: %d\n", i.val->first.str, i.val->second);
    puts("");
    cmap_cnt_del(&countries);

    // CVEC PAIR

    cvec_ip pairs1 = cvec_inits;
    c_push_items(&pairs1, cvec_ip, {
        {5, 6},
        {3, 4},
        {1, 2},
        {7, 8},
    });
    cvec_ip_sort(&pairs1);

    c_foreach (i, cvec_ip, pairs1)
        printf("(%d %d) ", i.val->x, i.val->y);
    puts("");
    cvec_ip_del(&pairs1);

    // CLIST PAIR

    clist_ip pairs2 = clist_inits;
    c_push_items(&pairs2, clist_ip, {
        {5, 6},
        {3, 4},
        {1, 2},
        {7, 8},
    });
    clist_ip_sort(&pairs2);

    c_foreach (i, clist_ip, pairs2)
        printf("(%d %d) ", i.val->x, i.val->y);
    puts("");
    clist_ip_del(&pairs2);
}