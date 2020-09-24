#include <stc/cqueue.h>
#include <stc/cstr.h>
#include <stc/csptr.h>
#include <stdio.h>

typedef struct { cstr_t name, last; } Person;

Person* Person_make(Person* p, const char* name, const char* last) {
    p->name = cstr(name), p->last = cstr(last);
    return p;
}
void Person_del(Person* p) {
    printf("del %s\n", p->name.str);
    c_del(cstr, &p->name, &p->last);
}
int Person_compare(const Person* p, const Person* q) {
    int cmp = cstr_equals_s(p->name, q->name);
    return cmp == 0 ? cstr_equals_s(p->last, q->last) : cmp;
}

//using_clist(p, Person, Person_del);
//using_cqueue(p, clist_p);

using_csptr(p, Person, Person_del);
int csptr_p_compare(const csptr_p* p, const csptr_p* q) {return Person_compare(p->get, q->get);}

using_clist(pp, csptr_p, csptr_p_del, csptr_p_compare);
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
        printf(" %s %s\n", i.val->get->name.str, i.val->get->last.str);

    cqueue_pp_del(&queue);
}