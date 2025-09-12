#include <stc/cstr.h>

#define T IdMap, int, cstr, (c_valpro)   // Map of int => cstr
#include <stc/hashmap.h>

#define T NationMap, cstr, int, (c_keypro)  // Map of cstr => int
#include <stc/hashmap.h>

typedef struct {int x, y;} IPair;
inline static int IPair_cmp(const IPair* a, const IPair* b) {
    int c = c_default_cmp(&a->x, &b->x);
    return c ? c : c_default_cmp(&a->y, &b->y);
}

#define T vec_ip, IPair, (c_cmpclass)
#include <stc/vec.h>

#define T list_ip, IPair, (c_cmpclass)
#include <stc/list.h>

#define T PriorityQ, float
#include <stc/pqueue.h>

int main(void)
{
    // VEC FLOAT / PRIORITY QUEUE

    PriorityQ floats = {0};
    const float nums[] = {4.0f, 2.0f, 5.0f, 3.0f, 1.0f};

    // PRIORITY QUEUE
    for (c_range(i, c_countof(nums)))
        PriorityQ_push(&floats, nums[i]);

    puts("\npop and show high priorites first:");
    while (! PriorityQ_is_empty(&floats)) {
        printf("%.1f ", (double)*PriorityQ_top(&floats));
        PriorityQ_pop(&floats);
    }
    puts("\n");
    PriorityQ_drop(&floats);

    // CMAP ID

    int year = 2020;
    IdMap idnames = {0};
    IdMap_emplace(&idnames, 100, "Hello");
    IdMap_insert(&idnames, 110, cstr_lit("World"));
    IdMap_insert(&idnames, 120, cstr_from_fmt("Howdy, -%d-", year));

    for (c_each_kv(k, v, IdMap, idnames))
        printf("%d: %s\n", *k, cstr_str(v));
    puts("");
    IdMap_drop(&idnames);

    // CMAP CNT

    NationMap countries = c_make(NationMap, {
        {"Norway", 100},
        {"Denmark", 50},
        {"Iceland", 10},
        {"Belgium", 10},
        {"Italy", 10},
        {"Germany", 10},
        {"Spain", 10},
        {"France", 10},
    });
    NationMap_emplace(&countries, "Greenland", 0).ref->second += 20;
    NationMap_emplace(&countries, "Sweden", 0).ref->second += 20;
    NationMap_emplace(&countries, "Norway", 0).ref->second += 20;
    NationMap_emplace(&countries, "Finland", 0).ref->second += 20;

    for (c_each_kv(country, health, NationMap, countries))
        printf("%s: %d\n", cstr_str(country), *health);
    puts("");
    NationMap_drop(&countries);

    // CVEC PAIR

    vec_ip pairs1 = c_make(vec_ip, {{5, 6}, {3, 4}, {1, 2}, {7, 8}});
    vec_ip_sort(&pairs1);

    for (c_each_item(e, vec_ip, pairs1))
        printf("(%d %d) ", e->x, e->y);
    puts("");
    vec_ip_drop(&pairs1);

    // CLIST PAIR

    list_ip pairs2 = c_make(list_ip, {{5, 6}, {3, 4}, {1, 2}, {7, 8}});
    list_ip_sort(&pairs2);

    for (c_each_item(e, list_ip, pairs2))
        printf("(%d %d) ", e->x, e->y);
    puts("");
    list_ip_drop(&pairs2);
}
