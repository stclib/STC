# STC [cbox](../include/stc/cbox.h): (Boxed) Heap Allocated Objects

**cbox** is a A box is a smart pointer to a heap allocated value of type X. A **cbox** can
be empty. The *cbox_X_compare()*, *cbox_X_del()* methods are defined based on the `i_cmp`
and `i_valdel` macros specified. Use *cbox_X_clone(p)* to make a deep copy, which uses the
`i_valfrom` macro if defined.

When declaring a container of **cbox** values, it is recommended to define `i_val_ref` to the
cbox type instead of defining `i_val`. This will auto-set `i_del`, `i_from`, and `i_cmp` using 
functions defined by the specified **cbox**.

For containers, make sure to pass the result of create functions like *cbox_X_new()* **only** to 
*insert()*, *push_back()*, and *push()* functions. Use the *emplace()* functions in order to
auto-*clone* an already existing/owned cbox element.

See similar c++ class [std::unique_ptr](https://en.cppreference.com/w/cpp/memory/unique_ptr) for a functional reference, or Rust [std::boxed::Box](https://doc.rust-lang.org/std/boxed/struct.Box.html)

## Header file and declaration

```c
#define i_val           // value: REQUIRED
#define i_cmp           // three-way compare two i_val* : REQUIRED IF i_val is a non-integral type
#define i_del           // destroy value func - defaults to empty destruct
#define i_from          // create from raw/clone func - REQUIRED if i_del is defined,
                        // unless 'i_opt c_no_clone' is defined.
#define i_tag           // type name tag, defaults to i_val
#include <stc/cbox.h>    
```
`X` should be replaced by the value of `i_tag` in all of the following documentation.
Define `i_opt` with `c_no_compare` if comparison between i_val's is not needed/available. Will then
compare the pointer addresses when used. Additionally, `c_no_clone` or `i_is_fwd` may be defined.

## Methods
```c
cbox_X      cbox_X_init();                                    // return an empty cbox
cbox_X      cbox_X_new(i_val val);                            // allocate new heap object with val. Take ownership of val.
cbox_X      cbox_X_from(i_rawval raw);                        // like cbox_X_new(), but create owned value from raw.
cbox_X      cbox_X_with(i_val* p);                            // create a cbox from a pointer. Takes ownership of p.

cbox_X      cbox_X_clone(cbox_X other);                       // return deep copied clone
cbox_X      cbox_X_move(cbox_X* self);                        // transfer ownership to another cbox.
void        cbox_X_take(cbox_X* self, cbox_X other);          // take ownership of other.
void        cbox_X_copy(cbox_X* self, cbox_X other);          // deep copy to self

void        cbox_X_del(cbox_X* self);                         // destruct the contained object and free's it.

void        cbox_X_reset(cbox_X* self);   
void        cbox_X_reset_new(cbox_X* self, i_val val);        // assign new cbox with value. Takes ownership of val.
void        cbox_X_reset_from(cbox_X* self, i_rawval raw);    // make and assign new cbox from raw value. 
void        cbox_X_reset_with(cbox_X* self, i_val* p);        // create cbox with pointer p. Takes ownership of p.

int         cbox_X_compare(const cbox_X* x, const cbox_X* y); // compares pointer addresses if 'i_opt c_no_compare'
                                                              // is defined. Otherwise uses 'i_cmp' or default compare.
```
## Types and constants

| Type name          | Type definition                                               | Used to represent...     |
|:-------------------|:--------------------------------|:------------------------|
| `cbox_null`        | `{NULL}`                        | Init nullptr const      |
| `cbox_X`           | `struct { cbox_X_value* get; }` | The cbox type           |
| `cbox_X_value`     | `i_val`                         | The cbox element type   |

## Example

```c
#include <stdio.h>
#include <string.h>

void int_del(int* x) {
    printf("del: %d\n", *x);
}

// When 'i_del' is defined, you are also forced to define a clone function with
// 'i_valfrom', as it is normally required when i_del destroys resources.
//
// If cloning is not needed, define 'i_opt c_no_clone' instead of 'i_valfrom'
// both for the cbox type and the container of cbox elements. It will also 
// disable emplace container functions.
//
// This applies to all container types, except those with csptr elements, as they
// define cloning internally.

#define i_val int
#define i_del int_del       // optional func, just to display elements destroyed
#define i_valfrom c_default_clone
#include <stc/cbox.h>       // cbox_int

#define i_key_ref cbox_int  // note: use i_key_ref instead of i_key
#define i_tag int           // tag otherwise defaults to 'ref'
#include <stc/csset.h>      // csset_int (like: std::set<std::unique_ptr<int>>)

#define i_val_ref cbox_int  // note: use i_val_ref instead of i_val
#define i_tag int           // tag otherwise defaults to 'ref'
#include <stc/cvec.h>       // cvec_int (like: std::vector<std::unique_ptr<int>>)

int main()
{
    c_auto (cvec_int, vec)   // declare and init vec, call del at scope exit
    c_auto (csset_int, set)  // declare and init set, call del at scope exit
    {
        c_apply(cvec_int, push_back, &vec, {
            cbox_int_new(2021),
            cbox_int_new(2012),
            cbox_int_new(2022),
            cbox_int_new(2015),
        });
        printf("vec:");
        c_foreach (i, cvec_int, vec) printf(" %d", *i.ref->get);
        puts("");

        // add odd numbers from vec to set
        c_foreach (i, cvec_int, vec)
            if (*i.ref->get & 1)
                csset_int_emplace(&set, *i.ref); // deep copy (clones) *i.ref object

        // erase the two last elements in vec
        cvec_int_pop_back(&vec);
        cvec_int_pop_back(&vec);

        printf("vec:");
        c_foreach (i, cvec_int, vec) printf(" %d", *i.ref->get);

        printf("\nset:");
        c_foreach (i, csset_int, set) printf(" %d", *i.ref->get);

        puts("\nDone");
    }
}
```
Output:
```
vec: 2021 2012 2022 2015
del: 2015
del: 2022
vec: 2021 2012
set: 2015 2021
Done
del: 2021
del: 2015
del: 2021
del: 2012
```
