#include <stc/cstr.h>

#include <stc/forward.h>
forward_csptr(person, struct Person);

struct Person { cstr name, last; } typedef Person;

Person Person_init(const char* name, const char* last) {
    return (Person){.name = cstr_from(name), .last = cstr_from(last)};
}
void Person_del(Person* p) {
    printf("del: %s %s\n", p->name.str, p->last.str);
    c_del(cstr, &p->name, &p->last);
}

#define F_tag person
#define i_val Person
#define i_valdel Person_del
#define i_cmp c_no_compare
#include <stc/csptr.h>

#define i_val int
#include <stc/csptr.h>

#define i_tag iptr
#define i_key_csptr int
#include <stc/csset.h>

int main(void) {
    c_autovar (csptr_person p = csptr_person_make(Person_init("John", "Smiths")), csptr_person_del(&p))
    c_autovar (csptr_person q = csptr_person_clone(p), csptr_person_del(&q)) // share the pointer
    {
        printf("%s %s. uses: %zu\n", q.get->name.str, q.get->last.str, *q.use_count);
    }

    c_auto (csset_iptr, map) {
        csset_iptr_insert(&map, csptr_int_make(2021));
        csset_iptr_insert(&map, csptr_int_make(2033));

        c_foreach (i, csset_iptr, map)
            printf(" %d", *i.ref->get);
        puts("");
    }
}