#define i_implement
#include "stc/cstr.h"

typedef struct { cstr name, last; } Person;
Person Person_make(const char* name, const char* last);
Person Person_clone(Person p);
void Person_drop(Person* p);
int Person_cmp(const Person* a, const Person* b);
uint64_t Person_hash(const Person* p);

#define i_type PersonArc
#define i_keyclass Person // "class" assume _clone, _drop, _cmp, _hash is defined.
#define i_use_cmp
#include "stc/arc.h"

#define i_TYPE IPtr,int
#define i_keydrop(x) printf("drop: %d\n", *x)
#define i_use_cmp
#include "stc/arc.h"

#define i_type IPStack
#define i_key_arc IPtr
#include "stc/stack.h"

#define i_type PASet
#define i_key_arc PersonArc
#include "stc/hset.h"


Person Person_make(const char* name, const char* last) {
    Person p = {.name = cstr_from(name), .last = cstr_from(last)};
    return p;
}

int Person_cmp(const Person* a, const Person* b) {
    return cstr_cmp(&a->name, &b->name);
}

uint64_t Person_hash(const Person* p) {
    return cstr_hash(&p->name);
}

Person Person_clone(Person p) {
    p.name = cstr_clone(p.name), p.last = cstr_clone(p.last);
    return p;
}

void Person_drop(Person* p) {
    printf("drop: %s %s\n", cstr_str(&p->name), cstr_str(&p->last));
    c_drop(cstr, &p->name, &p->last);
}


int main(void) {
    puts("Ex1");
    PersonArc p = PersonArc_from(Person_make("John", "Smiths"));
    PersonArc q = PersonArc_clone(p); // share
    PersonArc r = PersonArc_clone(p);
    PersonArc s = PersonArc_from(Person_clone(*p.get)); // deep copy
    printf("%s %s: refs %ld\n", cstr_str(&p.get->name), cstr_str(&p.get->last), *p.use_count);
    c_drop(PersonArc, &p, &q, &r, &s);

    puts("Ex2");
    IPStack vec = {0};
    IPStack_push(&vec, IPtr_from(10));
    IPStack_push(&vec, IPtr_from(20));
    IPStack_emplace(&vec, 30); // same as IPStack_push(&vec, IPtr_from(30));
    IPStack_push(&vec, IPtr_clone(*IPStack_back(&vec)));
    IPStack_push(&vec, IPtr_clone(*IPStack_front(&vec)));

    c_foreach (i, IPStack, vec)
        printf(" (%d: refs %ld)", *i.ref->get, *i.ref->use_count);
    puts("");
    IPStack_drop(&vec);
}
