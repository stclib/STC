# STC Container [cset](../stc/cmap.h): Unordered Set
![Set](pics/set.jpg)

A **cset** is an associative container that contains a set of unique objects of type Key. Search, insertion, and removal have average constant-time complexity. See [std::unordered_set](https://en.cppreference.com/w/cpp/container/unordered_set) for a similar c++ class.

## Declaration

```c
#define using_cset(X, Key, keyEqualsRaw=c_default_equals,
                           keyHashRaw=c_default_hash,
                           keyDestroy=c_default_del,
                           keyFromRaw=c_default_clone,
                           keyToRaw=c_default_to_raw,                           
                           RawKey=Key)
#define using_cset_str()                           
```
The macro `using_cset()` can be instantiated with 2, 4, 6, or 8 arguments in the global scope.
Default values are given above for args not specified. `X` is a type tag name and
will affect the names of all cset types and methods. E.g. declaring `using_cset(my, int);`, `X` should
be replaced by `my` in all of the following documentation.

`using_cset_str()` is a predefined macro for `using_cset(str, cstr_t, ...)`.

## Types

| Type name            | Type definition                       | Used to represent...     |
|:---------------------|:--------------------------------------|:-------------------------|
| `cset_X`             | `struct { ... }`                      | The cset type            |
| `cset_X_rawkey_t`    | `RawKey`                              | The raw key type         |
| `cset_X_rawvalue_t`  | `cset_X_rawkey_t`                     | The raw key type         |
| `cset_X_key_t`       | `Key`                                 | The key type             |
| `cset_X_value_t`     | `cset_X_key_t`                        | The value type           |
| `cset_X_result_t`    | `struct { Key first; bool second; }`  | Result of insert/emplace |
| `cset_X_iter_t`      | `struct { cset_X_value_t *ref; ... }` | Iterator type            |

## Constants and macros

| Name                                            | Purpose                  |
|:------------------------------------------------|:-------------------------|
|  `cset_inits`                                   | Initializer const        |

## Header file

All cset definitions and prototypes may be included in your C source file by including a single header file.

```c
#include "stc/cmap.h"  // both cmap and cset
```
## Methods

```c
cset_X              cset_X_init(void);
cset_X              cset_X_with_capacity(size_t cap);
void                cset_X_set_load_factors(cset_X* self, float min_load, float max_load);

cset_X              cset_X_clone(cset_x set);
void                cset_X_clear(cset_X* self);
void                cset_X_reserve(cset_X* self, size_t size);
void                cset_X_swap(cset_X* a, cset_X* b);

void                cset_X_del(cset_X* self);

bool                cset_X_empty(cset_X m);
size_t              cset_X_size(cset_X m);
size_t              cset_X_bucket_count(cset_X m);
size_t              cset_X_capacity(cset_X m);

void                cset_X_push_n(cset_X* self, const RawKey arr[], size_t size);

cset_X_result_t     cset_X_emplace(cset_X* self, RawKey rkey);
cset_X_result_t     cset_X_insert(cset_X* self, RawKey rkey);

size_t              cset_X_erase(cset_X* self, RawKey rkey);
void                cset_X_erase_entry(cset_X* self, cset_X_key_t* key);
cset_X_iter_t       cset_X_erase_at(cset_X* self, cset_X_iter_t pos);

cset_X_value_t*     cset_X_find(const cset_X* self, RawKey rkey);
bool                cset_X_contains(const cset_X* self, RawKey rkey);

cset_X_iter_t       cset_X_begin(cset_X* self);
cset_X_iter_t       cset_X_end(cset_X* self);
void                cset_X_next(cset_X_iter_t* it);
cset_X_value_t*     cset_X_itval(cset_X_iter_t it);

cset_X_value_t      cset_X_value_clone(cset_X_value_t val);
void                cset_X_value_del(cset_X_value_t* val);
uint32_t            c_default_hash(const void *data, size_t len);
uint32_t            c_default_hash32(const void* data, size_t len);
```

## Example
```c
#include <stdio.h>
#include <stc/cstr.h>
#include <stc/cmap.h>

using_cset_str();

int main ()
{
    cset_str first = cset_inits; // empty
    c_init (cset_str, second, {"red", "green", "blue"});
    c_init (cset_str, third, {"orange", "pink", "yellow"});

    cset_str fourth = cset_inits;
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
