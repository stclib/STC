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
#include <stc/carc.h>

#define i_type Persons
#define i_val_sptr PSPtr // binds PSPtr_cmp, ...
#include <stc/cvec.h>


int main()
{
    c_auto (Persons, vec)
    c_auto (PSPtr, p, q)
    {
        p = PSPtr_from(Person_new("Laura", "Palmer"));

        // We want a deep copy -- PSPtr_clone(p) only shares!
        q = PSPtr_from(Person_clone(*p.get));
        cstr_assign(&q.get->name, "Leland");

        printf("orig: %s %s\n", p.get->name.str, p.get->last.str);
        printf("copy: %s %s\n", q.get->name.str, q.get->last.str);

        Persons_push_back(&vec, PSPtr_from(Person_new("Dale", "Cooper")));
        Persons_push_back(&vec, PSPtr_from(Person_new("Audrey", "Home")));

        // Clone/share p and q to the vector
        c_apply(v, Persons_push_back(&vec, PSPtr_clone(v)), PSPtr, {p, q});

        c_foreach (i, Persons, vec)
            printf("%s %s\n", i.ref->get->name.str, i.ref->get->last.str);
        puts("");

        // Look-up Audrey!
        c_autovar (Person a = Person_new("Audrey", "Home"), Person_drop(&a)) {
            const PSPtr *v = Persons_get(&vec, a);
            if (v) printf("found: %s %s\n", v->get->name.str, v->get->last.str);
        }

        puts("");
    }
}
