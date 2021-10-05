# STC [csptr](../include/stc/csptr.h): Shared Pointers

**csptr** is a smart pointer that retains shared ownership of an object through a pointer.
Several **csptr** objects may own the same object. The object is destroyed and its memory
deallocated when the last remaining **csptr** owning the object is destroyed with *csptr_X_del()*;

The object is destroyed using *csptr_X_del()*. A **csptr** may also own no objects, in which 
case it is called empty. The *csptr_X_compare()*, *csptr_X_del()* methods are defined based on
the `i_cmp` and `i_valdel` macros specified. Use *csptr_X_clone(p)* when sharing ownership of
the pointed-to object. 

All **csptr** functions can be called by multiple threads on different instances of **csptr** without
additional synchronization even if these instances are copies and share ownership of the same object.
**csptr** uses thread-safe atomic reference counting, through the *csptr_X_clone()* and *csptr_X_del()* methods.

When declaring a container with shared pointers, define the `i_val_csptr` with the full csptr type.
See example.

Also for containers, make sure to pass the result of *csptr_X_make()* to *insert*, *push_back*,
or *push*, and not an *emplace* function. The *csptr_X_make()* method creates a **csptr** with 
use-count 1, and *emplace* will ***clone*** it and increase the count, causing a memory leak. Use
*emplace* functions when sharing **csptr**s between containers or other existing shared pointers.

See the c++ classes [std::shared_ptr](https://en.cppreference.com/w/cpp/memory/shared_ptr) for a functional reference.

## Header file and declaration

```c
#define i_val       // value: REQUIRED
#define i_cmp       // three-way compare two i_val* : REQUIRED IF i_val is a non-integral type
#define i_del       // destroy value func - defaults to empty destruct
#define i_tag       // defaults to i_val
#include <stc/csptr.h>
```
`X` should be replaced by the value of `i_tag` in all of the following documentation.

## Methods
```c
csptr_X             csptr_X_init();                               // empty constructor
csptr_X             csptr_X_make(i_val val);                      // make_shared constructor, like std::make_shared()
csptr_X             csptr_X_from(i_val* p);                       // construct from raw pointer
csptr_X             csptr_X_clone(csptr_X ptr);                   // return ptr with increased use count
csptr_X             csptr_X_move(csptr_X* self);                  // transfer ownership to another sptr.
void                csptr_X_take(csptr_X* self, csptr_X other);   // take a new-created or moved csptr
void                csptr_X_copy(csptr_X* self, csptr_X other);   // copy shared (increase use count)

void                csptr_X_del(csptr_X* self);                   // destruct (decrease use count, free at 0)
long                csptr_X_use_count(csptr_X ptr);

void                csptr_X_reset(csptr_X* self);
void                csptr_X_reset_with(csptr_X* self, i_val val); // make and assign new csptr with value
void                csptr_X_reset_from(csptr_X* self, i_val* p);  // create csptr from p.

int                 csptr_X_compare(const csptr_X* x, const csptr_X* y);
bool                csptr_X_equals(const csptr_X* x, const csptr_X* y);
```

## Types and constants

| Type name           | Type definition                                               | Used to represent...     |
|:--------------------|:--------------------------------------------------------------|:-------------------------|
| `csptr_null`        | `{NULL, NULL}`                                                | Init nullptr const       |
| `csptr_X`           | `struct { csptr_X_value_t* get; atomic_count_t* use_count; }` | The csptr type           |
| `csptr_X_value_t`   | `i_val`                                                       | The csptr element type   |
| `atomic_count_t`    | `long`                                                        | The reference counter    |

## Example

```c
#include <stdio.h>

void int_del(int* x) {
    printf("del: %d\n", *x);
}

#define i_val int
#define i_valdel int_del  // optional func to display elements destroyed
#include <stc/csptr.h>    // csptr_int

#define i_key_csptr csptr_int
#define i_tag int
#include <stc/csset.h>    // csset_int: csset<csptr_int>

#define i_val_csptr csptr_int
#define i_tag int
#include <stc/cvec.h>     // cvec_int: cvec<csptr_int>

int main()
{
    c_auto (cvec_int, vec)   // declare and init vec, call del at scope exit
    c_auto (csset_int, set)  // declare and init set, call del at scope exit
    {
        c_apply(cvec_int, push_back, &vec, {
            csptr_int_make(2021)),
            csptr_int_make(2012)),
            csptr_int_make(2022)),
            csptr_int_make(2015)),
        });
        printf("vec:");
        c_foreach (i, cvec_int, vec) printf(" %d", *i.ref->get);
        puts("");

        // add odd numbers from vec to set
        c_foreach (i, cvec_int, vec)
            if (*i.ref->get & 1)
                csset_int_emplace(&set, *i.ref); // copy shared pointer => increments counter.

        // erase the two last elements in vec
        cvec_int_pop_back(&vec);
        cvec_int_pop_back(&vec);

        printf("vec:");
        c_foreach (i, cvec_int, vec) printf(" %d", *i.ref->get);

        printf("\nset:");
        c_foreach (i, csset_int, set) printf(" %d", *i.ref->get);

        c_autovar (csptr_int p = csptr_int_clone(vec.data[0]), csptr_int_del(&p)) {
            printf("\n%d is now owned by %zu objects\n", *p.get, *p.use_count);
        }

        puts("\nDone");
    }
}
```
Output:
```
vec: 2021 2012 2022 2015
del: 2022
vec: 2021 2012
set: 2015 2021
2021 is now owned by 3 objects
Done
del: 2015
del: 2021
del: 2012
```
