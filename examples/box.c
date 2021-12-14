/* cbox: heap allocated boxed type */
#include <stc/cstr.h>

typedef struct { cstr name, last; } Person;

Person Person_from(const char* name, const char* last) {
    return (Person){.name = cstr_from(name), .last = cstr_from(last)};
}

int Person_cmp(const Person* a, const Person* b) {
    int c = strcmp(a->name.str, b->name.str);
    return c ? c : strcmp(a->last.str, b->last.str);
}

Person Person_clone(Person p) {
    p.name = cstr_clone(p.name);
    p.last = cstr_clone(p.last);
    return p;
}
void Person_del(Person* p) {
    printf("del: %s %s\n", p->name.str, p->last.str);
    c_del(cstr, &p->name, &p->last);
}

#define i_val Person
#define i_cmp Person_cmp
#define i_from Person_clone
#define i_del Person_del
#define i_tag prs
#include <stc/cbox.h>

#define i_val_ref cbox_prs
#define i_tag prs
#include <stc/cvec.h>


int main()
{
    c_auto (cvec_prs, vec)
    c_auto (cbox_prs, p, q)
    {
        p = cbox_prs_new(Person_from("Dave", "Cooper"));

        q = cbox_prs_clone(p);
        cstr_assign(&q.get->name, "Dale");

        printf("%s %s.\n", p.get->name.str, p.get->last.str);
        printf("%s %s.\n", q.get->name.str, q.get->last.str);

        cvec_prs_push_back(&vec, cbox_prs_new(Person_from("Laura", "Palmer")));
        cvec_prs_push_back(&vec, cbox_prs_new(Person_from("Shelly", "Johnson")));

        c_foreach (i, cvec_prs, vec)
            printf("%s: %s\n", i.ref->get->name.str, i.ref->get->last.str);
        
        c_autovar (Person per = Person_from("Laura", "Palmer"), Person_del(&per)) {
            const cbox_prs *v = cvec_prs_get(&vec, (cbox_prs){&per});
            if (v) printf("found: %s: %s\n", v->get->name.str, v->get->last.str);
        }
    }
}
