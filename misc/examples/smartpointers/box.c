/* box: heap allocated boxed type */
#define i_implement
#include "stc/cstr.h"

typedef struct { cstr name, last; } Person;

Person Person_make(const char* name, const char* last) {
    return c_LITERAL(Person){.name = cstr_from(name), .last = cstr_from(last)};
}

uint64_t Person_hash(const Person* a) {
    return cstr_hash(&a->name) ^ cstr_hash(&a->last);
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
#define i_keyclass Person // "class" binds _cmp, _clone, _drop functions.
#define i_use_cmp
#include "stc/box.h"

#define i_type Persons
#define i_key_box PBox    // i_key_box informs that PBox is a box type.
#include "stc/sset.h"

int main(void)
{
    Persons vec = {0};
    PBox p = PBox_from(Person_make("Laura", "Palmer"));
    PBox q = PBox_clone(p);
    cstr_assign(&q.get->name, "Leland");

    printf("orig: %s %s\n", cstr_str(&p.get->name), cstr_str(&p.get->last));
    printf("copy: %s %s\n", cstr_str(&q.get->name), cstr_str(&q.get->last));

    Persons_emplace(&vec, Person_make("Dale", "Cooper"));
    Persons_emplace(&vec, Person_make("Audrey", "Home"));

    // NB! Clone/share p and q in the Persons container.
    Persons_push(&vec, PBox_clone(p));
    Persons_push(&vec, PBox_clone(q));

    c_foreach (i, Persons, vec)
        printf("%s %s\n", cstr_str(&i.ref->get->name), cstr_str(&i.ref->get->last));
    puts("");

    // Look-up Audrey! Create a temporary Person for lookup.
    Person a = Person_make("Audrey", "Home");
    const PBox *v = Persons_get(&vec, a); // lookup
    if (v) printf("found: %s %s\n", cstr_str(&v->get->name), cstr_str(&v->get->last));

    Person_drop(&a);
    PBox_drop(&p);
    PBox_drop(&q);
    Persons_drop(&vec);
}
