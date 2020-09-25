#include <stc/cptr.h>
#include <stc/cstr.h>
#include <stc/cvec.h>

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

using_cptr(pe, Person, Person_del, Person_compare);
using_cvec(pe, Person*, cptr_pe_del, cptr_pe_compare);

int main() {
    cvec_pe vec = cvec_pe_init();
    cvec_pe_push_back(&vec, Person_make(c_new(Person), "Joe", "Jordan"));
    cvec_pe_push_back(&vec, Person_make(c_new(Person), "Annie", "Aniston"));
    cvec_pe_push_back(&vec, Person_make(c_new(Person), "Jane", "Jacobs"));

    cvec_pe_sort(&vec);
    c_foreach (i, cvec_pe, vec)
        printf("%s %s\n", (*i.val)->name.str, (*i.val)->last.str);

    cvec_pe_del(&vec);
}