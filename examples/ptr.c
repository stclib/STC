#include <stc/cptr.h>
#include <stc/cstr.h>
#include <stc/cvec.h>

typedef struct { cstr_t name, last; } Person;

Person* Person_make(Person* p, const char* name, const char* last) {
    p->name = cstr_from(name), p->last = cstr_from(last);
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

using_cvec(pe, Person, Person_compare, Person_del);

using_cptr(pu, Person, Person_compare, Person_del);
using_cvec(pu, Person*, cptr_pu_compare, cptr_pu_del);

using_csptr(ps, Person, Person_compare, Person_del);
using_cvec(ps, csptr_ps, csptr_ps_compare, csptr_ps_del);


const char* names[] = {
    "Joe", "Jordan",
    "Annie", "Aniston",
    "Jane", "Jacobs"
};

int main() {
    Person tmp;
    cvec_pe vec = cvec_inits;
    for (int i=0;i<6; i+=2) cvec_pe_push_back(&vec, *Person_make(&tmp, names[i], names[i+1]));
    puts("cvec of Person:");
    cvec_pe_sort(&vec);
    c_foreach (i, cvec_pe, vec)
        printf("  %s %s\n", i.ref->name.str, i.ref->last.str);

    cvec_pu uvec = cvec_inits;
    for (int i=0;i<6; i+=2) cvec_pu_push_back(&uvec, Person_make(c_new(Person), names[i], names[i+1]));
    puts("cvec of cptr<Person>:");
    cvec_pu_sort(&uvec);
    c_foreach (i, cvec_pu, uvec)
        printf("  %s %s\n", (*i.ref)->name.str, (*i.ref)->last.str);

    cvec_ps svec = cvec_inits;
    for (int i=0;i<6; i+=2) cvec_ps_push_back(&svec, csptr_ps_from(Person_make(c_new(Person), names[i], names[i+1])));
    puts("cvec of csptr<Person>:");
    cvec_ps_sort(&svec);
    c_foreach (i, cvec_ps, svec)
        printf("  %s %s\n", (*i.ref).get->name.str, (*i.ref).get->last.str);
    
    csptr_ps x = csptr_ps_share(svec.data[1]);

    puts("\nDestroy svec:");
    cvec_ps_del(&svec);
    puts("\nDestroy pvec:");
    cvec_pu_del(&uvec);
    puts("\nDestroy vec:");
    cvec_pe_del(&vec);
    puts("\nDestroy x:");
    csptr_ps_del(&x);
}