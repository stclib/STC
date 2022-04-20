# STC [carc](../include/stc/carc.h): Atomic Reference Counted Smart Pointer

**carc** is a smart pointer that retains shared ownership of an object through a pointer.
Several **carc** objects may own the same object. The object is destroyed and its memory
deallocated when the last remaining **carc** owning the object is destroyed with *carc_X_drop()*;

The object is destroyed using *carc_X_drop()*. A **carc** may also own no objects, in which 
case it is called empty. The *carc_X_cmp()*, *carc_X_drop()* methods are defined based on
the `i_cmp` and `i_valdrop` macros specified. Use *carc_X_clone(p)* when sharing ownership of
the pointed-to object. 

All **carc** functions can be called by multiple threads on different instances of **carc** without
additional synchronization even if these instances are copies and share ownership of the same object.
**carc** uses thread-safe atomic reference counting, through the *carc_X_clone()* and *carc_X_drop()* methods.

When declaring a container with shared pointers, define `i_val_bind` as the carc type, see example.

See similar c++ class [std::shared_ptr](https://en.cppreference.com/w/cpp/memory/shared_ptr) for a functional reference, or Rust [std::sync::Arc](https://doc.rust-lang.org/std/sync/struct.Arc.html) / [std::rc::Rc](https://doc.rust-lang.org/std/rc/struct.Rc.html).

## Header file and declaration

```c
#define i_val             // value: REQUIRED
#define i_cmp             // three-way compare two i_val* : REQUIRED IF i_val is a non-integral type
#define i_valdrop         // destroy value func - defaults to empty destruct
#define i_tag             // defaults to i_val
#define i_opt c_no_atomic // Non-atomic reference counting, like Rust Rc.
#include <stc/carc.h>
```
`X` should be replaced by the value of `i_tag` in all of the following documentation.

## Methods
```c
carc_X      carc_X_init();                                     // empty shared pointer
carc_X      carc_X_make(i_valraw raw);                         // create owned value from raw type, if defined.
carc_X      carc_X_from(i_val val);                            // create new heap allocated object. Take ownership of val.
carc_X      carc_X_from_ptr(i_val* p);                         // create a carc from raw pointer. Takes ownership of p.

carc_X      carc_X_clone(carc_X other);                        // return other with increased use count
carc_X      carc_X_move(carc_X* self);                         // transfer ownership to another carc.
void        carc_X_take(carc_X* self, carc_X other);           // take ownership of other.
void        carc_X_copy(carc_X* self, carc_X other);           // copy shared (increase use count)

void        carc_X_drop(carc_X* self);                         // destruct (decrease use count, free at 0)
long        carc_X_use_count(carc_X ptr);    

void        carc_X_reset(carc_X* self);    
void        carc_X_reset_from(carc_X* self, i_val val);        // assign new carc with value. Takes ownership of val.

uint64_t    carc_X_value_hash(const i_val* x, size_t n);       // hash value
int         carc_X_value_cmp(const i_val* x, const i_val* y);  // compares pointer addresses if 'i_opt c_no_cmp'
                                                               // is defined. Otherwise uses 'i_cmp' or default compare.
bool        carc_X_value_eq(const i_val* x, const i_val* y);   // cbox_X_value_cmp == 0
```

## Types and constants

| Type name         | Type definition                                   | Used to represent...   |
|:------------------|:--------------------------------------------------|:-----------------------|
| `carc_null`       | `{NULL, NULL}`                                    | Init nullptr const     |
| `carc_X`          | `struct { carc_X_value* get; long* use_count; }`  | The carc type          |
| `carc_X_value`    | `i_val`                                           | The carc element type  |
| `carc_X_raw`      | `i_valraw`                                        | Convertion type        |

## Example

```c
// Create a stack and a list of shared pointers to maps,
// and demonstrate sharing and cloning of maps.
#define i_type Map
#define i_key_str // strings
#define i_val int
#define i_keydrop(p) (printf("  drop name: %s\n", (p)->str), cstr_drop(p))
#include <stc/csmap.h>

#define i_type Arc // (atomic) ref. counted type
#define i_val Map
#define i_valfrom Map_clone
#define i_valdrop(p) (printf("drop Arc:\n"), Map_drop(p))
// no comparison of Maps needed (or available), and
// no need for atomic ref. count in single thread:
#define i_opt c_no_cmp|c_no_atomic 
#include <stc/carc.h>

#define i_type Stack
#define i_val_arcbox Arc // note: define i_val_arcbox for carc/cbox value
#include <stc/cstack.h>

#define i_type List
#define i_val_arcbox Arc // as above
#include <stc/clist.h>

int main()
{
    c_auto (Stack, stack)
    c_auto (List, list)
    {
        // POPULATE the stack with shared pointers to Map:
        Map *map;
        map = Stack_push(&stack, Arc_from(Map_init()))->get;
        c_apply(v, Map_emplace(map, c_pair(v)), Map_raw, {
            {"Joey", 1990},
            {"Mary", 1995},
            {"Joanna", 1992}
        });
        map = Stack_push(&stack, Arc_from(Map_init()))->get;
        c_apply(v, Map_emplace(map, c_pair(v)), Map_raw, {
            {"Rosanna", 2001},
            {"Brad", 1999},
            {"Jack", 1980}
        });

        // POPULATE the list:
        map = List_push_back(&list, Arc_from(Map_init()))->get;
        c_apply(v, Map_emplace(map, c_pair(v)), Map_raw, {
            {"Steve", 1979},
            {"Rick", 1974},
            {"Tracy", 2003}
        });
        
        // Share two Maps from the stack with the list by cloning(=sharing) the carc:
        List_push_back(&list, Arc_clone(stack.data[0]));
        List_push_back(&list, Arc_clone(stack.data[1]));
        
        // Deep-copy (not share) a Map from the stack to the list
        // List will contain two shared and two unshared maps.
        map = List_push_back(&list, Arc_from(Map_clone(*stack.data[1].get)))->get;
        
        // Add one more element to the cloned map:
        Map_emplace_or_assign(map, "Cloned", 2022);

        // Add one more element to the shared map:
        Map_emplace_or_assign(stack.data[1].get, "Shared", 2022);

        puts("STACKS");
        c_foreach (i, Stack, stack) {
            c_forpair (name, year, Map, *i.ref->get)
                printf("  %s:%d", _.name.str, _.year);
            puts("");
        }

        puts("LIST");
        c_foreach (i, List, list) {
            c_forpair (name, year, Map, *i.ref->get)
                printf("  %s:%d", _.name.str, _.year);
            puts("");
        }
    }
}
```
Output:
```
STACKS
  Joanna:1992  Joey:1990  Mary:1995
  Brad:1999  Jack:1980  Rosanna:2001  Shared:2022
LIST
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
