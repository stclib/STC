/* cbox: heap allocated boxed type */
#include <stc/cstr.h>

typedef struct { cstr name, last; } Person;

Person Person_new(const char* name, const char* last) {
    return (Person){.name = cstr_from(name), .last = cstr_from(last)};
}

int Person_cmp(const Person* a, const Person* b) {
    int c = cstr_cmp(&a->name, &b->name);
    return c ? c : cstr_cmp(&a->last, &b->last);
}

Person Person_clone(Person p) {
    p.name = cstr_clone(p.name);
    p.last = cstr_clone(p.last);
    return p;
}

void Person_drop(Person* p) {
    printf("drop: %s %s\n", cstr_str(&p->name), cstr_str(&p->last));
    c_drop(cstr, &p->name, &p->last);
}

#define i_type PBox
#define i_val_bind Person // binds Person_cmp, ...
#include <stc/cbox.h>

#define i_type Persons
#define i_val_arcbox PBox // informs that PBox is a smart pointer.
#include <stc/cvec.h>

int main()
{
    c_auto (Persons, vec)
    c_auto (PBox, p, q)
    {
        p = PBox_from(Person_new("Laura", "Palmer"));

        q = PBox_clone(p);
        cstr_assign(&q.get->name, "Leland");

        printf("orig: %s %s\n", cstr_str(&p.get->name), cstr_str(&p.get->last));
        printf("copy: %s %s\n", cstr_str(&q.get->name), cstr_str(&q.get->last));

        Persons_push_back(&vec, PBox_from(Person_new("Dale", "Cooper")));
        Persons_push_back(&vec, PBox_from(Person_new("Audrey", "Home")));
        
        // NB! Clone p and q to the vector using emplace_back()
        c_apply(v, Persons_push_back(&vec, PBox_clone(v)), PBox, {p, q});

        c_foreach (i, Persons, vec)
            printf("%s %s\n", cstr_str(&i.ref->get->name), cstr_str(&i.ref->get->last));
        puts("");
        
        // Look-up Audrey! Use a (fake) temporary PBox for lookup.
        c_autovar (Person a = Person_new("Audrey", "Home"), Person_drop(&a)) {
            const PBox *v = Persons_get(&vec, a);
            if (v) printf("found: %s %s\n", cstr_str(&v->get->name), cstr_str(&v->get->last));
        }
        puts("");

        // Alternative to use cbox (when not placed in container).
        Person *she = c_new(Person, Person_new("Shelly", "Johnson"));
        printf("%s %s\n", cstr_str(&she->name), cstr_str(&she->last));
        c_delete(Person, she); // drop and free
        puts("");
    }
}
