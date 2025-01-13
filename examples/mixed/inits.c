#include "stc/cstr.h"

#define i_type hmap_id   // Map of int => cstr
#define i_key int
#define i_valpro cstr
#include "stc/hmap.h"

#define i_type hmap_nat  // Map of cstr => int
#define i_keypro cstr
#define i_val int
#include "stc/hmap.h"

typedef struct {int x, y;} ipair_t;
inline static int ipair_cmp(const ipair_t* a, const ipair_t* b) {
    int c = c_default_cmp(&a->x, &b->x);
    return c ? c : c_default_cmp(&a->y, &b->y);
}

#define i_type vec_ip, ipair_t
#define i_cmp ipair_cmp
#include "stc/vec.h"

#define i_type list_ip, ipair_t
#define i_cmp ipair_cmp
#include "stc/list.h"

#define i_type pqueue_flt,float
#include "stc/pqueue.h"

int main(void)
{
    // VEC FLOAT / PRIORITY QUEUE

    pqueue_flt floats = {0};
    const float nums[] = {4.0f, 2.0f, 5.0f, 3.0f, 1.0f};

    // PRIORITY QUEUE
    for (c_range(i, c_arraylen(nums)))
        pqueue_flt_push(&floats, nums[i]);

    puts("\npop and show high priorites first:");
    while (! pqueue_flt_is_empty(&floats)) {
        printf("%.1f ", (double)*pqueue_flt_top(&floats));
        pqueue_flt_pop(&floats);
    }
    puts("\n");
    pqueue_flt_drop(&floats);

    // CMAP ID

    int year = 2020;
    hmap_id idnames = {0};
    hmap_id_emplace(&idnames, 100, "Hello");
    hmap_id_insert(&idnames, 110, cstr_lit("World"));
    hmap_id_insert(&idnames, 120, cstr_from_fmt("Howdy, -%d-", year));

    for (c_each(i, hmap_id, idnames))
        printf("%d: %s\n", i.ref->first, cstr_str(&i.ref->second));
    puts("");
    hmap_id_drop(&idnames);

    // CMAP CNT

    hmap_nat countries = c_make(hmap_nat, {
        {"Norway", 100},
        {"Denmark", 50},
        {"Iceland", 10},
        {"Belgium", 10},
        {"Italy", 10},
        {"Germany", 10},
        {"Spain", 10},
        {"France", 10},
    });
    hmap_nat_emplace(&countries, "Greenland", 0).ref->second += 20;
    hmap_nat_emplace(&countries, "Sweden", 0).ref->second += 20;
    hmap_nat_emplace(&countries, "Norway", 0).ref->second += 20;
    hmap_nat_emplace(&countries, "Finland", 0).ref->second += 20;

    for (c_each_kv(country, health, hmap_nat, countries))
        printf("%s: %d\n", cstr_str(country), *health);
    puts("");
    hmap_nat_drop(&countries);

    // CVEC PAIR

    vec_ip pairs1 = c_make(vec_ip, {{5, 6}, {3, 4}, {1, 2}, {7, 8}});
    vec_ip_sort(&pairs1);

    for (c_each(i, vec_ip, pairs1))
        printf("(%d %d) ", i.ref->x, i.ref->y);
    puts("");
    vec_ip_drop(&pairs1);

    // CLIST PAIR

    list_ip pairs2 = c_make(list_ip, {{5, 6}, {3, 4}, {1, 2}, {7, 8}});
    list_ip_sort(&pairs2);

    for (c_each(i, list_ip, pairs2))
        printf("(%d %d) ", i.ref->x, i.ref->y);
    puts("");
    list_ip_drop(&pairs2);
}
