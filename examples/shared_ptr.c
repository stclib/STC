#include <stc/cqueue.h>
#include <stc/cstr.h>
#include <stc/csptr.h>
#include <stdio.h>

typedef struct { cstr_t name, last; } Person;

void Person_del(Person* p) {
    printf("del %s\n", p->name.str);
    cstr_del(&p->name); cstr_del(&p->last);
}

//using_clist(p, Person, Person_del);
//using_cqueue(p, clist_p);

using_csptr(p, Person, Person_del);
using_clist(pp, csptr_p, csptr_p_del);
using_cqueue(pp, clist_pp);

int main() {
    cqueue_pp queue = cqueue_pp_init();

    // push() and pop() a few.
    c_forrange (i, 20) {
        csptr_p p = csptr_p_make(c_new(Person));
        p.get->name = cstr_from("Name %d", i);
        p.get->last = cstr_from("Last %d", i);
        cqueue_pp_push(&queue, p);
    }

    c_forrange (5)
        cqueue_pp_pop(&queue);

    c_foreach (i, cqueue_pp, queue)
        printf(" %s %s\n", i.get->get->name.str, i.get->get->last.str);

    cqueue_pp_del(&queue);
}