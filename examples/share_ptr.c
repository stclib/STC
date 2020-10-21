#include <stc/cptr.h>
#include <stc/clist.h>
#include <stc/cvec.h>
#include <stc/cfmt.h>
#include <stdio.h>

typedef struct { cstr_t name, last; } Person;

Person* Person_make(Person* p, const char* name, const char* last) {
    p->name = cstr_from(name), p->last = cstr_from(last);
    return p;
}
void Person_del(Person* p) {
    c_printf(0, "del: {}\n", p->name.str);
    c_del(cstr, &p->name, &p->last);
}
int Person_compare(const Person* p, const Person* q) {
    int cmp = strcmp(p->name.str, q->name.str);
    return cmp == 0 ? strcmp(p->last.str, q->last.str) : cmp;
}

using_csptr(pe, Person, Person_del, Person_compare);
using_clist(pe, csptr_pe, csptr_pe_del, csptr_pe_compare);
using_cvec(pe, csptr_pe, csptr_pe_del, csptr_pe_compare);

int main() {
    clist_pe queue = clist_pe_init();
    cvec_pe vec = cvec_pe_init();

    csptr_pe joe = csptr_pe_make((Person) {cstr_from("Joe"), cstr_from("Jordan")});
    clist_pe_push_back(&queue, csptr_pe_share(joe));
    cvec_pe_push_back(&vec, csptr_pe_share(joe));

    puts("Push 10:");
    c_forrange (i, 10) {
        csptr_pe p = csptr_pe_from(c_new(Person));
        p.get->name = cstr_from_fmt("Name %d", (i * 7) % 10);
        p.get->last = cstr_from_fmt("Last %d", (i * 7) % 10);
        clist_pe_push_back(&queue, p);
        cvec_pe_push_back(&vec, csptr_pe_share(p)); // Don't forget to share!
    }
    c_foreach (i, clist_pe, queue)
        c_printf(0, " {}\n", i.val->get->name.str);

    puts("Sort and pop 3:");
    clist_pe_sort(&queue);
    cvec_pe_sort(&vec);
    c_forrange (3) {
        clist_pe_pop_front(&queue);
        cvec_pe_pop_back(&vec);
    }

    puts("Sorted queue:");
    c_foreach (i, clist_pe, queue)
        c_printf(0, " {}\n", i.val->get->name.str);
    puts("Sorted vec:");
    c_foreach (i, cvec_pe, vec)
        c_printf(0, " {}\n", i.val->get->name.str);

    Person lost; Person_make(&lost, "Name 5", "Last 5");
    csptr_pe ptmp = {&lost, NULL}; // share pointer without counter - OK.
    clist_pe_iter_t lit = clist_pe_find(&queue, ptmp);
    Person_del(&lost);
    if (lit.val) c_printf(0, "Found: {}\n", lit.val->get->name.str);

    c_printf(0, "use {}\n", *joe.use_count);
    csptr_pe_del(&joe);

    puts("Destroy queue:");
    clist_pe_del(&queue);

    puts("Destroy vec:");
    cvec_pe_del(&vec);
}