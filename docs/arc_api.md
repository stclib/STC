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

When declaring a container with shared pointers, define `i_key_arcbox` with the arc type, see example.

See similar c++ class [std::shared_ptr](https://en.cppreference.com/w/cpp/memory/shared_ptr) for a functional reference, or Rust [std::sync::Arc](https://doc.rust-lang.org/std/sync/struct.Arc.html) / [std::rc::Rc](https://doc.rust-lang.org/std/rc/struct.Rc.html).

## Header file and declaration

```c
#define i_TYPE <ct>,<kt>   // shorthand to define i_type,i_key
#define i_type <t>         // arc container type name (default: arc_{i_key})
#define i_key <t>          // element type: REQUIRED. Defines arc_X_value
#define i_cmp <f>          // three-way compareison. REQUIRED IF i_key is a non-integral type
                           // Note that containers of arcs will "inherit" i_cmp
                           // when using arc in containers with i_val_arcbox MyArc - ie. the i_type.
#define i_use_cmp          // may be defined instead of i_cmp when i_key is an integral/native-type.
#define i_keydrop <f>      // destroy element func - defaults to empty destruct
#define i_keyclone <f>     // REQUIRED if i_keydrop is defined, unless 'i_opt c_no_clone' is defined.

#define i_keyraw <t>       // convertion type (lookup): default to {i_key}
#define i_keyto <f>        // convertion func i_key* => i_keyraw: REQUIRED IF i_keyraw defined.
#define i_keyfrom <f>      // from-raw func.

#define i_opt c_no_atomic  // Non-atomic reference counting, like Rust Rc.
#define i_tag <s>          // alternative typename: arc_{i_tag}. i_tag defaults to i_key
#include "stc/arc.h"
```
`X` should be replaced by the value of `i_tag` in all of the following documentation.

## Methods
```c
arc_X       arc_X_init();                                   // empty shared pointer
arc_X       arc_X_from(i_keyraw raw);                       // create an arc from raw type (available if i_keyraw defined by user).
arc_X       arc_X_from_ptr(i_key* p);                       // create an arc from raw pointer. Takes ownership of p.
arc_X       arc_X_make(i_key key);                          // create an arc from constructed key object. Faster than from_ptr().

arc_X       arc_X_clone(arc_X other);                       // return other with increased use count
arc_X       arc_X_move(arc_X* self);                        // transfer ownership to receiver; self becomes NULL
void        arc_X_take(arc_X* self, arc_X unowned);         // take ownership of unowned.
void        arc_X_assign(arc_X* self, arc_X other);         // shared assign (increases use count)

void        arc_X_drop(arc_X* self);                        // destruct (decrease use count, free at 0)
long        arc_X_use_count(const arc_X* self);

void        arc_X_reset_to(arc_X* self, i_key* p);          // assign new arc from ptr. Takes ownership of p.

uint64_t    arc_X_hash(const arc_X* x);                     // hash value
int         arc_X_cmp(const arc_X* x, const arc_X* y);      // compares pointer addresses if no `i_cmp` is specified.
                                                            // is defined. Otherwise uses 'i_cmp' or default cmp.
bool        arc_X_eq(const arc_X* x, const arc_X* y);       // arc_X_cmp() == 0

// functions on pointed to objects.

uint64_t    arc_X_value_hash(const i_key* x);
int         arc_X_value_cmp(const i_key* x, const i_key* y);
bool        arc_X_value_eq(const i_key* x, const i_key* y);
```

## Types and constants

| Type name        | Type definition                                   | Used to represent...   |
|:-----------------|:--------------------------------------------------|:-----------------------|
| `arc_null`       | `{0}`                                             | Init nullptr const     |
| `arc_X`          | `struct { arc_X_value* get; long* use_count; }`   | The arc type          |
| `arc_X_value`    | `i_key`                                           | The arc element type  |
| `arc_X_raw`      | `i_keyraw`                                        | Convertion type        |

## Example

```c
// Create two stacks with arcs to maps.
// Demonstrate sharing and cloning of maps.
// Show elements dropped.
#define i_implement
#include "stc/cstr.h"

#define i_type Map
#define i_key_str // i_key: cstr, i_keydrop: cstr_drop, etc..
#define i_val int // year
// override cstr_drop(p) by defining i_keydrop:
#define i_keydrop(p) (printf("  drop name: %s\n", cstr_str(p)), cstr_drop(p))
#include "stc/smap.h"

#define i_type Arc // (atomic) ref. counted pointer
#define i_key Map
#define i_keydrop(p) (printf("drop Arc:\n"), Map_drop(p))
#include "stc/arc.h"

#define i_type Stack
#define i_key_arcbox Arc // Note: use i_key_arcbox for arc or box value types
#include "stc/stack.h"

int main(void)
{
    Stack s1 = {0}, s2 = {0};
    Map *map;

    // POPULATE s1 with shared pointers to Map:
    map = Stack_push(&s1, Arc_make(Map_init()))->get; // push empty map to s1.
    Map_emplace(map, "Joey", 1990);
    Map_emplace(map, "Mary", 1995);
    Map_emplace(map, "Joanna", 1992);

    map = Stack_push(&s1, Arc_make(Map_init()))->get;
    Map_emplace(map, "Rosanna", 2001);
    Map_emplace(map, "Brad", 1999);
    Map_emplace(map, "Jack", 1980);

    // POPULATE s2:
    map = Stack_push(&s2, Arc_make(Map_init()))->get;
    Map_emplace(map, "Steve", 1979);
    Map_emplace(map, "Rick", 1974);
    Map_emplace(map, "Tracy", 2003);

    // Share two Maps from s1 with s2 by cloning(=sharing) the arcs:
    Stack_push(&s2, Arc_clone(s1.data[0]));
    Stack_push(&s2, Arc_clone(s1.data[1]));

    // Deep-copy (does not share) a Map from s1 to s2.
    // s2 will contain two shared and two unshared maps.
    map = Stack_push(&s2, Arc_from(Map_clone(*s1.data[1].get)))->get;

    // Add one more element to the cloned map:
    Map_emplace_or_assign(map, "Cloned", 2022);

    // Add one more element to the shared map:
    Map_emplace_or_assign(s1.data[1].get, "Shared", 2022);

    puts("S1");
    c_foreach (i, Stack, s1) {
        c_forpair (name, year, Map, *i.ref->get)
            printf("  %s:%d", cstr_str(_.name), *_.year);
        puts("");
    }

    puts("S2");
    c_foreach (i, Stack, s2) {
        c_forpair (name, year, Map, *i.ref->get)
            printf("  %s:%d", cstr_str(_.name), *_.year);
        puts("");
    }
    puts("");

    c_drop(Stack, &s1, &s2);
}
```
Output:
```
S1
  Joanna:1992  Joey:1990  Mary:1995
  Brad:1999  Jack:1980  Rosanna:2001  Shared:2022
S2
  Rick:1974  Steve:1979  Tracy:2003
  Joanna:1992  Joey:1990  Mary:1995
  Brad:1999  Jack:1980  Rosanna:2001  Shared:2022
  Brad:1999  Cloned:2022  Jack:1980  Rosanna:2001

drop Arc:
  drop name: Rick
  drop name: Tracy
  drop name: Steve
drop Arc:
  drop name: Cloned
  drop name: Brad
  drop name: Rosanna
  drop name: Jack
drop Arc:
  drop name: Brad
  drop name: Shared
  drop name: Rosanna
  drop name: Jack
drop Arc:
  drop name: Joanna
  drop name: Mary
  drop name: Joey
```
