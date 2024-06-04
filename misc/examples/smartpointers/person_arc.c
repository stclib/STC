/* box: heap allocated boxed type */
#define i_implement
#include "stc/cstr.h"

typedef struct { cstr name, last; } Person;

Person Person_make(const char* name, const char* last) {
    Person p = {.name = cstr_from(name), .last = cstr_from(last)};
    return p;
}

int Person_cmp(const Person* a, const Person* b) {
    int c = cstr_cmp(&a->name, &b->name);
    return c ? c : cstr_cmp(&a->last, &b->last);
}

uint64_t Person_hash(const Person* a) {
    return cstr_hash(&a->name) ^ cstr_hash(&a->last);
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

#define i_type PSPtr
#define i_keyclass Person // ensure Person_drop
#define i_use_cmp
#include "stc/arc.h"

#define i_type Persons
#define i_key_arc PSPtr // binds PSPtr_cmp, PSPtr_drop...
#define i_use_cmp
#include "stc/vec.h"


int main(void)
{
    PSPtr p = PSPtr_from(Person_make("Laura", "Palmer"));
    PSPtr q = PSPtr_from(Person_clone(*p.get)); // deep copy
    Persons vec = {0};

    c_defer(
        PSPtr_drop(&p),
        PSPtr_drop(&q),
        Persons_drop(&vec)
    ){
        cstr_assign(&q.get->name, "Leland");

        printf("orig: %s %s\n", cstr_str(&p.get->name), cstr_str(&p.get->last));
        printf("copy: %s %s\n", cstr_str(&q.get->name), cstr_str(&q.get->last));

        // Use Persons_emplace to implicitly call PSPtr_make on the argument.
        // No need to do: Persons_push(&vec, PSPtr_make(Person_make("Audrey", "Home")));
        Persons_emplace(&vec, Person_make("Audrey", "Home"));
        Persons_emplace(&vec, Person_make("Dale", "Cooper"));

        // Clone/share p and q to the vector
        c_foritems (i, PSPtr, {p, q})
            Persons_push(&vec, PSPtr_clone(*i.ref));

        c_foreach (i, Persons, vec)
            printf("%s %s\n", cstr_str(&i.ref->get->name), cstr_str(&i.ref->get->last));
        puts("");

        // Look-up Audrey!
        Person a = Person_make("Audrey", "Home");
        const PSPtr *v = Persons_get(&vec, a);
        if (v) printf("found: %s %s\n", cstr_str(&v->get->name), cstr_str(&v->get->last));
        Person_drop(&a);
    }
}
