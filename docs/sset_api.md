# STC [sset](../include/stc/sset.h): Sorted Set
![Set](pics/sset.jpg)

A **sset** is an associative container that contains a sorted set of unique objects of type *i_key*. Sorting is done using the key comparison function *keyCompare*. Search, removal, and insertion operations have logarithmic complexity. **sset** is implemented as an AA-tree.

See the c++ class [std::set](https://en.cppreference.com/w/cpp/container/set) for a functional description.

## Header file and declaration

```c
#define i_TYPE <ct>,<kt> // shorthand to define i_type,i_key
#define i_type <t>       // container type name (default: sset_{i_key})
#define i_key <t>        // element type: REQUIRED. Defines sset_X_value
#define i_cmp <f>        // three-way compare two i_keyraw* : REQUIRED IF i_keyraw is a non-integral type
#define i_keydrop <f>    // destroy key func - defaults to empty destruct
#define i_keyclone <f>   // REQUIRED IF i_keydrop defined

#define i_keyraw <t>     // convertion "raw" type - defaults to i_key
#define i_keyfrom <f>    // convertion func i_keyraw => i_key - defaults to plain copy
#define i_keyto <f>      // convertion func i_key* => i_keyraw - defaults to plain copy

#define i_tag <s>        // alternative typename: sset_{i_tag}. i_tag defaults to i_key
#include "stc/sset.h"
```
`X` should be replaced by the value of `i_tag` in all of the following documentation.

## Methods

```c
sset_X              sset_X_init(void);
sset_X              sset_X_with_capacity(intptr_t cap);
bool                sset_X_reserve(sset_X* self, intptr_t cap);
void                sset_X_shrink_to_fit(sset_X* self);
sset_X              sset_X_clone(sset_x set);

void                sset_X_clear(sset_X* self);
void                sset_X_copy(sset_X* self, const sset_X* other);
void                sset_X_drop(sset_X* self);                                             // destructor

bool                sset_X_is_empty(const sset_X* self);
intptr_t            sset_X_size(const sset_X* self);
intptr_t            sset_X_capacity(const sset_X* self);

const sset_X_value* sset_X_get(const sset_X* self, i_keyraw rkey);                         // const get
sset_X_value*       sset_X_get_mut(sset_X* self, i_keyraw rkey);                           // return NULL if not found
bool                sset_X_contains(const sset_X* self, i_keyraw rkey);
sset_X_iter         sset_X_find(const sset_X* self, i_keyraw rkey);
sset_X_value*       sset_X_find_it(const sset_X* self, i_keyraw rkey, sset_X_iter* out);   // return NULL if not found
sset_X_iter         sset_X_lower_bound(const sset_X* self, i_keyraw rkey);                 // find closest entry >= rkey

sset_X_result       sset_X_insert(sset_X* self, i_key key);
sset_X_result       sset_X_push(sset_X* self, i_key key);                                  // alias for insert()
sset_X_result       sset_X_emplace(sset_X* self, i_keyraw rkey);

int                 sset_X_erase(sset_X* self, i_keyraw rkey);
sset_X_iter         sset_X_erase_at(sset_X* self, sset_X_iter it);                         // return iter after it
sset_X_iter         sset_X_erase_range(sset_X* self, sset_X_iter it1, sset_X_iter it2);    // return updated it2

sset_X_iter         sset_X_begin(const sset_X* self);
sset_X_iter         sset_X_end(const sset_X* self);
void                sset_X_next(sset_X_iter* it);

sset_X_value        sset_X_value_clone(sset_X_value val);
```

## Types

| Type name         | Type definition                                 | Used to represent...        |
|:------------------|:------------------------------------------------|:----------------------------|
| `sset_X`          | `struct { ... }`                                | The sset type               |
| `sset_X_key`      | `i_key`                                         | The key type                |
| `sset_X_value`    | `i_key`                                         | The key type (alias)        |
| `sset_X_keyraw`   | `i_keyraw`                                      | The raw key type            |
| `sset_X_raw`      | `i_keyraw`                                      | The raw key type (alias)    |
| `sset_X_result`   | `struct { sset_X_value* ref; bool inserted; }`  | Result of insert/emplace    |
| `sset_X_iter`     | `struct { sset_X_value *ref; ... }`             | Iterator type               |

## Example
```c
#define i_implement
#include "stc/cstr.h"

#define i_type SSet
#define i_key_str
#include "stc/sset.h"

int main(void)
{
    SSet second = c_init(SSet, {"red", "green", "blue"});
    SSet third={0}, fourth={0}, fifth={0};

    c_foritems (i, const char*, {"orange", "pink", "yellow"})
        SSet_emplace(&third, *i.ref);

    SSet_emplace(&fourth, "potatoes");
    SSet_emplace(&fourth, "milk");
    SSet_emplace(&fourth, "flour");

    // Copy all to fifth:

    fifth = SSet_clone(second);

    c_foreach (i, SSet, third)
        SSet_emplace(&fifth, cstr_str(i.ref));

    c_foreach (i, SSet, fourth)
        SSet_emplace(&fifth, cstr_str(i.ref));

    printf("fifth contains:\n\n");
    c_foreach (i, SSet, fifth)
        printf("%s\n", cstr_str(i.ref));

    c_drop(SSet, &second, &third, &fourth, &fifth);
}
```
Output:
```
fifth contains:

blue
flour
green
milk
orange
pink
potatoes
red
yellow
```
