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

using_cvec(pe, Person, Person_del, Person_compare);
using_cptr(pe, Person, Person_del, Person_compare);
using_cvec(pp, Person*, cptr_pe_del, cptr_pe_compare);

int main() {
    puts("Vec of Person *:");
    cvec_pp pvec = cvec_pp_init();
    cvec_pp_push_back(&pvec, Person_make(c_new(Person), "Joe", "Jordan"));
    cvec_pp_push_back(&pvec, Person_make(c_new(Person), "Annie", "Aniston"));
    cvec_pp_push_back(&pvec, Person_make(c_new(Person), "Jane", "Jacobs"));

    cvec_pp_sort(&pvec);
    c_foreach (i, cvec_pp, pvec)
        printf("%s %s\n", (*i.val)->name.str, (*i.val)->last.str);

    puts("\nVec of Person:");
    cvec_pe vec = cvec_pe_init();
    Person tmp;
    cvec_pe_push_back(&vec, *Person_make(&tmp, "Joe", "Jordan"));
    cvec_pe_push_back(&vec, *Person_make(&tmp, "Annie", "Aniston"));
    cvec_pe_push_back(&vec, *Person_make(&tmp, "Jane", "Jacobs"));

    cvec_pe_sort(&vec);
    c_foreach (i, cvec_pe, vec)
        printf("%s %s\n", i.val->name.str, i.val->last.str);

    puts("\nDestroy pvec:");
    cvec_pp_del(&pvec);
    puts("\nDestroy vec:");
    cvec_pe_del(&vec);
}