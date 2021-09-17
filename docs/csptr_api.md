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
#define i_tag       // defaults to i_val name
#define i_val       // value: REQUIRED
#define i_cmp       // three-way compare two i_val* : REQUIRED IF i_val is a non-integral type
#define i_valdel    // destroy value func - defaults to empty destruct
#include <stc/csptr.h>
```
`X` should be replaced by the value of i_tag in all of the following documentation.

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
#include <stc/cstr.h>

typedef struct { cstr name, surname; } Person;

Person Person_init(const char* name, const char* surname) {
    return (Person){.name = cstr_from(name), .surname = cstr_from(surname)};
}
void Person_del(Person* p) {
    printf("Person_del: %s %s\n", p->name.str, p->surname.str);
    c_del(cstr, &p->name, &p->surname);
}

#define i_val Person
#define i_cmp c_no_compare
#define i_valdel Person_del
#include <stc/csptr.h>

int main() {
    csptr_person p = csptr_person_make(Person_init("John", "Smiths"));
    csptr_person q = csptr_person_clone(p); // means: share the pointer

    printf("Person: %s %s. uses: %zu\n", p.get->name.str, p.get->surname.str, *p.use_count);
    csptr_person_del(&p);

    printf("Last man standing: %s %s. uses: %zu\n", q.get->name.str, q.get->surname.str, *q.use_count);
    csptr_person_del(&q);
}
```
Output:
```
Person: John Smiths. uses: 2
Last man standing: John Smiths. uses: 1
Person_del: John Smiths
```

### Example 2

Vector of shared pointers to Person:
```c
#include <stc/cstr.h>

typedef struct { cstr name, surname; } Person;

Person Person_init(const char* name, const char* surname) {
    return (Person){.name = cstr_from(name), .surname = cstr_from(surname)};
}
void Person_del(Person* p) {
    printf("Person_del: %s %s\n", p->name.str, p->surname.str);
    c_del(cstr, &p->name, &p->surname);
}
int Person_compare(const Person* p, const Person* q) {
    int cmp = strcmp(p->surname.str, q->surname.str);
    return cmp == 0 ? strcmp(p->name.str, q->name.str) : cmp;
}

#define i_tag pers
#define i_val Person
#define i_cmp Person_compare
#define i_valdel Person_del
#include <stc/csptr.h>

#define i_val_csptr pers  // shorthand: derives other i_xxx defines from this. i_tag may be defined.
#include <stc/cvec.h>

const char* names[] = {
    "Joe", "Jordan",
    "Annie", "Aniston",
    "Jane", "Jacobs"
};

int main() {
    cvec_pers vec = cvec_pers_init();

    for (int i = 0; i < c_arraylen(names); i += 2) {
        cvec_pers_push_back(&vec, csptr_pers_make(Person_init(names[i], names[i+1])));
    }

    // Append a shared copy of vec.data[0]. Will only be destructed once!
    cvec_pers_emplace_back(&vec, vec.data[0]);  // will internally call csptr_pers_clone()!
    
    puts("\nSorted vec of shared-pointer to Person:");
    cvec_pers_sort(&vec);
    
    c_foreach (i, cvec_pers, vec)
        printf("  %s, %s\n", i.ref->get->surname.str, i.ref->get->name.str);

    // Share vec.data[1] with elem1 variable.
    csptr_pers elem1 = csptr_pers_clone(vec.data[1]);

    puts("\nDestroy vec:");
    cvec_pers_del(&vec);

    puts("\nDestroy elem1:");
    csptr_pers_del(&elem1);
}
```
Output:
```
Sorted vec of shared-pointer to Person:
  Aniston, Annie
  Jacobs, Jane
  Jordan, Joe
  Jordan, Joe

Destroy vec:
Person_del: Annie Aniston
Person_del: Joe Jordan

Destroy elem1:
Person_del: Jane Jacobs
```
