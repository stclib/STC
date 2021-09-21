# STC [cset](../include/stc/cset.h): Unordered Set
![Set](pics/set.jpg)

A **cset** is an associative container that contains a set of unique objects of type Key. Search, insertion, and removal have average constant-time complexity. See the c++ class
[std::unordered_set](https://en.cppreference.com/w/cpp/container/unordered_set) for a functional description.

## Header file and declaration

```c
#define i_tag       // defaults to i_key name
#define i_key       // key: REQUIRED
#define i_hash      // hash func: REQUIRED IF i_keyraw is a non-pod type
#define i_equ       // equality comparison two i_keyraw*.  REQUIRED IF i_keyraw is a non-integral type
#define i_cmp       // three-way compare two i_keyraw* : alternative to i_equ
#define i_keyraw    // convertion "raw" type - defaults to i_key
#define i_keyfrom   // convertion func i_keyraw => i_key - defaults to plain copy
#define i_keyto     // convertion func i_key* => i_keyraw - defaults to plain copy
#define i_keydel    // destroy key func - defaults to empty destruct
#include <stc/cset.h>
```
`X` should be replaced by the value of ***i_tag*** in all of the following documentation.

## Methods

```c
cset_X              cset_X_init(void);
cset_X              cset_X_with_capacity(size_t cap);
cset_X              cset_X_clone(cset_x set);

void                cset_X_clear(cset_X* self);
void                cset_X_max_load_factor(cset_X* self, float max_load);                    // default: 0.85
void                cset_X_reserve(cset_X* self, size_t size);
void                cset_X_shrink_to_fit(cset_X* self);
void                cset_X_swap(cset_X* a, cset_X* b);
void                cset_X_del(cset_X* self);                                                // destructor

bool                cset_X_empty(cset_X set);
size_t              cset_X_size(cset_X set);                                                 // num. of allocated buckets
size_t              cset_X_capacity(cset_X set);                                             // buckets * max_load_factor
size_t              cset_X_bucket_count(cset_X set);

bool                cset_X_contains(const cset_X* self, RawKey rkey);
cset_X_value_t*     cset_X_get(const cset_X* self, RawKey rkey);                             // return NULL if not found
cset_X_iter_t       cset_X_find(const cset_X* self, RawKey rkey);

cset_X_result_t     cset_X_insert(cset_X* self, Key key);
cset_X_result_t     cset_X_emplace(cset_X* self, RawKey rkey);
void                cset_X_emplace_items(cset_X* self, const RawKey arr[], size_t n);

size_t              cset_X_erase(cset_X* self, RawKey rkey);                                 // return 0 or 1
cset_X_iter_t       cset_X_erase_at(cset_X* self, cset_X_iter_t it);                         // return iter after it
void                cset_X_erase_entry(cset_X* self, cset_X_value_t* entry);

cset_X_iter_t       cset_X_begin(const cset_X* self);
cset_X_iter_t       cset_X_end(const cset_X* self);
void                cset_X_next(cset_X_iter_t* it);

cset_X_value_t      cset_X_value_clone(cset_X_value_t val);
```

## Types

| Type name            | Type definition                                  | Used to represent...        |
|:---------------------|:-------------------------------------------------|:----------------------------|
| `cset_X`             | `struct { ... }`                                 | The cset type               |
| `cset_X_rawkey_t`    | `i_keyraw`                                       | The raw key type            |
| `cset_X_rawvalue_t`  | `i_keyraw`                                       | The raw value type          |
| `cset_X_key_t`       | `i_key`                                          | The key type                |
| `cset_X_value_t`     | `i_key`                                          | The value                   |
| `cset_X_result_t`    | `struct { cset_X_value_t* ref; bool inserted; }` | Result of insert/emplace    |
| `cset_X_iter_t`      | `struct { cset_X_value_t *ref; ... }`            | Iterator type               |

## Example
```c
#include <stc/cstr.h>

#define i_key_str
#include <stc/cset.h>

int main ()
{
    cset_str first = cset_str_init(); // empty
    c_var (cset_str, second, {"red", "green", "blue"});
    c_var (cset_str, third, {"orange", "pink", "yellow"});

    cset_str fourth = cset_str_init();
    cset_str_emplace(&fourth, "potatoes");
    cset_str_emplace(&fourth, "milk");
    cset_str_emplace(&fourth, "flour");

    cset_str fifth = cset_str_clone(second);
    c_foreach (i, cset_str, third)
        cset_str_emplace(&fifth, i.ref->str);
    c_foreach (i, cset_str, fourth)
        cset_str_emplace(&fifth, i.ref->str);

    c_del(cset_str, &first, &second, &third, &fourth);

    printf("fifth contains:\n\n");
    c_foreach (i, cset_str, fifth)
        printf("%s\n", i.ref->str);

    cset_str_del(&fifth);
    return 0;
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
