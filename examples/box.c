/* cbox: heap allocated boxed type */
#include <stc/cstr.h>

typedef struct { cstr name, last; } Person;

Person Person_new(const char* name, const char* last) {
    return (Person){.name = cstr_from(name), .last = cstr_from(last)};
}

int Person_cmp(const Person* a, const Person* b) {
    int c = strcmp(a->name.str, b->name.str);
    return c ? c : strcmp(a->last.str, b->last.str);
}

Person Person_clone(Person p) {
    p.name = cstr_clone(p.name);
    p.last = cstr_clone(p.last);
    return p;
}

void Person_drop(Person* p) {
    printf("drop: %s %s\n", p->name.str, p->last.str);
    c_drop(cstr, &p->name, &p->last);
}

#define i_type PPtr
#define i_val_bind Person // binds Person_cmp, ...
#include <stc/cbox.h>

#define i_type Persons
#define i_val_bind PPtr // binds PPtr_cmp, ...
#include <stc/cvec.h>


int main()
{
    c_auto (Persons, vec)
    c_auto (PPtr, p, q)
    {
        p = PPtr_new(Person_new("Dave", "Cooper"));

        q = PPtr_clone(p);
        cstr_assign(&q.get->name, "Dale");

        printf("%s %s.\n", p.get->name.str, p.get->last.str);
        printf("%s %s.\n", q.get->name.str, q.get->last.str);

        Persons_push_back(&vec, PPtr_new(Person_new("Laura", "Palmer")));
        Persons_push_back(&vec, PPtr_new(Person_new("Shelly", "Johnson")));

        c_foreach (i, Persons, vec)
            printf("%s: %s\n", i.ref->get->name.str, i.ref->get->last.str);
        
        // Look-up Shelly!
        c_autovar (Person s = Person_new("Shelly", "Johnson"), Person_drop(&s)) {
            const PPtr *v = Persons_get(&vec, (PPtr){&s});
            if (v) printf("found: %s: %s\n", v->get->name.str, v->get->last.str);
        }
    }
}
