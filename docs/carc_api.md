# STC [carc](../include/stc/carc.h): Atomic Reference Counted Smart Pointer

**carc** is a smart pointer that retains shared ownership of an object through a pointer.
Several **carc** objects may own the same object. The object is destroyed and its memory
deallocated when the last remaining **carc** owning the object is destroyed with *carc_X_drop()*;

The object is destroyed using *carc_X_drop()*. A **carc** may also own no objects, in which 
case it is called empty. The *carc_X_cmp()*, *carc_X_drop()* methods are defined based on
the `i_cmp` and `i_keydrop` macros specified. Use *carc_X_clone(p)* when sharing ownership of
the pointed-to object. 

All **carc** functions can be called by multiple threads on different instances of **carc** without
additional synchronization even if these instances are copies and share ownership of the same object.
**carc** uses thread-safe atomic reference counting, through the *carc_X_clone()* and *carc_X_drop()* methods.

When declaring a container with shared pointers, define `i_keyboxed` with the carc type, see example.

See similar c++ class [std::shared_ptr](https://en.cppreference.com/w/cpp/memory/shared_ptr) for a functional reference, or Rust [std::sync::Arc](https://doc.rust-lang.org/std/sync/struct.Arc.html) / [std::rc::Rc](https://doc.rust-lang.org/std/rc/struct.Rc.html).

## Header file and declaration

```c
#define i_key <t>          // element type: REQUIRED. Note: i_val* may be specified instead of i_key*.
#define i_type <t>         // carc container type name
#define i_cmp <f>          // three-way compareison. REQUIRED IF i_key is a non-integral type
                           // Note that containers of carcs will "inherit" i_cmp
                           // when using carc in containers with i_valboxed MyArc - ie. the i_type.
#define i_use_cmp          // define instead of i_cmp only when i_key is an integral/native-type.
#define i_keydrop <f>      // destroy element func - defaults to empty destruct
#define i_keyclone <f>     // REQUIRED if i_keydrop is defined, unless 'i_opt c_no_clone' is defined.

#define i_keyraw <t>       // convertion type (lookup): default to {i_key}
#define i_keyto <f>        // convertion func i_key* => i_keyraw: REQUIRED IF i_keyraw defined.
#define i_keyfrom <f>      // from-raw func.

#define i_opt c_no_atomic  // Non-atomic reference counting, like Rust Rc.
#define i_tag <s>          // alternative typename: carc_{i_tag}. i_tag defaults to i_key
#include <stc/carc.h>
```
`X` should be replaced by the value of `i_tag` in all of the following documentation.

## Methods
```c
carc_X      carc_X_init();                                     // empty shared pointer
carc_X      carc_X_from(i_keyraw raw);                         // create an carc from raw type (available if i_keyraw defined by user).
carc_X      carc_X_from_ptr(i_key* p);                         // create an carc from raw pointer. Takes ownership of p.
carc_X      carc_X_make(i_key key);                            // create an carc from constructed key object. Faster than from_ptr().

carc_X      carc_X_clone(carc_X other);                        // return other with increased use count
carc_X      carc_X_move(carc_X* self);                         // transfer ownership to receiver; self becomes NULL
void        carc_X_take(carc_X* self, carc_X unowned);         // take ownership of unowned.
void        carc_X_assign(carc_X* self, carc_X other);         // shared assign (increases use count)

void        carc_X_drop(carc_X* self);                         // destruct (decrease use count, free at 0)
long        carc_X_use_count(const carc_X* self);    

void        carc_X_reset(carc_X* self);    
void        carc_X_reset_to(carc_X* self, i_key* p);           // assign new carc from ptr. Takes ownership of p.

uint64_t    carc_X_hash(const carc_X* x);                      // hash value
int         carc_X_cmp(const carc_X* x, const carc_X* y);      // compares pointer addresses if no `i_cmp` is specified.
                                                               // is defined. Otherwise uses 'i_cmp' or default cmp.
bool        carc_X_eq(const carc_X* x, const carc_X* y);       // carc_X_cmp() == 0

// functions on pointed to objects.

uint64_t    carc_X_value_hash(const i_key* x);
int         carc_X_value_cmp(const i_key* x, const i_key* y);
bool        carc_X_value_eq(const i_key* x, const i_key* y);
```

## Types and constants

| Type name         | Type definition                                   | Used to represent...   |
|:------------------|:--------------------------------------------------|:-----------------------|
| `carc_null`       | `{0}`                                             | Init nullptr const     |
| `carc_X`          | `struct { carc_X_value* get; long* use_count; }`  | The carc type          |
| `carc_X_value`    | `i_key`                                           | The carc element type  |
| `carc_X_raw`      | `i_keyraw`                                        | Convertion type        |

## Example

```c
// Create two stacks with carcs to maps.
// Demonstrate sharing and cloning of maps.
// Show elements dropped.
#define i_implement
#include <stc/cstr.h>

#define i_type Map
#define i_key_str // i_key: cstr, i_keydrop: cstr_drop, etc..
#define i_val int // year
// override cstr_drop(p) by defining i_keydrop:
#define i_keydrop(p) (printf("  drop name: %s\n", cstr_str(p)), cstr_drop(p))
#include <stc/csmap.h>

#define i_type Arc // (atomic) ref. counted pointer
#define i_key Map
#define i_keydrop(p) (printf("drop Arc:\n"), Map_drop(p))
#include <stc/carc.h>

#define i_type Stack
#define i_keyboxed Arc // Note: use i_keyboxed for carc or cbox value types
#include <stc/cstack.h>

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

    // Share two Maps from s1 with s2 by cloning(=sharing) the carcs:
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
