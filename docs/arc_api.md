# STC [arc](../include/stc/arc.h): Atomic Reference Counted Smart Pointer

**arc** is a smart pointer that retains shared ownership of an object through a pointer.
Several **arc** objects may own the same object. The object is destroyed and its memory
deallocated when the last remaining **arc** owning the object is destroyed with *arc_X_drop()*;

The object is destroyed using *arc_X_drop()*. A **arc** may also own no objects, in which
case it is called empty. The *arc_X_cmp()*, *arc_X_drop()* methods are defined based on
the `i_cmp` and `i_keydrop` macros specified. Use *arc_X_clone(p)* when sharing ownership of
the pointed-to object. Note that an arc set to NULL is consider uninitialized, and
cannot be e.g. cloned or dropped.

All **arc** functions can be called by multiple threads on different instances of **arc** without
additional synchronization even if these instances are copies and share ownership of the same object.
**arc** uses thread-safe atomic reference counting, through the *arc_X_clone()* and *arc_X_drop()* methods.

When declaring a container with shared pointers, define `i_keypro` with the arc type, see example.

See similar c++ class [std::shared_ptr](https://en.cppreference.com/w/cpp/memory/shared_ptr) for a functional reference, or Rust [std::sync::Arc](https://doc.rust-lang.org/std/sync/struct.Arc.html) / [std::rc::Rc](https://doc.rust-lang.org/std/rc/struct.Rc.html).

## Header file and declaration

```c++
#define i_type <ct>,<kt> // shorthand for defining i_type, i_key
#define i_type <t>       // arc container type name
// One of the following:
#define i_key <t>        // key type
#define i_keyclass <t>   // key type, and bind <t>_clone() and <t>_drop() function names
#define i_keypro <t>     // key "pro" type, use for cstr, arc, box types

#define i_use_cmp        // may be defined instead of i_cmp when i_key is an integral/native-type.
#define i_cmp <fn>       // three-way element comparison. If not specified, pointer comparison is used.
                         // Note that containers of arcs will derive i_cmp from the i_key type
                         // when using arc in containers specified with i_keypro <arc-type>.
#define i_less <fn>      // less comparison. Alternative to i_cmp
#define i_eq <fn>        // equality comparison. Implicitly defined with i_cmp, but not i_less.

#define i_keydrop <fn>   // destroy element func - defaults to empty destruct
#define i_keyclone <fn>  // REQUIRED if i_keydrop is defined, unless 'i_opt c_no_clone' is defined.

#define i_keyraw <t>     // convertion type (lookup): defaults to {i_key}
#define i_keytoraw <fn>  // convertion func i_key* => i_keyraw: REQUIRED IF i_keyraw defined.
#define i_keyfrom <fn>   // from-raw func.

#define i_no_atomic      // Non-atomic reference counting, like Rust Rc.
#define i_opt c_no_atomic // Same as above, but can combine other options on one line with |.
#include "stc/arc.h"
```
When defining a container with **arc** elements, specify `#define i_keypro <arc-type>` instead of `i_key`.

In the following, `X` is the value of `i_key` unless `i_type` is defined.

## Methods
```c++
arc_X           arc_X_init();                                   // empty shared pointer
arc_X           arc_X_from(i_keyraw raw);                       // create an arc from raw type (available if i_keyraw defined by user).
arc_X           arc_X_from_ptr(i_key* p);                       // create an arc from raw pointer. Takes ownership of p.
arc_X           arc_X_make(i_key key);                          // create an arc from constructed key object. Faster than from_ptr().

arc_X           arc_X_clone(arc_X other);                       // return other with increased use count
void            arc_X_assign(arc_X* self, const arc_X* other);  // shared assign (increases use count)
void            arc_X_take(arc_X* self, arc_X unowned);         // take ownership of unowned.
arc_X           arc_X_move(arc_X* self);                        // transfer ownership to receiver; self becomes NULL
void            arc_X_drop(arc_X* self);                        // destruct (decrease use count, free at 0)

long            arc_X_use_count(const arc_X* self);
void            arc_X_reset_to(arc_X* self, i_key* p);          // assign new arc from ptr. Takes ownership of p.

size_t          arc_X_hash(const arc_X* x);                     // hash value
int             arc_X_cmp(const arc_X* x, const arc_X* y);      // compares pointer addresses if no `i_cmp` is specified
                                                                // is defined. Otherwise uses 'i_cmp' or default cmp.
bool            arc_X_eq(const arc_X* x, const arc_X* y);       // arc_X_cmp() == 0

// functions on pointed to objects.

size_t          arc_X_value_hash(const i_key* x);
int             arc_X_value_cmp(const i_key* x, const i_key* y);
bool            arc_X_value_eq(const i_key* x, const i_key* y);
```

## Types and constants

| Type name        | Type definition                                   | Used to represent...   |
|:-----------------|:--------------------------------------------------|:-----------------------|
| `arc_null`       | `{0}`                                             | Init nullptr const     |
| `arc_X`          | `struct { arc_X_value* get; long* use_count; }`   | The arc type          |
| `arc_X_value`    | `i_key`                                           | The arc element type  |
| `arc_X_raw`      | `i_keyraw`                                        | Convertion type        |

## Example

[ [Run this code](https://godbolt.org/z/jEf4oqzen) ]
<!--{%raw%}-->
```c++
// Create two stacks with arcs to maps.
// Demonstrate sharing and cloning of maps.
// Show elements dropped.
#include "stc/cstr.h"

#define i_type Map
#define i_keypro cstr  // cstr is a "pro" type:
#define i_val int      // year
#define i_keydrop(p) (printf("  drop name: %s\n", cstr_str(p)), cstr_drop(p))
#include "stc/smap.h"

#define i_type Arc     // (atomic) ref. counted pointer
#define i_keyclass Map // bind _clone & _drop
#include "stc/arc.h"   // try to switch to box.h!

#define i_type Stack
#define i_keypro Arc // Note: use i_keypro for arc key types
#include "stc/stack.h"

int main(void)
{
    Stack s1 = {0}, s2 = {0};
    Map *map;

    // POPULATE s1 with shared pointers to Map:
    map = Stack_push(&s1, Arc_from(Map_init()))->get; // push empty map to s1.
    c_push_items(Map, map, {{"Joey", 1990}, {"Mary", 1995},
                            {"Mary", 1996}, {"Joanna", 1992}});

    map = Stack_emplace(&s1, Map_init())->get; // emplace will make the Arc
    c_push_items(Map, map, {{"Rosanna", 2001}, {"Brad", 1999}, {"Jack", 1980}});

    // POPULATE s2:
    map = Stack_emplace(&s2, Map_init())->get;
    c_push_items(Map, map, {{"Steve", 1979}, {"Rick", 1974}, {"Tracy", 2003}});


    // Share Arc 1 from s1 with s2 by cloning(=sharing):
    Stack_push(&s2, Arc_clone(s1.data[1]));

    puts("S2 is now");
    for (c_each(i, Stack, s2)) {
        Map m = Stack_value_toraw(i.ref);
        for (c_each_kv(name, year, Map, m))
            printf("  %s:%d\n", cstr_str(name), *year);
        puts("");
    }

    c_drop(Stack, &s1, &s2);
}
```
<!--{%endraw%}-->