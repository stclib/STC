#include <stc/csptr.h>
#include <stc/clist.h>
#include <stc/cvec.h>
#include <stc/cstr.h>
#include <stdio.h>

typedef struct { cstr_t name, last; } Person;

Person* Person_make(Person* p, const char* name, const char* last) {
    p->name = cstr(name), p->last = cstr(last);
    return p;
}
void Person_del(Person* p) {
    printf("del: %s\n", p->name.str);
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

    puts("Push 10:");
    c_forrange (i, 10) {
        csptr_pe p = csptr_pe_make(c_new(Person));
        p.get->name = cstr_from("Name %d", (i * 7) % 10);
        p.get->last = cstr_from("Last %d", (i * 9) % 10);
        clist_pe_push_back(&queue, p);
        cvec_pe_push_back(&vec, csptr_pe_share(p)); // Don't forget to share!
    }
    c_foreach (i, clist_pe, queue)
        printf(" %s\n", i.val->get->name.str);

    puts("Sort and pop 3:");
    clist_pe_sort(&queue);
    cvec_pe_sort(&vec);
    c_forrange (3) {
        clist_pe_pop_front(&queue);
        cvec_pe_pop_back(&vec);
    }

    puts("Sorted queue:");
    c_foreach (i, clist_pe, queue)
        printf(" %s\n", i.val->get->name.str);
    puts("Sorted vec:");
    c_foreach (i, cvec_pe, vec)
        printf(" %s\n", i.val->get->name.str);

    puts("Destroy queue:");
    clist_pe_del(&queue);

    puts("Destroy vec:");
    cvec_pe_del(&vec);
}