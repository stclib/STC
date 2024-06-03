# STC [box](../include/stc/box.h): Smart Pointer (boxed object)

**box** is a smart pointer to a heap allocated value of type X. A **box** can
be empty. The *box_X_cmp()*, *box_X_drop()* methods are defined based on the `i_cmp`
and `i_keydrop` macros specified. Use *box_X_clone(p)* to make a deep copy, which uses the
`i_keyclone` macro if defined. Note that a box set to NULL is consider uninitialized, and
cannot be e.g. cloned or dropped.

When declaring a container of **box** values, define `i_key_box` with the
box type instead of defining `i_key`. This will auto-set `i_keydrop`, `i_keyclone`, and `i_cmp` using
functions defined by the specified **box**.

See similar c++ class [std::unique_ptr](https://en.cppreference.com/w/cpp/memory/unique_ptr) for a functional reference, or Rust [std::boxed::Box](https://doc.rust-lang.org/std/boxed/struct.Box.html)

## Header file and declaration

```c
#define i_TYPE <ct>,<kt> // shorthand to define i_type,i_key
#define i_type <t>       // box container type name (default: box_{i_key})
#define i_key <t>        // element type: REQUIRED. Note: i_val* may be specified instead of i_key*.
#define i_key_box <t>    // Use instead of i_key when key itself is a box-type.
#define i_keyclass <t>   // Use instead of i_key when functions {i_key}_clone,
                         //   {i_key}_drop and {i_keyraw}_cmp exist.
#define i_cmp <f>        // three-way compareison. REQUIRED IF i_key is a non-integral type
                         // Note that containers of arcs will "inherit" i_cmp
                         // when using arc in containers with i_val_arc MyArc - ie. the i_type.
#define i_use_cmp        // may be defined instead of i_cmp when i_key is an integral/native-type.
#define i_keydrop <f>    // destroy element func - defaults to empty destruct
#define i_keyclone <f>   // REQUIRED if i_keydrop is defined, unless 'i_opt c_no_clone' is defined.

#define i_keyraw <t>     // convertion type (lookup): default to {i_key}
#define i_keyto <f>      // convertion func i_key* => i_keyraw: REQUIRED IF i_keyraw defined.
#define i_keyfrom <f>    // from-raw func.
#define i_tag <s>        // alternative typename: box_{i_tag}. i_tag defaults to i_key
#include "stc/box.h"
```
`X` should be replaced by the value of `i_tag` in all of the following documentation.
Unless `c_use_cmp` is defined, comparison between i_key's is not needed/available. Will then
compare the pointer addresses when used. Additionally, `c_no_clone` or `i_is_fwd` may be defined.

## Methods
```c
box_X       box_X_init();                                   // return an empty box
box_X       box_X_from(i_keyraw raw);                       // create a box from raw type. Avail if i_keyraw user defined.
box_X       box_X_from_ptr(i_key* ptr);                     // create a box from a pointer. Takes ownership of ptr.
box_X       box_X_make(i_key val);                          // create a box from unowned val object.

box_X       box_X_clone(box_X other);                       // return deep copied clone
box_X       box_X_move(box_X* self);                        // transfer ownership to receiving box returned. self becomes NULL.
void        box_X_take(box_X* self, box_X unowned);         // take ownership of unowned box object.
void        box_X_assign(box_X* self, box_X* moved);        // transfer ownership from moved to self; moved becomes NULL.
void        box_X_drop(box_X* self);                        // destruct the contained object and free its heap memory.

void        box_X_reset_to(box_X* self, i_key* p);          // assign new box from ptr. Takes ownership of p.

uint64_t    box_X_hash(const box_X* x);                     // hash value
int         box_X_cmp(const box_X* x, const box_X* y);      // compares pointer addresses if no `i_cmp` is specified.
                                                            // is defined. Otherwise uses 'i_cmp' or default cmp.
bool        box_X_eq(const box_X* x, const box_X* y);       // box_X_cmp() == 0

// functions on pointed to objects.

uint64_t    box_X_value_hash(const i_key* x);
int         box_X_value_cmp(const i_key* x, const i_key* y);
bool        box_X_value_eq(const i_key* x, const i_key* y);
```

## Types and constants

| Type name         | Type definition                 | Used to represent...     |
|:------------------|:--------------------------------|:-----------------------|
| `box_null`        | `{0}`                           | Init nullptr const     |
| `box_X`           | `struct { box_X_value* get; }`  | The box type           |
| `box_X_value`     | `i_key`                         | The box element type   |

## Example
Create a vec and a set with owned pointers to int elements, using box.
```c
#include <stdio.h>
void int_drop(int* x) {
    printf("\n drop %d", *x);
}

#define i_TYPE IBox,int
#define i_keydrop int_drop  // optional func, just to display elements destroyed
#define i_keyclone(x) x     // must be specified when i_keydrop is defined.
#define i_use_cmp           // enable usage of default comparison == and < operators
#include "stc/box.h"

#define i_type ISet
#define i_key_box IBox      // NB: use i_key_box instead of i_key
#include "stc/sset.h"       // ISet : std::set<std::unique_ptr<int>>

#define i_type IVec
#define i_key_box IBox      // NB: use i_key_box instead of i_key
#include "stc/vec.h"        // IVec : std::vector<std::unique_ptr<int>>

int main(void)
{
    IVec vec = c_init(IVec, {2021, 2012, 2022, 2015});
    ISet set = {0};
    c_defer(
      IVec_drop(&vec),
      ISet_drop(&set)
    ){
        printf("vec:");
        c_foreach (i, IVec, vec)
            printf(" %d", *i.ref->get);

        // add odd numbers from vec to set
        c_foreach (i, IVec, vec)
            if (*i.ref->get & 1)
                ISet_insert(&set, IBox_clone(*i.ref));

        // pop the two last elements in vec
        IVec_pop(&vec);
        IVec_pop(&vec);

        printf("\nvec:");
        c_foreach (i, IVec, vec)
            printf(" %d", *i.ref->get);

        printf("\nset:");
        c_foreach (i, ISet, set)
            printf(" %d", *i.ref->get);
    }
}
```
Output:
```
vec: 2021 2012 2022 2015
 drop 2015
 drop 2022
vec: 2021 2012
set: 2015 2021
 drop 2021
 drop 2015
 drop 2012
 drop 2021
```
