#define i_implement
#include "stc/cstr.h"

#define i_type hmap_id   // Map of int => cstr
#define i_key int
#define i_val_cstr
#include "stc/hmap.h"

#define i_type hmap_nat  // Map of cstr => int
#define i_key_cstr
#define i_val int
#include "stc/hmap.h"

typedef struct {int x, y;} ipair_t;
inline static int ipair_cmp(const ipair_t* a, const ipair_t* b) {
    int c = c_default_cmp(&a->x, &b->x);
    return c ? c : c_default_cmp(&a->y, &b->y);
}

#define i_TYPE vec_ip, ipair_t
#define i_cmp ipair_cmp
#include "stc/vec.h"

#define i_TYPE list_ip, ipair_t
#define i_cmp ipair_cmp
#include "stc/list.h"

#define i_TYPE pque_flt,float
#include "stc/pque.h"

int main(void)
{
    // VEC FLOAT / PRIORITY QUEUE

    pque_flt floats = {0};
    const float nums[] = {4.0f, 2.0f, 5.0f, 3.0f, 1.0f};

    // PRIORITY QUEUE
    c_forrange (i, c_arraylen(nums))
        pque_flt_push(&floats, nums[i]);

    puts("\npop and show high priorites first:");
    while (! pque_flt_is_empty(&floats)) {
        printf("%.1f ", (double)*pque_flt_top(&floats));
        pque_flt_pop(&floats);
    }
    puts("\n");
    pque_flt_drop(&floats);

    // CMAP ID

    int year = 2020;
    hmap_id idnames = {0};
    hmap_id_emplace(&idnames, 100, "Hello");
    hmap_id_insert(&idnames, 110, cstr_lit("World"));
    hmap_id_insert(&idnames, 120, cstr_from_fmt("Howdy, -%d-", year));

    c_foreach (i, hmap_id, idnames)
        printf("%d: %s\n", i.ref->first, cstr_str(&i.ref->second));
    puts("");
    hmap_id_drop(&idnames);

    // CMAP CNT

    hmap_nat countries = c_init(hmap_nat, {
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

    c_forpair (country, health, hmap_nat, countries)
        printf("%s: %d\n", cstr_str(_.country), *_.health);
    puts("");
    hmap_nat_drop(&countries);

    // CVEC PAIR

    vec_ip pairs1 = c_init(vec_ip, {{5, 6}, {3, 4}, {1, 2}, {7, 8}});
    vec_ip_sort(&pairs1);

    c_foreach (i, vec_ip, pairs1)
        printf("(%d %d) ", i.ref->x, i.ref->y);
    puts("");
    vec_ip_drop(&pairs1);

    // CLIST PAIR

    list_ip pairs2 = c_init(list_ip, {{5, 6}, {3, 4}, {1, 2}, {7, 8}});
    list_ip_sort(&pairs2);

    c_foreach (i, list_ip, pairs2)
        printf("(%d %d) ", i.ref->x, i.ref->y);
    puts("");
    list_ip_drop(&pairs2);
}
