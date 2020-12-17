# Module [cptr](../stc/cptr.h): Smart Pointers

This describes the API of the pointer type **cptr** and the shared pointer type **csptr**. Type **cptr** is meant to be used like a c++ *std::unique_ptr*, while **csptr** is similar to c++ *std::shared_ptr*.

**cptr** and **csptr** objects can be used as items of containers. The pointed-to elements are automatically destructed+deleted when the container is destructed. **csptr** elements are only deleted if there are no other shared references to the element. **csptr** has thread-safe atomic use count, enabled by the *csptr_X_share(sp)* and *csptr_X_del(&sp)* methods.

## Declaration

```c
#define using_cptr(X, Value, valueDestroy=c_default_del,
                             valueCompare=c_default_compare)

#define using_csptr(X, Value, valueDestroy=c_default_del,
                              valueCompare=c_default_compare)
```
The macro `using_cptr()` must be instantiated in the global scope. `X` is a type tag name and will
affect the names of all cptr types and methods. E.g. declaring `using_cptr(my, cvec_my);`,
`X` should be replaced by `my` in all of the following documentation.

 Types

| Type name          | Type definition       | Used to represent...    |
|:-------------------|:----------------------|:------------------------|
| `cptr_X`           | `cptr_X_value_t *`    | The cptr type           |
| `cptr_X_value_t`   | `Value`               | The cptr element type   |


| Type name           | Type definition                                               | Used to represent...     |
|:--------------------|:--------------------------------------------------------------|:-------------------------|
| `csptr_X`           | `struct { csptr_X_value_t* get; atomic_count_t* use_count; }` | The csptr type           |
| `csptr_X_value_t`   | `Value`                                                       | The csptr element type   |
| `atomic_count_t`    | `long`                                                        | The reference counter    |


## Header file

All cptr definitions and prototypes may be included in your C source file by including a single header file.

```c
#include "stc/cptr.h"
```

## Methods

The *\*_del()* and *\*_compare()* methods are defined based on the methods passed to the *using_\*ptr()* macro. For *csptr* use *csptr_X_share(p)* when sharing ownership of the pointed-to object to others. See example, where x has shared ownership with the shared pointer element in cvec container. Refer to c++ std::shared_ptr for more information.
```c
cptr_X              cptr_X_init(void);
void                cptr_X_reset(cptr_X* self, cptr_X_value_t* ptr);
void                cptr_X_del(cptr_X* self);
int                 cptr_X_compare(cptr_X* x, cptr_X* y);
```
```c
csptr_X             csptr_X_from(csptr_X_value_t* ptr);
csptr_X             csptr_X_make(csptr_X_value_t ref);
csptr_X             csptr_X_share(csptr_X ptr);
void                csptr_X_reset(csptr_X* self, csptr_X_value_t* p);
void                csptr_X_del(csptr_X* self);
int                 csptr_X_compare(csptr_X* x, csptr_X* y);
```

## Example

This shows 2 cvecs with two different pointer to Person. pvec: cptr<Person>, and svec: csptr<Person>.
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
    printf("del: %s\n", p->name.str);
    c_del(cstr, &p->name, &p->last);
}
int Person_compare(const Person* p, const Person* q) {
    int cmp = strcmp(p->name.str, q->name.str);
    return cmp == 0 ? strcmp(p->last.str, q->last.str) : cmp;
}

using_cvec(pe, Person, Person_del, Person_compare); // unused

using_cptr(pp, Person, Person_del, Person_compare);
using_cvec(pp, Person*, cptr_pp_del, cptr_pp_compare);

using_csptr(ps, Person, Person_del, Person_compare);
using_cvec(ps, csptr_ps, csptr_ps_del, csptr_ps_compare);

const char* names[] = {
    "Joe", "Jordan",
    "Annie", "Aniston",
    "Jane", "Jacobs"
};

int main() {
    cvec_pp pvec = cvec_inits;
    for (int i=0;i<6; i+=2) cvec_pp_push_back(&pvec, Person_make(c_new(Person), names[i], names[i+1]));
    puts("cvec of cptr<Person>:");
    cvec_pp_sort(&pvec);
    c_foreach (i, cvec_pp, pvec)
        printf("  %s %s\n", (*i.ref)->name.str, (*i.ref)->last.str);

    cvec_ps svec = cvec_inits;
    for (int i=0;i<6; i+=2) cvec_ps_push_back(&svec, csptr_ps_from(Person_make(c_new(Person), names[i], names[i+1])));
    puts("cvec of csptr<Person>:");
    cvec_ps_sort(&svec);
    c_foreach (i, cvec_ps, svec)
        printf("  %s %s\n", i.ref->get->name.str, i.ref->get->last.str);
    
    csptr_ps x = csptr_ps_share(svec.data[1]);

    puts("\nDestroy svec:");
    cvec_ps_del(&svec); // destroys all elements but x!
    puts("\nDestroy pvec:");
    cvec_pp_del(&pvec);
    puts("\nDestroy x:");
    csptr_ps_del(&x);
}
```
Output:
```
cvec of cptr<Person>:
  Annie Aniston
  Jane Jacobs
  Joe Jordan
cvec of csptr<Person>:
  Annie Aniston
  Jane Jacobs
  Joe Jordan

Destroy svec:
del: Annie
del: Joe

Destroy pvec:
del: Annie
del: Jane
del: Joe

Destroy x:
del: Jane
```
