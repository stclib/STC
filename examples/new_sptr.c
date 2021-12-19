#include <stc/cstr.h>

struct Person { cstr name, last; } typedef Person;

Person Person_new(const char* name, const char* last) {
    return (Person){.name = cstr_from(name), .last = cstr_from(last)};
}
void Person_drop(Person* p) {
    printf("drop: %s %s\n", p->name.str, p->last.str);
    c_drop(cstr, &p->name, &p->last);
}

#define i_val Person
#define i_drop Person_drop
#define i_opt c_no_cmp
#define i_tag person
#include <stc/csptr.h>

// ...

#define i_val int
#define i_drop(x) printf("drop: %d\n", *(x))
#include <stc/csptr.h>

#define i_val_bind csptr_int
#define i_tag iptr
#include <stc/cstack.h>

int main(void) {
    c_autovar (csptr_person p = csptr_person_new(Person_new("John", "Smiths")), csptr_person_drop(&p))
    c_autovar (csptr_person q = csptr_person_clone(p), csptr_person_drop(&q)) // share the pointer
    {
        printf("%s %s. uses: %lu\n", q.get->name.str, q.get->last.str, *q.use_count);
    }

    c_auto (cstack_iptr, stk) {
        cstack_iptr_push(&stk, csptr_int_new(10));
        cstack_iptr_push(&stk, csptr_int_new(20));
        cstack_iptr_push(&stk, csptr_int_new(30));
        cstack_iptr_emplace(&stk, *cstack_iptr_top(&stk));
        cstack_iptr_emplace(&stk, *cstack_iptr_begin(&stk).ref);

        c_foreach (i, cstack_iptr, stk)
            printf(" %d", *i.ref->get);
        puts("");
    }
}