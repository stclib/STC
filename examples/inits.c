#include <stdio.h>
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
inline static int ipair_compare(const ipair_t* a, const ipair_t* b) {
    int cx = c_default_compare(&a->x, &b->x);
    return cx == 0 ? c_default_compare(&a->y, &b->y) : cx;
}


#define i_val ipair_t
#define i_cmp ipair_compare
#define i_tag ip
#include <stc/cvec.h>

#define i_val ipair_t
#define i_cmp ipair_compare
#define i_tag ip
#include <stc/clist.h>

#define i_val float
#define i_tag f
#include <stc/cpque.h>

int main(void)
{
    // CVEC FLOAT / PRIORITY QUEUE

    c_auto (cpque_f, floats) {
        float nums[] = {4.0f, 2.0f, 5.0f, 3.0f, 1.0f};

        c_forrange (i, c_arraylen(nums)) {
            cpque_f_push_back(&floats, nums[i]);
            printf("%.1f ", floats.data[i]);
        }
        puts("");

        // CVEC PRIORITY QUEUE

        cpque_f_make_heap(&floats);
        c_apply(cpque_f, push, &floats, {40.0f, 20.0f, 50.0f, 30.0f, 10.0f});

        puts("\npop and show high priorites first:");
        while (! cpque_f_empty(floats)) {
            printf("%.1f ", *cpque_f_top(&floats));
            cpque_f_pop(&floats);
        }
        puts("\n");
    }

    // CMAP ID

    int year = 2020;
    c_auto (cmap_id, idnames) {
        cmap_id_emplace(&idnames, 100, "Hello");
        cmap_id_insert(&idnames, 110, cstr_from("World"));
        cmap_id_insert(&idnames, 120, cstr_from_fmt("Howdy, -%d-", year));

        c_foreach (i, cmap_id, idnames)
            printf("%d: %s\n", i.ref->first, i.ref->second.str);
        puts("");
    }

    // CMAP CNT

    c_auto (cmap_cnt, countries) {
        c_apply_pair(cmap_cnt, emplace, &countries, {
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

        c_foreach (i, cmap_cnt, countries)
            printf("%s: %d\n", i.ref->first.str, i.ref->second);
        puts("");
    }

    // CVEC PAIR

    c_auto (cvec_ip, pairs1) {
        c_apply(cvec_ip, push_back, &pairs1, {{5, 6}, {3, 4}, {1, 2}, {7, 8}});
        cvec_ip_sort(&pairs1);

        c_foreach (i, cvec_ip, pairs1)
            printf("(%d %d) ", i.ref->x, i.ref->y);
        puts("");
    }

    // CLIST PAIR

    c_auto (clist_ip, pairs2) {
        c_apply(clist_ip, push_back, &pairs2, {{5, 6}, {3, 4}, {1, 2}, {7, 8}});
        clist_ip_sort(&pairs2);

        c_foreach (i, clist_ip, pairs2)
            printf("(%d %d) ", i.ref->x, i.ref->y);
        puts("");
    }
}