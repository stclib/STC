#include <stc/cstr.h>

typedef struct { cstr name, last; } Person;
Person Person_make(const char* name, const char* last);
Person Person_clone(Person p);
void Person_drop(Person* p);
int Person_cmp(const Person* a, const Person* b);
uint64_t Person_hash(const Person* p);

#define i_type PersonArc
#define i_val_class Person // "class" ensure Person_drop will be called
#define i_cmp Person_cmp   // enable carc object comparisons (not ptr to obj)
#define i_hash Person_hash // enable carc object hash (not ptr to obj)
#include <stc/carc.h>

#define i_type IPtr
#define i_val int
#define i_valdrop(x) printf("drop: %d\n", *x)
#include <stc/carc.h>

#define i_type IPStack
#define i_val_arcbox IPtr
#include <stc/cstack.h>

#define i_type PASet
#define i_val_arcbox PersonArc
#include <stc/cset.h>


Person Person_make(const char* name, const char* last) {
    return (Person){.name = cstr_from(name), .last = cstr_from(last)};
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
    c_auto (PersonArc, p, q, r, s)
    {
        puts("Ex1");
        p = PersonArc_from(Person_make("John", "Smiths"));
        q = PersonArc_clone(p); // share
        r = PersonArc_clone(p);
        s = PersonArc_from(Person_clone(*p.get)); // deep copy
        printf("%s %s: refs %ld\n", cstr_str(&p.get->name), cstr_str(&p.get->last), *p.use_count);
    }
    c_auto (IPStack, vec)
    {
        puts("Ex2");
        IPStack_push(&vec, IPtr_from(10));
        IPStack_push(&vec, IPtr_from(20));
        IPStack_emplace(&vec, 30); // same as IPStack_push(&vec, IPtr_from(30));
        IPStack_push(&vec, IPtr_clone(*IPStack_back(&vec)));
        IPStack_push(&vec, IPtr_clone(*IPStack_front(&vec)));

        c_foreach (i, IPStack, vec)
            printf(" (%d: refs %ld)", *i.ref->get, *i.ref->use_count);
        puts("");
    }
}
