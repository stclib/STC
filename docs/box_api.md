# STC [box](../include/stc/box.h): Smart Pointer (boxed object)

**box** is a smart pointer to a heap allocated value of type X. A **box** can
be empty. The *box_X_cmp()*, *box_X_drop()* methods are defined based on the `i_cmp`
and `i_keydrop` macros specified. Use *box_X_clone(p)* to make a deep copy, which uses the
`i_keyclone` macro if defined. Note that a box set to NULL is consider uninitialized, and
cannot be e.g. cloned or dropped.

When declaring a container of **box** elements, define `i_keypro` with the
box type instead of defining `i_key`. This will auto-define `i_keydrop`, `i_keyclone`, and `i_cmp` using
functions defined by the specified **box**.

See similar c++ class [std::unique_ptr](https://en.cppreference.com/w/cpp/memory/unique_ptr) for a functional reference, or Rust [std::boxed::Box](https://doc.rust-lang.org/std/boxed/struct.Box.html)

## Header file and declaration

```c++
#define T <ct>, <kt>[, (<opt>)] // shorthand for defining box name, i_key, and i_opt
// Common <opt> traits:
//   c_keycomp  - Key type <kt> is a comparable;
//                Binds <kt>_cmp(), <kt>_hash() "member" function names.
//   c_keyclass - Additionally binds <kt>_clone() and <kt>_drop() function names.
//                All containers used as keys themselves can be specified with the c_keyclass trait.
//   c_keypro   - "Pro" key type, use e.g. for built-in `cstr`, `zsview`, `arc`, and `box` as keys.
//                These support conversion to/from a "raw" input type (such as const char*) when
//                using <ct>_emplace*() functions, and may do optimized lookups via the raw type.
//   c_use_cmp  - Enable comparison
//
// To apply multiple traits, specify e.g. (c_keyclass | c_use_cmp) as <opt>.

// Alternative to defining T:
#define i_key <kt> // define key type. container type name <ct> defaults to box_<kt>.

// Override/define when not the <opt> traits are specified:
#define i_keydrop <fn>   // Destroy-element function - defaults to empty destruct
#define i_keyclone <fn>  // Required if i_keydrop is defined
#define i_cmp <fn>       // Three-way compare two i_keyraw*
#define i_less <fn>      // Less comparison. Alternative to i_cmp
#define i_eq <fn>        // Equality comparison. Implicitly defined with i_cmp, but not with i_less.

#include <stc/box.h>
```
In the following, `X` is the value of `i_key` unless `T` is defined.
Unless `c_use_cmp` is defined, comparison between i_key's is not needed/available. Will then
compare the pointer addresses when used. Additionally, `c_no_clone` or `i_is_fwd` may be defined.

## Methods
```c++
box_X           box_X_init();                                   // return an empty box
box_X           box_X_from(i_keyraw raw);                       // create a box from raw type. Avail if i_keyraw user defined.
box_X           box_X_from_ptr(i_key* ptr);                     // create a box from a pointer. Takes ownership of ptr.
box_X           box_X_make(i_key val);                          // create a box from unowned val object.

box_X           box_X_clone(box_X other);                       // return deep copied clone
void            box_X_assign(box_X* self, box_X* other);        // transfer ownership from other to self; other set to NULL.
void            box_X_take(box_X* self, box_X unowned);         // take ownership of unowned box object.
box_X           box_X_move(box_X* self);                        // transfer ownership to receiving box. self set to NULL.
i_key*          box_X_release(box_X* self);                     // release owned pointer; must be freed by receiver. self set NULL.
void            box_X_drop(const box_X* self);                  // destruct the contained object and free its heap memory.

void            box_X_reset_to(box_X* self, i_key* ptr);        // assign ptr, and take ownership of ptr.

size_t          box_X_hash(const box_X* x);                     // hash value
int             box_X_cmp(const box_X* x, const box_X* y);      // compares pointer addresses if no `i_cmp` is specified
                                                                // is defined. Otherwise uses 'i_cmp' or default cmp.
bool            box_X_eq(const box_X* x, const box_X* y);       // box_X_cmp() == 0

// functions on pointed to objects.

size_t          box_X_value_hash(const i_key* x);
int             box_X_value_cmp(const i_key* x, const i_key* y);
bool            box_X_value_eq(const i_key* x, const i_key* y);
```

## Types and constants

| Type name         | Type definition                 | Used to represent...     |
|:------------------|:--------------------------------|:-----------------------|
| `box_null`        | `{0}`                           | Init nullptr const     |
| `box_X`           | `struct { box_X_value* get; }`  | The box type           |
| `box_X_value`     | `i_key`                         | The box element type   |

## Example
Create a vec and a set with owned pointers to int elements, using box.

[ [Run this code](https://godbolt.org/z/61rcjcz1x) ]
```c++
#include <stdio.h>

void int_drop(int* x) {
    printf("\n drop %d", *x);
}

#define T IBox, int, (c_use_cmp) // defines _cmp(), _eq(), and _hash() box-functions
#define i_keydrop int_drop  // just to display elements destroyed
#define i_keyclone(x) x     // must be specified when i_keydrop is defined.
#include <stc/box.h>

// ISet : std::set<std::unique_ptr<int>>
#define T ISet, IBox, (c_keypro) // box is a "pro"-type
#include <stc/sortedset.h>

// IVec : std::vector<std::unique_ptr<int>>
#define T IVec, IBox, (c_keypro)
#include <stc/vec.h>


int main(void)
{
    IVec vec = c_make(IVec, {2021, 2012, 2022, 2015});
    ISet set = {0};

    printf("vec:");
    for (c_each(i, IVec, vec))
        printf(" %d", *i.ref->get);

    // add odd numbers from vec to set
    for (c_each(i, IVec, vec)) {
        if (*i.ref->get & 1) {
            ISet_insert(&set, IBox_clone(*i.ref));
        }
    }
    // pop the two last elements in vec
    IVec_pop(&vec);
    IVec_pop(&vec);
    printf("\nvec:");
    for (c_each(i, IVec, vec))
        printf(" %d", *i.ref->get);

    printf("\nset:");
    for (c_each(i, ISet, set))
        printf(" %d", *i.ref->get);

    IVec_drop(&vec);
    ISet_drop(&set);
}
```
