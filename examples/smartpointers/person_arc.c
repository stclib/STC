/* box example: heap allocated smart pointer type */
#include <stc/cstr.h>

// ===== Person: create a "pro" type:
typedef struct { cstr name, last; } Person;
typedef struct { const char *name, *last; } Person_raw;

Person Person_from(Person_raw raw)
    { return (Person){.name = cstr_from(raw.name), .last = cstr_from(raw.last)}; }

Person_raw Person_toraw(Person* p)
    { return (Person_raw){.name = cstr_str(&p->name), .last = cstr_str(&p->last)}; }

int Person_raw_cmp(const Person_raw* a, const Person_raw* b) {
    int c = strcmp(a->name, b->name);
    return c ? c : strcmp(a->last, b->last);
}

size_t Person_raw_hash(const Person_raw* a)
    { return c_hash_str(a->name) ^ c_hash_str(a->last); }

Person Person_clone(Person p) {
    p.name = cstr_clone(p.name);
    p.last = cstr_clone(p.last);
    return p;
}

void Person_drop(Person* p) {
    printf("drop: %s %s\n", cstr_str(&p->name), cstr_str(&p->last));
    c_drop(cstr, &p->name, &p->last);
}
// =====

// binds Person_clone, Person_drop, enable search/sort
// Person is a "pro" type (has Person_raw conversion type):
#define T PersArc, Person, (c_keypro | c_use_cmp)
#include <stc/arc.h>

// Arcs and Boxes are always "pro" types:
#define T Persons, PersArc, (c_keypro | c_use_cmp)
#include <stc/vec.h>


int main(void)
{
    Persons vec = {0};
    PersArc laura = PersArc_from((Person_raw){"Laura", "Palmer"});
    PersArc bobby = PersArc_from((Person_raw){"Bobby", "Briggs"});

    c_defer(
        PersArc_drop(&laura),
        PersArc_drop(&bobby),
        Persons_drop(&vec)
    ){
        // Use Persons_emplace() to implicitly call PersArc_from() on the argument:
        Persons_emplace(&vec, (Person_raw){"Audrey", "Home"});
        Persons_emplace(&vec, (Person_raw){"Dale", "Cooper"});

        Persons_push(&vec, PersArc_clone(laura));
        Persons_push(&vec, PersArc_clone(bobby));

        for (c_each(i, Persons, vec)) {
            Person_raw p = Persons_value_toraw(i.ref);
            printf("%s %s (%d)\n", p.name, p.last, (int)PersArc_use_count(*i.ref));
        }
        puts("");

        // Look-up Audrey!
        const PersArc *a = Persons_find(&vec, (Person_raw){"Audrey", "Home"}).ref;
        if (a) {
            Person_raw p = Persons_value_toraw(a); // two-level unwrap!
            printf("found: %s %s\n", p.name, p.last);
        }
    }
}
