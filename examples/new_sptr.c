#include <stc/cstr.h>

struct Person { cstr name, last; } typedef Person;

Person Person_new(const char* name, const char* last) {
    return (Person){.name = cstr_from(name), .last = cstr_from(last)};
}
Person Person_clone(Person p) {
    p.name = cstr_clone(p.name), p.last = cstr_clone(p.last);
    return p;
}
void Person_drop(Person* p) {
    printf("drop: %s %s\n", cstr_str(&p->name), cstr_str(&p->last));
    c_drop(cstr, &p->name, &p->last);
}

#define i_val_bind Person
#define i_opt c_no_cmp
#define i_tag person
#include <stc/carc.h>

// ...
#define i_type SPtr
#define i_val int
#define i_valdrop(x) printf("drop: %d\n", *x)
#define i_valfrom(i) i
#include <stc/carc.h>

#define i_val_arcbox SPtr
#define i_tag iptr
#include <stc/cstack.h>

int main(void) {
    c_auto (carc_person, p, q, r, s)
    {
        puts("Ex1");
        p = carc_person_from(Person_new("John", "Smiths"));
        q = carc_person_clone(p);
        r = carc_person_clone(p);
        s = carc_person_from(Person_clone(*p.get)); // deep copy
        printf("%s %s. uses: %lu\n", cstr_str(&r.get->name), cstr_str(&s.get->last), *p.use_count);
    }

    c_auto (cstack_iptr, stk) {
        puts("Ex2");
        cstack_iptr_push(&stk, SPtr_make(10));
        cstack_iptr_push(&stk, SPtr_make(20));
        cstack_iptr_push(&stk, SPtr_make(30));
        cstack_iptr_push(&stk, SPtr_clone(*cstack_iptr_top(&stk)));
        cstack_iptr_push(&stk, SPtr_clone(*cstack_iptr_begin(&stk).ref));

        c_foreach (i, cstack_iptr, stk)
            printf(" (%d, uses %ld)", *i.ref->get, *i.ref->use_count);
        puts("");
    }
}
