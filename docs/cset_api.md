# STC [cset](../stc/cmap.h): Unordered Set
![Set](pics/set.jpg)

A **cset** is an associative container that contains a set of unique objects of type Key. Search, insertion, and removal have average constant-time complexity. See the c++ class 
[std::unordered_set](https://en.cppreference.com/w/cpp/container/unordered_set) for a functional description.

## Declaration

```c
using_cset(X, Key);
using_cset(X, Key, keyEquals, keyHash);
using_cset(X, Key, keyEquals, keyHash, keyDel, keyClone);
using_cset(X, Key, keyEqualsRaw, keyHashRaw, keyDel, keyFromRaw, keyToRaw, RawKey);
using_cset_str();
```
The macro `using_cset()` must be instantiated in the global scope. `X` is a type tag name and
will affect the names of all cset types and methods. E.g. declaring `using_cset(i, int);`, `X` should
be replaced by `i` in all of the following documentation.

`using_cset_str()` is a predefined macro for `using_cset(str, cstr, ...)`.

## Header file

All cset definitions and prototypes are available by including a single header file.

```c
#include "stc/cset.h"
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

void                cset_X_del(cset_X* self);      // destructor

bool                cset_X_empty(cset_X set);
size_t              cset_X_size(cset_X set);
size_t              cset_X_bucket_count(cset_X set);
size_t              cset_X_capacity(cset_X set);

cset_X_iter_t       cset_X_find(const cset_X* self, RawKey rkey);
bool                cset_X_contains(const cset_X* self, RawKey rkey);

cset_X_result_t     cset_X_insert(cset_X* self, Key key);
cset_X_result_t     cset_X_emplace(cset_X* self, RawKey rkey);
void                cset_X_emplace_n(cset_X* self, const RawKey arr[], size_t size);

size_t              cset_X_erase(cset_X* self, RawKey rkey);
cset_X_iter_t       cset_X_erase_at(cset_X* self, cset_X_iter_t pos);

cset_X_iter_t       cset_X_begin(const cset_X* self);
cset_X_iter_t       cset_X_end(const cset_X* self);
void                cset_X_next(cset_X_iter_t* it);
cset_X_value_t*     cset_X_itval(cset_X_iter_t it);

cset_X_value_t      cset_X_value_clone(cset_X_value_t val);
```

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

## Example
```c
#include <stdio.h>
#include <stc/cstr.h>
#include <stc/cmap.h>

using_cset_str();

int main ()
{
    cset_str first = cset_str_init(); // empty
    c_init (cset_str, second, {"red", "green", "blue"});
    c_init (cset_str, third, {"orange", "pink", "yellow"});

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
