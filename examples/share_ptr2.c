#include <stc/cptr.h>
#include <stc/cmap.h>
#include <stc/cstr.h>
#include <stdio.h>

typedef struct { cstr_t name, last; } Person;

Person* Person_from(Person* p, cstr_t name, cstr_t last) {
    p->name = name, p->last = last;
    return p;
}
void Person_del(Person* p) {
    c_del(cstr, &p->name, &p->last);
}

using_csptr(ps, Person, Person_del, c_no_compare);
using_cmap(ps, int, csptr_ps, csptr_ps_del);


int main() {
    cmap_ps map = cmap_ps_init();
    c_forrange (i, 20) {
        c_try_emplace(&map, cmap_ps, (i * 7) % 10,
                            csptr_ps_from(Person_from(c_new(Person), cstr_from_fmt("Name %d", (i * 7) % 10),
                                                                     cstr_from_fmt("Last %d", (i * 9) % 10))));
    }
    c_foreach (i, cmap_ps, map)
        printf(" %d: %s\n", i.ref->first, i.ref->second.get->name.str);
    cmap_ps_del(&map);
}