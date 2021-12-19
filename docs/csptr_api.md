# STC [csptr](../include/stc/csptr.h): Atomic Reference Counted

**csptr** is a smart pointer that retains shared ownership of an object through a pointer.
Several **csptr** objects may own the same object. The object is destroyed and its memory
deallocated when the last remaining **csptr** owning the object is destroyed with *csptr_X_drop()*;

The object is destroyed using *csptr_X_drop()*. A **csptr** may also own no objects, in which 
case it is called empty. The *csptr_X_cmp()*, *csptr_X_drop()* methods are defined based on
the `i_cmp` and `i_valdrop` macros specified. Use *csptr_X_clone(p)* when sharing ownership of
the pointed-to object. 

All **csptr** functions can be called by multiple threads on different instances of **csptr** without
additional synchronization even if these instances are copies and share ownership of the same object.
**csptr** uses thread-safe atomic reference counting, through the *csptr_X_clone()* and *csptr_X_drop()* methods.

When declaring a container with shared pointers, define `i_val_bind` as the csptr type, see example.

For containers, make sure to pass the result of create functions *csptr_X_new()* **only** to *insert()*,
*push_back()*, and *push()* functions. Use *emplace()* method for sharing existing **csptr**s between
containers or other existing shared pointers, as they internally clone/share the input.

See similar c++ class [std::shared_ptr](https://en.cppreference.com/w/cpp/memory/shared_ptr) for a functional reference, or Rust [std::sync::Arc](https://doc.rust-lang.org/std/sync/struct.Arc.html) / [std::rc::Rc](https://doc.rust-lang.org/std/rc/struct.Rc.html).

## Header file and declaration

```c
#define i_val             // value: REQUIRED
#define i_cmp             // three-way compare two i_val* : REQUIRED IF i_val is a non-integral type
#define i_drop            // destroy value func - defaults to empty destruct
#define i_tag             // defaults to i_val
#define i_opt c_no_atomic // Non-atomic reference counting, like Rust Rc.
#include <stc/csptr.h>
```
`X` should be replaced by the value of `i_tag` in all of the following documentation.

## Methods
```c
csptr_X      csptr_X_init();                                     // empty shared pointer
csptr_X      csptr_X_new(i_val val);                             // create new heap allocated object. Take ownership of val.
csptr_X      csptr_X_from(i_valraw raw);                         // like csptr_X_new(), but construct owned value from raw.
csptr_X      csptr_X_with(i_val* p);                             // create a csptr from raw pointer. Takes ownership of p.
    
csptr_X      csptr_X_clone(csptr_X other);                        // return other with increased use count
csptr_X      csptr_X_move(csptr_X* self);                         // transfer ownership to another csptr.
void        csptr_X_take(csptr_X* self, csptr_X other);           // take ownership of other.
void        csptr_X_copy(csptr_X* self, csptr_X other);           // copy shared (increase use count)
    
void        csptr_X_drop(csptr_X* self);                         // destruct (decrease use count, free at 0)
long        csptr_X_use_count(csptr_X ptr);    
    
void        csptr_X_reset(csptr_X* self);    
void        csptr_X_reset_new(csptr_X* self, i_val val);         // assign new csptr with value. Takes ownership of val.
void        csptr_X_reset_from(csptr_X* self, i_valraw raw);     // make and assign new csptr from raw value. 
void        csptr_X_reset_with(csptr_X* self, i_val* p);         // create csptr with pointer p. Takes ownership of p.

int         csptr_X_cmp(const csptr_X* x, const csptr_X* y);      // compares pointer addresses if 'i_opt c_no_cmp'
                                                               // is defined. Otherwise uses 'i_cmp' or default compare.
```

## Types and constants

| Type name          | Type definition                                   | Used to represent...    |
|:-------------------|:--------------------------------------------------|:------------------------|
| `csptr_null`        | `{NULL, NULL}`                                    | Init nullptr const      |
| `csptr_X`           | `struct { csptr_X_value* get; long* use_count; }`  | The csptr type           |
| `csptr_X_value`     | `i_val`                                           | The csptr element type   |

## Example

```c
// Create a stack and a list of shared pointers to maps,
// and demonstrate sharing and cloning of maps.
#define i_type Map
#define i_key_str // strings
#define i_val int
#define i_keydrop(p) (printf("drop name: %s\n", (p)->str), cstr_drop(p))
#include <stc/csmap.h>

#define i_type Arc // (atomic) ref. counted type
#define i_val Map
#define i_from Map_clone
#define i_drop(p) (printf("drop Arc:\n"), Map_drop(p))
// no comparison of Maps needed (or available), and
// no need for atomic ref. count in single thread:
#define i_opt c_no_cmp|c_no_atomic 
#include <stc/csptr.h>

#define i_type Stack
#define i_val_bind Arc // define i_val_bind for csptr / cbox value, not i_val
#include <stc/cstack.h>

#define i_type List
#define i_val_bind Arc // as above
#include <stc/clist.h>

int main()
{
    c_auto (Stack, stack)
    c_auto (List, list)
    {
        // POPULATE the stack with shared pointers to Map:
        Map *map;
        map = Stack_push(&stack, Arc_new(Map_init()))->get;
        c_apply_pair (Map, emplace, map, {
            {"Joey", 1990}, {"Mary", 1995}, {"Joanna", 1992}
        });
        map = Stack_push(&stack, Arc_new(Map_init()))->get;
        c_apply_pair (Map, emplace, map, {
            {"Rosanna", 2001}, {"Brad", 1999}, {"Jack", 1980}
        });

        // POPULATE the list:
        map = List_push_back(&list, Arc_new(Map_init()))->get;
        c_apply_pair (Map, emplace, map, {
            {"Steve", 1979}, {"Rick", 1974}, {"Tracy", 2003}
        });
        
        // Share two Maps from the stack with the list using emplace (clones the csptr):
        List_emplace_back(&list, stack.data[0]);
        List_emplace_back(&list, stack.data[1]);
        
        // Clone (deep copy) a Map from the stack to the list
        // List will contain two shared and two unshared maps.
        map = List_push_back(&list, Arc_new(Map_clone(*stack.data[1].get)))->get;
        
        // Add one more element to the cloned map:
        Map_emplace_or_assign(map, "CLONED", 2021);

        // Add one more element to the shared map:
        Map_emplace_or_assign(stack.data[1].get, "SHARED", 2021);


        puts("STACKS");
        c_foreach (i, Stack, stack) {
            c_forpair (name, year, Map, *i.ref->get)
                printf(" %s:%d", _.name.str, _.year);
            puts("");
        }
        puts("LIST");
        c_foreach (i, List, list) {
            c_forpair (name, year, Map, *i.ref->get)
                printf(" %s:%d", _.name.str, _.year);
            puts("");
        }
    }
}
```
Output:
```
STACKS
 Joanna:1992 Joey:1990 Mary:1995
 Brad:1999 Jack:1980 Rosanna:2001 SHARED:2021
LIST
 Rick:1974 Steve:1979 Tracy:2003
 Joanna:1992 Joey:1990 Mary:1995
 Brad:1999 Jack:1980 Rosanna:2001 SHARED:2021
 Brad:1999 CLONED:2021 Jack:1980 Rosanna:2001
drop Arc:
drop name: Rick
drop name: Tracy
drop name: Steve
drop Arc:
drop name: CLONED
drop name: Brad
drop name: Rosanna
drop name: Jack
drop Arc:
drop name: Brad
drop name: SHARED
drop name: Rosanna
drop name: Jack
drop Arc:
drop name: Joanna
drop name: Mary
drop name: Joey
```
