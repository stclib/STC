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

- ***arc type 1*** is the default arc type. It occupies the size of only one pointer, and it can convert
a raw pointer gotten from an arc back to an arc with the *X_toarc()* function. Conceptually, the arc which
passed the raw pointer should be ***moved*** (or not dropped), e.g.:
```c++
Arc a1 = Arc_make(value);
Value* vp = Arc_move(&a1).get; // a1 will now be NULL
Arc a2 = Arc_toarc(vp);
c_drop(Arc, &a1, &a2);
```
- ***arc type 2*** occupies the size of two pointers, however it may also be constructed from a existing unmanaged
pointer. Enable it by `#define T Arc, key, (c_arc2)`
- When defining a container with shared pointer elements, add *c_keypro*/*valpro*: `#define T MyVec, MyArc, (c_keypro)`

See similar c++ class [std::shared_ptr](https://en.cppreference.com/w/cpp/memory/shared_ptr) for a functional reference, or Rust [std::sync::Arc](https://doc.rust-lang.org/std/sync/struct.Arc.html) / [std::rc::Rc](https://doc.rust-lang.org/std/rc/struct.Rc.html).

## Header file and declaration

```c++
#define T <ct>,<kt>[,<op>] // shorthand
#define T <ct>           // arc container type name
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

#define i_keyraw <t>     // conversion type (lookup): defaults to {i_key}
#define i_keytoraw <fn>  // conversion func i_key* => i_keyraw: REQUIRED IF i_keyraw defined.
#define i_keyfrom <fn>   // from-raw func.

#define i_no_atomic      // Non-atomic reference counting, like Rust Rc.
#define i_opt c_no_atomic // Same as above, but can combine other options on one line with |.
#include <stc/arc.h>
```
When defining a container with **arc** elements, specify `#define i_keypro <arc-type>` instead of `i_key`.

In the following, `X` is the value of `i_key` unless `T` is defined.

## Methods
```c++
arc_X           arc_X_init();                                   // empty shared pointer
arc_X           arc_X_from(i_keyraw raw);                       // create an arc from raw type (available if i_keyraw defined by user).
arc_X           arc2_X_from_ptr(i_key* ptr);                    // NB! arc2 only: create an arc from raw pointer. Takes ownership of p.
arc_X           arc_X_make(i_key key);                          // create an arc from constructed key object. Faster than from_ptr().

arc_X           arc_X_clone(arc_X other);                       // return other with increased use count
void            arc_X_assign(arc_X* self, const arc_X* other);  // shared assign (increases use count)
void            arc_X_take(arc_X* self, arc_X unowned);         // take ownership of unowned.
arc_X           arc_X_move(arc_X* self);                        // transfer ownership to receiver; self becomes NULL
arc_X           arc1_X_toarc(i_key* ptr);                       // NB! arc1 only: convert raw pointer previously created by arc back to arc.
void            arc_X_drop(const arc_X* self);                  // destruct (decrease use count, free at 0)

long            arc_X_use_count(arc_X arc);
void            arc2_X_reset_to(arc_X* self, i_key* ptr);       // NB! arc2 only: assign new arc from ptr. Takes ownership of p.

size_t          arc_X_hash(const arc_X* self);                  // hash value
int             arc_X_cmp(const arc_X* x, const arc_X* y);      // compares pointer addresses if no `i_cmp` is specified
                                                                // is defined. Otherwise uses 'i_cmp' or default cmp.
bool            arc_X_eq(const arc_X* x, const arc_X* y);       // arc_X_cmp() == 0

// functions on pointed to objects.

size_t          arc_X_value_hash(const i_key* self);
int             arc_X_value_cmp(const i_key* x, const i_key* y);
bool            arc_X_value_eq(const i_key* x, const i_key* y);
```

## Types and constants

| Type name        | Type definition                                   | Used to represent...   |
|:-----------------|:--------------------------------------------------|:-----------------------|
| `arc_null`       | `{0}`                                             | Init nullptr const     |
| `arc_X`          | `union { arc_X_value* get; }`                     | The arc type          |
| `arc_X_value`    | `i_key`                                           | The arc element type  |
| `arc_X_raw`      | `i_keyraw`                                        | Convertion type        |

## Example

[ [Run this code](https://godbolt.org/z/TvcoxG1fz) ]
<!--{%raw%}-->
```c++
// Create two stacks with arcs to maps.
// Demonstrate sharing and cloning of maps.
// Show elements dropped.
#include <stc/cstr.h>

#define T Map, cstr, int, (c_keypro) // cstr is a "pro" type
#define i_keydrop(p) (printf("  drop name: %s\n", cstr_str(p)), cstr_drop(p))
#include <stc/sortedmap.h>

// keyclass binds _clone & _drop:
#define T Arc, Map, (c_keyclass) // (Atomic) Ref. Counted pointer
#include <stc/arc.h>   // try to switch to box.h!

#define T Stack, Arc, (c_keypro) // arc is "pro"
#include <stc/stack.h>

int main(void)
{
    Stack s1 = {0}, s2 = {0};
    Map *map;

    // POPULATE s1 with shared pointers to Map:
    map = Stack_emplace(&s1, Map_init())->get; // emplace empty map on s1.
    c_put_items(Map, map, {{"Joey", 1990}, {"Mary", 1995},
                           {"Mary", 1996}, {"Joanna", 1992}});

    map = Stack_emplace(&s1, Map_init())->get; // emplace will make the Arc
    c_put_items(Map, map, {{"Rosanna", 2001}, {"Brad", 1999}, {"Jack", 1980}});

    // POPULATE s2:
    map = Stack_emplace(&s2, Map_init())->get;
    c_put_items(Map, map, {{"Steve", 1979}, {"Rick", 1974}, {"Tracy", 2003}});


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
