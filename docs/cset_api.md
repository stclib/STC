# STC [cset](../include/stc/cset.h): Unordered Set
![Set](pics/set.jpg)

A **cset** is an associative container that contains a set of unique objects of type i_key. Search, insertion, and removal have average constant-time complexity. See the c++ class
[std::unordered_set](https://en.cppreference.com/w/cpp/container/unordered_set) for a functional description.

## Header file and declaration

```c
#define i_key       // hash key: REQUIRED.
#define i_hash      // hash func: REQUIRED IF i_keyraw is a non-pod type.
#define i_eq        // equality comparison two i_keyraw*: !i_cmp will be used if not defined.
#define i_valdrop   // destroy key func - defaults to empty destruct
#define i_keyraw    // convertion "raw" type - defaults to i_key
#define i_keyfrom   // convertion func i_keyraw => i_key - defaults to plain copy
#define i_keyto     // convertion func i_key* => i_keyraw - defaults to plain copy
#define i_tag       // typename tag. defaults to i_key
#define i_type      // full typename of the container
#include <stc/cset.h>
```
`X` should be replaced by the value of `i_tag` in all of the following documentation.

## Methods

```c
cset_X              cset_X_init(void);
cset_X              cset_X_with_capacity(size_t cap);
cset_X              cset_X_clone(cset_x set);

void                cset_X_clear(cset_X* self);
void                cset_X_copy(cset_X* self, cset_X other);
void                cset_X_max_load_factor(cset_X* self, float max_load);            // default: 0.85
bool                cset_X_reserve(cset_X* self, size_t size);
void                cset_X_shrink_to_fit(cset_X* self);
void                cset_X_swap(cset_X* a, cset_X* b);
void                cset_X_drop(cset_X* self);                                       // destructor

size_t              cset_X_size(cset_X set);                                         // num. of allocated buckets
size_t              cset_X_capacity(cset_X set);                                     // buckets * max_load_factor
bool                cset_X_empty(cset_X set);
size_t              cset_X_bucket_count(cset_X set);

bool                cset_X_contains(const cset_X* self, i_keyraw rkey);
const cset_X_value* cset_X_get(const cset_X* self, i_keyraw rkey);                   // return NULL if not found
cset_X_value*       cset_X_get_mut(cset_X* self, i_keyraw rkey);                     // mutable get
cset_X_iter         cset_X_find(const cset_X* self, i_keyraw rkey);

cset_X_result       cset_X_insert(cset_X* self, i_key key);
cset_X_result       cset_X_emplace(cset_X* self, i_keyraw rkey);

size_t              cset_X_erase(cset_X* self, i_keyraw rkey);                       // return 0 or 1
cset_X_iter         cset_X_erase_at(cset_X* self, cset_X_iter it);                   // return iter after it
void                cset_X_erase_entry(cset_X* self, cset_X_value* entry);

cset_X_iter         cset_X_begin(const cset_X* self);
cset_X_iter         cset_X_end(const cset_X* self);
void                cset_X_next(cset_X_iter* it);

cset_X_value        cset_X_value_clone(cset_X_value val);
```

## Types

| Type name          | Type definition                                  | Used to represent...        |
|:-------------------|:-------------------------------------------------|:----------------------------|
| `cset_X`           | `struct { ... }`                                 | The cset type               |
| `cset_X_rawkey`    | `i_keyraw`                                       | The raw key type            |
| `cset_X_raw`       | `i_keyraw`                                       | The raw value type          |
| `cset_X_key`       | `i_key`                                          | The key type                |
| `cset_X_value`     | `i_key`                                          | The value                   |
| `cset_X_result`    | `struct { cset_X_value* ref; bool inserted; }`   | Result of insert/emplace    |
| `cset_X_iter`      | `struct { cset_X_value *ref; ... }`              | Iterator type               |

## Example
```c
#include <stc/cstr.h>

#define i_key_str
#include <stc/cset.h>

int main ()
{
    c_auto (cset_str, fifth)
    {
        c_auto (cset_str, first, second)
        c_auto (cset_str, third, fourth)
        {
            c_apply(v, cset_str_emplace(&second, v), const char*,
                {"red", "green", "blue"});
            c_apply(v, cset_str_emplace(&third, v), const char*,
                {"orange", "pink", "yellow"});

            cset_str_emplace(&fourth, "potatoes");
            cset_str_emplace(&fourth, "milk");
            cset_str_emplace(&fourth, "flour");

            fifth = cset_str_clone(second);
            c_foreach (i, cset_str, third)
                cset_str_emplace(&fifth, i.ref->str);
            c_foreach (i, cset_str, fourth)
                cset_str_emplace(&fifth, i.ref->str);
        }
        printf("fifth contains:\n\n");
        c_foreach (i, cset_str, fifth)
            printf("%s\n", i.ref->str);
    }
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
