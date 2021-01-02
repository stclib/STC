#include <stc/cptr.h>
#include <stc/cstr.h>
#include <stc/cvec.h>

typedef struct { cstr_t name, last; } Person;

Person* Person_make(Person* p, const char* name, const char* last) {
    p->name = cstr_from(name), p->last = cstr_from(last);
    return p;
}
int Person_compare(const Person* p, const Person* q) {
    int cmp = strcmp(p->name.str, q->name.str);
    return cmp == 0 ? strcmp(p->last.str, q->last.str) : cmp;
}
void Person_del(Person* p) {
    printf("del: %s\n", p->name.str);
    c_del(cstr, &p->name, &p->last);
}
Person Person_clone(Person p) {
    p.name = cstr_clone(p.name);
    p.last = cstr_clone(p.last);
    return p;
}

// 1. cvec of Person struct
using_cvec(pe, Person, Person_compare, Person_del);

// 2. cvec of raw/owned pointers to Person
using_cptr(pe, Person, Person_compare, Person_del);
using_cvec(pp, Person*, cptr_pe_compare, cptr_pe_del, cptr_pe_clone);

// 3. cvec of shared-ptr to Person
using_csptr(pe, Person, Person_compare, Person_del);
using_cvec(ps, csptr_pe, csptr_pe_compare, csptr_pe_del);

const char* names[] = {
    "Joe", "Jordan",
    "Annie", "Aniston",
    "Jane", "Jacobs"
};

int main() {
    cvec_pe vec1 = cvec_inits;
    cvec_pp vec2 = cvec_inits;
    cvec_ps vec3 = cvec_inits;

    for (int i = 0; i < 6; i += 2) {
        Person tmp;
        cvec_pe_push_back(&vec1, *Person_make(&tmp, names[i], names[i+1]));
        cvec_pp_push_back(&vec2, Person_make(c_new(Person), names[i], names[i+1]));
        cvec_ps_push_back(&vec3, csptr_pe_from(Person_make(c_new(Person), names[i], names[i+1])));
    }
    puts("1. sorted cvec of Person :");
    cvec_pe_sort(&vec1);
    c_foreach (i, cvec_pe, vec1)
        printf("  %s %s\n", i.ref->name.str, i.ref->last.str);

    puts("\n2. sorted cvec of pointer to Person :");
    cvec_pp_sort(&vec2);
    c_foreach (i, cvec_pp, vec2)
        printf("  %s %s\n", (*i.ref)->name.str, (*i.ref)->last.str);
        
    puts("\n3. sorted cvec of shared-pointer to Person :");
    cvec_ps_sort(&vec3);
    c_foreach (i, cvec_ps, vec3)
        printf("  %s %s\n", i.ref->get->name.str, i.ref->get->last.str);
  
    // share vec3[1] with elem variable.
    csptr_pe elem = csptr_pe_clone(vec3.data[1]);

    puts("\nDestroy vec3:");
    cvec_ps_del(&vec3); // destroys all elements, but elem!
    puts("\nDestroy vec2:");
    cvec_pp_del(&vec2);
    puts("\nDestroy vec1:");
    cvec_pe_del(&vec1);

    puts("\nDestroy elem:");
    csptr_pe_del(&elem);
}