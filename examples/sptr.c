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

#define i_type PSPtr
#define i_val_bind Person // binds Person_cmp, ...
#include <stc/csptr.h>

#define i_type Persons
#define i_val_bind PSPtr // binds PSPtr_cmp, ...
#include <stc/cvec.h>


int main()
{
    c_auto (Persons, vec)
    c_auto (PSPtr, p, q)
    {
        p = PSPtr_new(Person_new("Laura", "Palmer"));

        // We want a deep copy -- PSPtr_clone(p) only shares!
        q = PSPtr_new(Person_clone(*p.get));
        cstr_assign(&q.get->name, "Leland");

        printf("orig: %s %s\n", p.get->name.str, p.get->last.str);
        printf("copy: %s %s\n", q.get->name.str, q.get->last.str);

        Persons_push_back(&vec, PSPtr_new(Person_new("Dale", "Cooper")));
        Persons_push_back(&vec, PSPtr_new(Person_new("Audrey", "Home")));
        
        // NB! Clone/share p and q to the vector using emplace_back()
        c_apply(Persons, emplace_back, &vec, {p, q});

        c_foreach (i, Persons, vec)
            printf("%s %s\n", i.ref->get->name.str, i.ref->get->last.str);
        puts("");
        
        // Look-up Audrey! Use a (fake) temporary PSPtr for lookup.
        c_autovar (Person a = Person_new("Audrey", "Home"), Person_drop(&a)) {
            const PSPtr *v = Persons_get(&vec, (PSPtr){&a});
            if (v) printf("found: %s %s\n", v->get->name.str, v->get->last.str);
        }
        puts("");
    }
}
