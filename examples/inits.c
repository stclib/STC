#include <stdio.h>
#include <stc/cstr.h>
#include <stc/cmap.h>
#include <stc/cvec.h>
#include <stc/cpque.h>
#include <stc/clist.h>

using_cmap_strval(id, int); // Map of int => cstr
using_cmap_strkey(cnt, int); // Map of cstr => int

typedef struct {int x, y;} ipair_t;
inline static int ipair_compare(const ipair_t* a, const ipair_t* b) {
    int cx = c_default_compare(&a->x, &b->x);
    return cx == 0 ? c_default_compare(&a->y, &b->y) : cx;
}

using_cvec(ip, ipair_t, ipair_compare);
using_clist(ip, ipair_t, ipair_compare);
using_cvec(f, float);
using_cpque(f, cvec_f);

int main(void)
{
    // CVEC FLOAT / PRIORITY QUEUE

    c_forvar (cvec_f floats = cvec_f_init(), cvec_f_del(&floats)) {
        c_emplace(cvec_f, floats, {4.0f, 2.0f, 5.0f, 3.0f, 1.0f});

        c_foreach (i, cvec_f, floats) printf("%.1f ", *i.ref);
        puts("");

        // CVEC PRIORITY QUEUE

        cpque_f_make_heap(&floats);
        c_emplace(cpque_f, floats, {40.0f, 20.0f, 50.0f, 30.0f, 10.0f});

        puts("\npop and show high priorites first:");
        while (! cpque_f_empty(floats)) {
            printf("%.1f ", *cpque_f_top(&floats));
            cpque_f_pop(&floats);
        }
        puts("\n");
    }

    // CMAP ID

    int year = 2020;
    c_forvar (cmap_id idnames = cmap_id_init(), cmap_id_del(&idnames)) {
        cmap_id_emplace(&idnames, 100, "Hello");
        cmap_id_insert(&idnames, 110, cstr_from("World"));
        cmap_id_insert(&idnames, 120, cstr_from_fmt("Howdy, -%d-", year));

        c_foreach (i, cmap_id, idnames)
            printf("%d: %s\n", i.ref->first, i.ref->second.str);
        puts("");
    }

    // CMAP CNT

    c_forvar (cmap_cnt countries = cmap_cnt_init(), cmap_cnt_del(&countries)) {
        c_emplace(cmap_cnt, countries, {
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

    c_forvar (cvec_ip pairs1 = cvec_ip_init(), cvec_ip_del(&pairs1)) {
        c_emplace (cvec_ip, pairs1, {{5, 6}, {3, 4}, {1, 2}, {7, 8}});
        cvec_ip_sort(&pairs1);

        c_foreach (i, cvec_ip, pairs1)
            printf("(%d %d) ", i.ref->x, i.ref->y);
        puts("");
    }

    // CLIST PAIR

    c_forvar (clist_ip pairs2 = clist_ip_init(), clist_ip_del(&pairs2)) {
        c_emplace(clist_ip, pairs2, {{5, 6}, {3, 4}, {1, 2}, {7, 8}});
        clist_ip_sort(&pairs2);

        c_foreach (i, clist_ip, pairs2)
            printf("(%d %d) ", i.ref->x, i.ref->y);
        puts("");
    }
}