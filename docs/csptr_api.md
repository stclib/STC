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
`X` should be replaced by the value of ***i_tag*** in all of the following documentation.

## Methods

The *csptr_X_compare()*, *csptr_X_del()* methods are defined based on the ***i_cmp*** and ***i_valdel*** macros specified.

Use *csptr_X_clone(p)* when sharing ownership of the pointed-to object. For shared pointers stored in containers, define ***i_val_csptr*** to the shared pointers tag instead of a ***i_val*** macro. See example below.
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
| `csptr_X_value_t`   | `i_val`                                                       | The csptr element type   |
| `atomic_count_t`    | `long`                                                        | The reference counter    |

## Example

```c
#include <stdio.h>

void int_del(int* x) {
    printf("del: %d\n", *x);
}

#define i_val int
#define i_valdel int_del  // optional func to show elements destroyed
#include <stc/csptr.h>    // define csptr_int shared pointers

#define i_key_csptr int   // refer to csptr_int definition above
#include <stc/csset.h>    // define a sorted set of csptr_int

#define i_val_csptr int
#include <stc/cvec.h>

int main()
{
    c_auto (cvec_int, vec)   // declare and init vec, call del at scope exit
    c_auto (csset_int, set)  // declare and init set, call del at scope exit
    {
        cvec_int_push_back(&vec, csptr_int_make(2021));
        cvec_int_push_back(&vec, csptr_int_make(2012));
        cvec_int_push_back(&vec, csptr_int_make(2022));
        cvec_int_push_back(&vec, csptr_int_make(2015));

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
