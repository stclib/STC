# STC [cptr](../stc/cptr.h): Smart Pointers

**cptr** (managed raw pointer) and **csptr** (shared pointer) enables memory managed pointers in containers.

The pointed-to elements are automatically destructed and deleted when the container is destructed. **csptr** elements are only deleted if there are no other shared references to the element. **csptr** uses thread-safe atomic use-count, through the *csptr_X_clone()* and *csptr_X_del()* methods.

 See the c++ classes [std::shared_ptr](https://en.cppreference.com/w/cpp/memory/shared_ptr) for a functional reference.

## Declaration

```c
using_cptr(X, Value, valueCompare=c_default_compare,
                     valueDestroy=c_default_del,
                     valueClone=c_default_clone)

using_csptr(X, Value, valueCompare=c_default_compare,
                      valueDestroy=c_default_del,
                      valueClone=ignored)
```
The macro `using_cptr()` must be instantiated in the global scope. `X` is a type tag name and will
affect the names of all cptr types and methods. E.g. declaring `using_cptr(my, cvec_my);`,
`X` should be replaced by `my` in all of the following documentation.

Note: for shared-ptr **csptr**, "cloning" is done by pointer sharing (ref counting), so *valueClone* is not required and ignored.

## Header file

All cptr definitions and prototypes may be included in your C source file by including a single header file.

```c
#include "stc/cptr.h"
```

## Methods

The *\*_del()* and *\*_compare()* methods are defined based on the methods passed to the *using_\*ptr()* macro. For *csptr* use *csptr_X_clone(p)* when sharing ownership of the pointed-to object to others. See example below.
```c
cptr_X              cptr_X_init(void);
cptr_X              cptr_X_clone(cptr_X ptr);
void                cptr_X_reset(cptr_X* self, cptr_X_value_t* ptr);
void                cptr_X_del(cptr_X* self);
int                 cptr_X_compare(cptr_X* x, cptr_X* y);
```
```c
csptr_X             csptr_X_from(csptr_X_value_t* ptr);
csptr_X             csptr_X_make(csptr_X_value_t val);
csptr_X             csptr_X_clone(csptr_X sptr);
void                csptr_X_reset(csptr_X* self, csptr_X_value_t* ptr);
void                csptr_X_del(csptr_X* self);
int                 csptr_X_compare(csptr_X* x, csptr_X* y);
```

## Types

| Type name          | Type definition       | Used to represent...    |
|:-------------------|:----------------------|:------------------------|
| `cptr_X`           | `cptr_X_value_t *`    | The cptr type           |
| `cptr_X_value_t`   | `Value`               | The cptr element type   |


| Type name           | Type definition                                               | Used to represent...     |
|:--------------------|:--------------------------------------------------------------|:-------------------------|
| `csptr_X`           | `struct { csptr_X_value_t* get; atomic_count_t* use_count; }` | The csptr type           |
| `csptr_X_value_t`   | `Value`                                                       | The csptr element type   |
| `atomic_count_t`    | `long`                                                        | The reference counter    |

## Example

Managed raw pointers (cptr) in a cvec.
```c
#include <stc/cptr.h>
#include <stc/cstr.h>
#include <stc/cvec.h>

typedef struct { cstr_t name, last; } Person;

Person* Person_make(Person* p, const char* name, const char* last) {
    p->name = cstr_from(name), p->last = cstr_from(last);
    return p;
}
void Person_del(Person* p) {
    printf("Destroy: %s %s\n", p->name.str, p->last.str);
    c_del(cstr, &p->name, &p->last);
}
// declare managed pointer and cvec with pointers
using_cptr(pe, Person, c_no_compare, Person_del, c_no_clone);
using_cvec(pe, Person*, c_no_compare, cptr_pe_del, c_no_clone);

int main() {
    cvec_pe vec = cvec_pe_init();
    cvec_pe_push_back(&vec, Person_make(c_new(Person), "John", "Smiths"));
    cvec_pe_push_back(&vec, Person_make(c_new(Person), "Jane", "Doe"));

    c_foreach (i, cvec_pe, vec)
        printf("%s %s\n", (*i.ref)->name.str, (*i.ref)->last.str);
    cvec_pe_del(&vec);
}
```
Output:
```
Average Joe
Joe Blow
Destroy: Average Joe
Destroy: Joe Blow
```
## Example 2

Simple shared pointer (csptr) usage.
```c
#include <stc/cptr.h>
#include <stc/cstr.h>

typedef struct { cstr_t name, last; } Person;

Person* Person_make(Person* p, const char* name, const char* last) {
    p->name = cstr_from(name), p->last = cstr_from(last);
    return p;
}
void Person_del(Person* p) {
    printf("Destroy: %s %s\n", p->name.str, p->last.str);
    c_del(cstr, &p->name, &p->last);
}

using_csptr(pe, Person, c_no_compare, Person_del);

int main() {
    csptr_pe p = csptr_pe_from(Person_make(c_new(Person), "John", "Smiths"));
    csptr_pe q = csptr_pe_clone(p); // means: share the pointer
    
    printf("Person: %s %s. uses: %zu\n", p.get->name.str, p.get->last.str, *p.use_count);
    csptr_pe_del(&p);

    printf("Last man standing: %s %s. uses: %zu\n", q.get->name.str, q.get->last.str, *q.use_count);
    csptr_pe_del(&q);
}
```
Output:
```
Person: John Smiths. uses: 2
Last man standing: John Smiths. uses: 1
Destroy: John Smiths
```

### Example 2

Advanced: Three different ways to store Person in vectors: 1) `cvec<Person>`, 2) `cvec<Person *>`, and 3) `cvec<csptr<Person>>`.
```c
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
using_cvec(pe, Person, Person_compare, Person_del, Person_clone);

// 2. cvec of raw/owned pointers to Person
using_cptr(pe, Person, Person_compare, Person_del, Person_clone);
using_cvec(pp, Person*, cptr_pe_compare, cptr_pe_del, cptr_pe_clone);

// 3. cvec of shared-ptr to Person.
using_csptr(pe, Person, Person_compare, Person_del); // clone is done internally by "sharing"
using_cvec(ps, csptr_pe, csptr_pe_compare, csptr_pe_del, Person_clone);

const char* names[] = {
    "Joe", "Jordan",
    "Annie", "Aniston",
    "Jane", "Jacobs"
};

int main() {
    cvec_pe vec1 = cvec_pe_init();
    cvec_pp vec2 = cvec_pp_init();
    cvec_ps vec3 = cvec_ps_init();

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
```
Output:
```
1. sorted cvec of Person :
  Annie Aniston
  Jane Jacobs
  Joe Jordan

2. sorted cvec of pointer to Person :       
  Annie Aniston
  Jane Jacobs
  Joe Jordan

3. sorted cvec of shared-pointer to Person :
  Annie Aniston
  Jane Jacobs
  Joe Jordan

Destroy vec3:
del: Annie
del: Joe

Destroy vec2:
del: Annie
del: Jane
del: Joe

Destroy vec1:
del: Annie
del: Jane
del: Joe

Destroy elem:
del: Jane
```
