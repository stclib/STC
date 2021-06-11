# STC [csptr](../include/stc/csptr.h): Shared Pointers

**csptr** is a smart pointer that retains shared ownership of an object through a pointer. 
Several **csptr** objects may own the same object. The object is destroyed and its memory
deallocated when either of the following happens:

- the last remaining **csptr** owning the object is destroyed with *csptr_X_del()*;
- the last remaining **csptr** owning the object is assigned another pointer via *csptr_X_clone()*, *csptr_X_move()* or by *csptr_X_reset()*.

The object is destroyed using *csptr_X_del()* or a custom deleter that is supplied to **csptr**
in the using-statement.

A **csptr** may also own no objects, in which case it is called empty.

All **csptr** functions can be called by multiple threads on different instances of **csptr** without
additional synchronization even if these instances are copies and share ownership of the same object.
**csptr** uses thread-safe atomic reference counting, through the *csptr_X_clone()* and *csptr_X_del()* methods.

See the c++ classes [std::shared_ptr](https://en.cppreference.com/w/cpp/memory/shared_ptr) for a functional reference.

## Header file and declaration

```c
#include <stc/csptr.h>

using_csptr(X, Value);
using_csptr(X, Value, valueCompare);
using_csptr(X, Value, valueCompare, valueDel);
```
The macro `using_csptr()` must be instantiated in the global scope. `X` is a type tag name and will affect the names of all csptr types and methods. E.g. declaring `using_csptr(v4, Vec4)`, `X` should be replaced by `v4` in all of the following documentation.

## Methods

Use *csptr_X_clone(p)* when sharing ownership of the pointed-to object. See examples below.

The *csptr_X_compare()*, *csptr_X_equals()* and *csptr_X_del()* methods are defined based on the *valeCompare* and *valueDel* arguments passed to the **using**-macro.

```c
csptr_X             csptr_X_init();                               // empty constructor
csptr_X             csptr_X_make(Value val);                      // make_shared constructor, fast
csptr_X             csptr_X_from(Value* p);                       // construct from raw pointer
csptr_X             csptr_X_clone(csptr_X ptr);                   // clone shared (increase use count)
csptr_X             csptr_X_move(csptr_X* self);                  // fast transfer ownership to another sptr.

void                csptr_X_del(csptr_X* self);                   // destruct (decrease use count, free at 0)
long                csptr_X_use_count(csptr_X ptr);

void                csptr_X_reset(csptr_X* self);
csptr_X_value_t*    csptr_X_reset_make(csptr_X* self, Value val); // assign new sptr with value
csptr_X_value_t*    csptr_X_reset_with(csptr_X* self, Value* p);  // slower than reset_make().
csptr_X_value_t*    csptr_X_copy(csptr_X* self, CX other);        // copy shared (increase use count)

int                 csptr_X_compare(const csptr_X* x, const csptr_X* y);
bool                csptr_X_equals(const csptr_X* x, const csptr_X* y);
```

## Types and constants

| Type name           | Type definition                                               | Used to represent...     |
|:--------------------|:--------------------------------------------------------------|:-------------------------|
| `csptr_null`        | `{NULL, NULL}`                                                | Init nullptr const       |
| `csptr_X`           | `struct { csptr_X_value_t* get; atomic_count_t* use_count; }` | The csptr type           |
| `csptr_X_value_t`   | `Value`                                                       | The csptr element type   |
| `atomic_count_t`    | `long`                                                        | The reference counter    |

## Example

```c
#include <stc/csptr.h>
#include <stc/cstr.h>

typedef struct { cstr name, last; } Person;

Person Person_init(const char* name, const char* last) {
    return (Person){.name = cstr_from(name), .last = cstr_from(last)};
}
void Person_del(Person* p) {
    printf("Destroy: %s %s\n", p->name.str, p->last.str);
    c_del(cstr, &p->name, &p->last);
}

using_csptr(person, Person, c_no_compare, Person_del);

int main() {
    csptr_person p = csptr_person_make(Person_init("John", "Smiths"));
    csptr_person q = csptr_person_clone(p); // means: share the pointer

    printf("Person: %s %s. uses: %zu\n", p.get->name.str, p.get->last.str, *p.use_count);
    csptr_person_del(&p);

    printf("Last man standing: %s %s. uses: %zu\n", q.get->name.str, q.get->last.str, *q.use_count);
    csptr_person_del(&q);
}
```
Output:
```
Person: John Smiths. uses: 2
Last man standing: John Smiths. uses: 1
Destroy: John Smiths
```

### Example 2

Advanced: Two different ways to store Person in vectors: 1) `cvec<Person>`, 2) `cvec< csptr<Person> >`.
```c
#include <stc/csptr.h>
#include <stc/cstr.h>
#include <stc/cvec.h>

typedef struct { cstr name, last; } Person;

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

// 1. cvec of Person struct; emplace and cloning disabled.
using_cvec(pe, Person, Person_compare, Person_del, c_no_clone);

// 2. cvec of shared-ptr to Person - with emplace_back() and cloning cvec ENABLED.
using_csptr(pe, Person, Person_compare, Person_del);
using_cvec(ps, csptr_pe, csptr_pe_compare, csptr_pe_del, csptr_pe_clone);

const char* names[] = {
    "Joe", "Jordan",
    "Annie", "Aniston",
    "Jane", "Jacobs"
};

int main() {
    cvec_pe vec1 = cvec_pe_init();
    cvec_ps vec2 = cvec_ps_init();

    for (int i = 0; i < 6; i += 2) {
        Person tmp;
        cvec_pe_push_back(&vec1, *Person_make(&tmp, names[i], names[i+1]));
        cvec_ps_push_back(&vec2, csptr_pe_from(Person_make(c_new(Person), names[i], names[i+1])));
    }
    puts("1. Sorted vec1 of Person:");
    cvec_pe_sort(&vec1);
    c_foreach (i, cvec_pe, vec1)
        printf("  %s %s\n", i.ref->name.str, i.ref->last.str);

    // Append a shared copy of vec2.data[0]. Will only be destructed once!
    cvec_ps_emplace_back(&vec2, vec2.data[0]);   // emplace will internally call csptr_ps_clone()!
    puts("\n2. Sorted vec2 of shared-pointer to Person:");
    cvec_ps_sort(&vec2);
    c_foreach (i, cvec_ps, vec2)
        printf("  %s %s\n", i.ref->get->name.str, i.ref->get->last.str);

    // Share vec2.data[1] with elem1 variable.
    csptr_pe elem1 = csptr_pe_clone(vec2.data[1]);

    puts("\nDestroy vec1:");
    cvec_pe_del(&vec1);
    puts("\nDestroy vec2:");
    cvec_ps_del(&vec2);

    puts("\nDestroy elem1:");
    csptr_pe_del(&elem1);
}
```
Output:
```
1. Sorted vec1 of Person:
  Annie Aniston
  Jane Jacobs
  Joe Jordan

2. Sorted vec2 of shared-pointer to Person:
  Annie Aniston
  Jane Jacobs
  Joe Jordan
  Joe Jordan

Destroy vec1:
del: Annie
del: Jane
del: Joe

Destroy vec2:
del: Annie
del: Joe

Destroy elem1:
del: Jane
```
