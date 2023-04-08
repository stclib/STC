#include <stc/cstr.h>

#define i_key int
#define i_val_str
#define i_tag id   // Map of int => cstr
#include <stc/cmap.h>

#define i_key_str
#define i_val int
#define i_tag cnt  // Map of cstr => int
#include <stc/cmap.h>

typedef struct {int x, y;} ipair_t;
inline static int ipair_cmp(const ipair_t* a, const ipair_t* b) {
    int c = c_default_cmp(&a->x, &b->x);
    return c ? c : c_default_cmp(&a->y, &b->y);
}


#define i_val ipair_t
#define i_cmp ipair_cmp
#define i_tag ip
#include <stc/cvec.h>

#define i_val ipair_t
#define i_cmp ipair_cmp
#define i_tag ip
#define i_extern // define _clist_mergesort() once
#include <stc/clist.h>

#define i_val float
#define i_tag f
#include <stc/cpque.h>

int main(void)
{
    // CVEC FLOAT / PRIORITY QUEUE

    cpque_f floats = {0};
    const float nums[] = {4.0f, 2.0f, 5.0f, 3.0f, 1.0f};

    // PRIORITY QUEUE
    c_forrange (i, c_arraylen(nums))
        cpque_f_push(&floats, nums[i]);

    puts("\npop and show high priorites first:");
    while (! cpque_f_empty(&floats)) {
        printf("%.1f ", *cpque_f_top(&floats));
        cpque_f_pop(&floats);
    }
    puts("\n");
    cpque_f_drop(&floats);

    // CMAP ID

    int year = 2020;
    cmap_id idnames = {0};
    cmap_id_emplace(&idnames, 100, "Hello");
    cmap_id_insert(&idnames, 110, cstr_lit("World"));
    cmap_id_insert(&idnames, 120, cstr_from_fmt("Howdy, -%d-", year));

    c_foreach (i, cmap_id, idnames)
        printf("%d: %s\n", i.ref->first, cstr_str(&i.ref->second));
    puts("");
    cmap_id_drop(&idnames);

    // CMAP CNT

    cmap_cnt countries = c_make(cmap_cnt, {
        {"Norway", 100},
        {"Denmark", 50},
        {"Iceland", 10},
        {"Belgium", 10},
        {"Italy", 10},
        {"Germany", 10},
        {"Spain", 10},
        {"France", 10},
    });
    cmap_cnt_emplace(&countries, "Greenland", 0).ref->second += 20;
    cmap_cnt_emplace(&countries, "Sweden", 0).ref->second += 20;
    cmap_cnt_emplace(&countries, "Norway", 0).ref->second += 20;
    cmap_cnt_emplace(&countries, "Finland", 0).ref->second += 20;

    c_forpair (country, health, cmap_cnt, countries)
        printf("%s: %d\n", cstr_str(_.country), *_.health);
    puts("");
    cmap_cnt_drop(&countries);

    // CVEC PAIR

    cvec_ip pairs1 = c_make(cvec_ip, {{5, 6}, {3, 4}, {1, 2}, {7, 8}});
    cvec_ip_sort(&pairs1);

    c_foreach (i, cvec_ip, pairs1)
        printf("(%d %d) ", i.ref->x, i.ref->y);
    puts("");
    cvec_ip_drop(&pairs1);

    // CLIST PAIR

    clist_ip pairs2 = c_make(clist_ip, {{5, 6}, {3, 4}, {1, 2}, {7, 8}});
    clist_ip_sort(&pairs2);

    c_foreach (i, clist_ip, pairs2)
        printf("(%d %d) ", i.ref->x, i.ref->y);
    puts("");
    clist_ip_drop(&pairs2);
}
