# STC [cbox](../include/stc/cbox.h): Smart Pointer (Boxed object)

**cbox** is a smart pointer to a heap allocated value of type X. A **cbox** can
be empty. The *cbox_X_cmp()*, *cbox_X_drop()* methods are defined based on the `i_cmp`
and `i_valdrop` macros specified. Use *cbox_X_clone(p)* to make a deep copy, which uses the
`i_valclone` macro if defined.

When declaring a container of **cbox** values, define `i_valboxed` with the
cbox type instead of defining `i_val`. This will auto-set `i_valdrop`, `i_valclone`, and `i_cmp` using 
functions defined by the specified **cbox**.

See similar c++ class [std::unique_ptr](https://en.cppreference.com/w/cpp/memory/unique_ptr) for a functional reference, or Rust [std::boxed::Box](https://doc.rust-lang.org/std/boxed/struct.Box.html)

## Header file and declaration

```c
#define i_type          // full typename of the cbox
#define i_val           // value: REQUIRED
#define i_cmp           // three-way compare two i_val* : REQUIRED IF i_val is a non-integral type
#define i_valdrop       // destroy value func - defaults to empty destruct
#define i_valclone      // REQUIRED if i_valdrop is defined, unless 'i_opt c_no_clone' is defined.

#define i_valraw        // convertion type (lookup): default to {i_val}
#define i_valto         // convertion func i_val* => i_valraw: REQUIRED IF i_valraw defined.
#define i_valfrom       // from-raw func.

#define i_valclass      // alt. to i_val: REQUIRES that {i_val}_clone, {i_val}_drop, {i_valraw}_cmp exist.
#define i_tag           // alternative typename: cbox_{i_tag}. i_tag defaults to i_val
#include <stc/cbox.h>    
```
`X` should be replaced by the value of `i_tag` in all of the following documentation.
Define `i_opt` with `c_no_cmp` if comparison between i_val's is not needed/available. Will then
compare the pointer addresses when used. Additionally, `c_no_clone` or `i_is_fwd` may be defined.

## Methods
```c
cbox_X      cbox_X_init();                                    // return an empty cbox
cbox_X      cbox_X_from(i_valraw raw);                        // create a cbox from raw type. Avail if i_valraw user defined.
cbox_X      cbox_X_from_ptr(i_val* ptr);                      // create a cbox from a pointer. Takes ownership of ptr.
cbox_X      cbox_X_make(i_val val);                           // create a cbox from unowned val object.

cbox_X      cbox_X_clone(cbox_X other);                       // return deep copied clone
cbox_X      cbox_X_move(cbox_X* self);                        // transfer ownership to receiving cbox returned. self becomes NULL.
void        cbox_X_take(cbox_X* self, cbox_X unowned);        // take ownership of unowned box object.
void        cbox_X_assign(cbox_X* self, cbox_X* moved);       // transfer ownership from moved to self; moved becomes NULL.
void        cbox_X_drop(cbox_X* self);                        // destruct the contained object and free its heap memory.

void        cbox_X_reset(cbox_X* self);   
void        cbox_X_reset_to(cbox_X* self, i_val* p);          // assign new cbox from ptr. Takes ownership of p.

uint64_t    cbox_X_hash(const cbox_X* x);                     // hash value
int         cbox_X_cmp(const cbox_X* x, const cbox_X* y);     // compares pointer addresses if no `i_cmp` is specified.
                                                              // is defined. Otherwise uses 'i_cmp' or default cmp.
bool        cbox_X_eq(const cbox_X* x, const cbox_X* y);      // cbox_X_cmp() == 0

// functions on pointed to objects.

uint64_t    cbox_X_value_hash(const i_val* x);
int         cbox_X_value_cmp(const i_val* x, const i_val* y);
bool        cbox_X_value_eq(const i_val* x, const i_val* y);
```

## Types and constants

| Type name          | Type definition                                               | Used to represent...     |
|:-------------------|:--------------------------------|:------------------------|
| `cbox_NULL`        | `{NULL}`                        | Init nullptr const      |
| `cbox_X`           | `struct { cbox_X_value* get; }` | The cbox type           |
| `cbox_X_value`     | `i_val`                         | The cbox element type   |

## Example

```c
#include <stdio.h>
void int_drop(int* x) {
    printf("\n drop %d", *x);
}

#define i_type IBox
#define i_val int
#define i_valdrop int_drop    // optional func, just to display elements destroyed
#define i_valclone(x) x       // must specified when i_valdrop is defined.
#include <stc/cbox.h>

#define i_type ISet
#define i_keyboxed IBox       // NB: use i_keyboxed instead of i_key
#include <stc/csset.h>        // ISet : std::set<std::unique_ptr<int>>

#define i_type IVec
#define i_valboxed IBox       // NB: use i_valboxed instead of i_val
#include <stc/cvec.h>         // IVec : std::vector<std::unique_ptr<int>>

int main()
{
    IVec vec = c_make(Vec, {2021, 2012, 2022, 2015});
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
