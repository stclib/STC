# STC [hset](../include/stc/hset.h): HashSet (unordered)
![Set](pics/set.jpg)

A **hset** is an associative container that contains a set of unique objects of type i_key. Search, insertion, and removal have average constant-time complexity. See the c++ class
[std::unordered_set](https://en.cppreference.com/w/cpp/container/unordered_set) for a functional description.

## Header file and declaration

```c
#define i_TYPE <ct>,<kt> // shorthand to define i_type,i_key
#define i_type <t>       // container type name (default: hset_{i_key})
#define i_key <t>        // element type: REQUIRED. Defines hset_X_value
#define i_hash <f>       // hash func i_keyraw*: REQUIRED IF i_keyraw is non-pod type
#define i_eq <f>         // equality comparison two i_keyraw*: REQUIRED IF i_keyraw is a
                         // non-integral type. Three-way i_cmp may be specified instead.
#define i_keydrop <f>    // destroy key func: defaults to empty destruct
#define i_keyclone <f>   // clone func: REQUIRED IF i_keydrop defined

#define i_keyraw <t>     // convertion "raw" type - defaults to i_key
#define i_keyfrom <f>    // convertion func i_keyraw => i_key - defaults to plain copy
#define i_keyto <f>      // convertion func i_key* => i_keyraw - defaults to plain copy

#define i_tag <s>        // alternative typename: hmap_{i_tag}. i_tag defaults to i_key
#include "stc/hset.h"
```
`X` should be replaced by the value of `i_tag` in all of the following documentation.

## Methods

```c
hset_X              hset_X_init(void);
hset_X              hset_X_with_capacity(intptr_t cap);
hset_X              hset_X_clone(hset_x set);

void                hset_X_clear(hset_X* self);
void                hset_X_copy(hset_X* self, const hset_X* other);
float               hset_X_max_load_factor(const hset_X* self);              // default: 0.85
bool                hset_X_reserve(hset_X* self, intptr_t size);
void                hset_X_shrink_to_fit(hset_X* self);
void                hset_X_drop(hset_X* self);                               // destructor

bool                hset_X_is_empty(const hset_X* self);
intptr_t            hset_X_size(const hset_X* self);                         // num. of allocated buckets
intptr_t            hset_X_capacity(const hset_X* self);                     // buckets * max_load_factor
intptr_t            hset_X_bucket_count(const hset_X* self);

bool                hset_X_contains(const hset_X* self, i_keyraw rkey);
const hset_X_value* hset_X_get(const hset_X* self, i_keyraw rkey);          // return NULL if not found
hset_X_value*       hset_X_get_mut(hset_X* self, i_keyraw rkey);            // mutable get
hset_X_iter         hset_X_find(const hset_X* self, i_keyraw rkey);

hset_X_result       hset_X_insert(hset_X* self, i_key key);
hset_X_result       hset_X_push(hset_X* self, i_key key);                    // alias for insert.
hset_X_result       hset_X_emplace(hset_X* self, i_keyraw rkey);

int                 hset_X_erase(hset_X* self, i_keyraw rkey);               // return 0 or 1
hset_X_iter         hset_X_erase_at(hset_X* self, hset_X_iter it);           // return iter after it
void                hset_X_erase_entry(hset_X* self, hset_X_value* entry);

hset_X_iter         hset_X_begin(const hset_X* self);
hset_X_iter         hset_X_end(const hset_X* self);
void                hset_X_next(hset_X_iter* it);

hset_X_value        hset_X_value_clone(hset_X_value val);
```

## Types

| Type name          | Type definition                                  | Used to represent...        |
|:-------------------|:-------------------------------------------------|:----------------------------|
| `hset_X`           | `struct { ... }`                                 | The hset type               |
| `hset_X_key`       | `i_key`                                          | The key type                |
| `hset_X_value`     | `i_key`                                          | The key type (alias)        |
| `hset_X_keyraw`    | `i_keyraw`                                       | The raw key type            |
| `hset_X_raw`       | `i_keyraw`                                       | The raw key type (alias)    |
| `hset_X_result`    | `struct { hset_X_value* ref; bool inserted; }`   | Result of insert/emplace    |
| `hset_X_iter`      | `struct { hset_X_value *ref; ... }`              | Iterator type               |

## Example
```c
#define i_implement
#include "stc/cstr.h"
#define i_type Strings
#define i_key_cstr
#include "stc/hset.h"

int main(void)
{
    Strings first = c_init(Strings, {"red", "green", "blue"});
    Strings second={0}, third={0}, fourth={0}, fifth={0};

    c_foritems (i, const char*, {"orange", "pink", "yellow"})
        Strings_emplace(&third, *i.ref);

    c_foreach (i, Strings, third)
        Strings_insert(&fifth, cstr_clone(*i.ref));

    Strings_emplace(&fourth, "potatoes");
    Strings_emplace(&fourth, "milk");
    Strings_emplace(&fourth, "flour");

    c_foreach (i, Strings, fourth)
        Strings_emplace(&fifth, cstr_str(i.ref));

    printf("fifth contains:\n\n");
    c_foreach (i, Strings, fifth)
        printf("%s\n", cstr_str(i.ref));

    c_drop(Strings, &first, &second, &third, &fourth, &fifth);
}
```
Output:
```
fifth contains:

red
green
flour
orange
blue
pink
yellow
milk
potatoes
```
