#include <stc/cptr.h>
#include <stc/cmap.h>
#include <stc/cstr.h>
#include <stdio.h>

typedef struct { cstr_t name, last; } Person;

Person* Person_make(Person* p, cstr_t name, cstr_t last) {
    printf("make %s\n", name.str);
    p->name = name, p->last = last;
    return p;
}
Person* Person_from(Person* p, const char* name, const char* last) {
    p->name = cstr(name), p->last = cstr(last);
    return p;
}
void Person_del(Person* p) {
    printf("del: %s\n", p->name.str);
    c_del(cstr, &p->name, &p->last);
}

using_csptr(pe, Person, Person_del);
using_cmap(pe, int, csptr_pe, csptr_pe_del);


int main() {
    cmap_pe map = cmap_pe_init();

    puts("Emplace 10:");
    // c_try_emplace: The last argument is completely ignored if key already exist in map, so no memory leak happens!
    c_forrange (i, 20) { // When i>9, all key will exist, so value arg is not executed.
        c_try_emplace(&map, cmap_pe, (i * 7) % 10,
                            csptr_pe_make(Person_make(c_new(Person), cstr_from("Name %d", (i * 7) % 10),
                                                                     cstr_from("Last %d", (i * 9) % 10))));
    }
    c_try_emplace(&map, cmap_pe, 11, csptr_pe_make(Person_from(c_new(Person), "Hello", "world!")));

    c_foreach (i, cmap_pe, map)
        printf(" %d: %s\n", i.val->first, i.val->second.get->name.str);

    puts("Destroy map:");
    cmap_pe_del(&map);
}