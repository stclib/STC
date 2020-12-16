# Container [cset](../stc/cmap.h): Unordered Set

This describes the API of the unordered set type **cset**.
Same base implementation as cmap, but contains and uses keys only.

## Declaration

```c
#define using_cset_str()

#define using_cset(X, Key, keyEqualsRaw=c_default_equals,
                           keyHashRaw=c_default_hash,
                           keyDestroy=c_default_del,
                           RawKey=Key,
                           keyToRaw=c_default_to_raw,
                           keyFromRaw=c_default_from_raw)
```
The macro `using_cset()` can be instantiated with 2, 4, 5, or 8 arguments in the global scope.
Default values are given above for args not specified. `X` is a type tag name and
will affect the names of all cset types and methods. E.g. declaring `using_cset(my, int);`, `X` should
be replaced by `my` in all of the following documentation.

`using_cset_str()` is a predefined macro for `using_cset(str, cstr_t, ...)`.

## Types

| Type name            | Type definition                       | Used to represent...     |
|:---------------------|:--------------------------------------|:-------------------------|
| `cset_X`             | `struct { ... }`                      | The cset type            |
| `cset_X_rawkey_t`    | `RawKey`                              | The raw key type         |
| `cset_X_key_t`       | `Key`                                 | The key type             |
| `cset_X_value_t`     | `Key`                                 | The value type           |
| `cset_X_result_t`    | `struct { Key first; bool second; }`  | Result of insert/emplace |
| `cset_X_input_t`     | `cset_X_rawkey_t`                     | The input type (rawkey)  |
| `cset_X_iter_t`      | `struct { cset_X_value_t *val; ... }` | Iterator type            |

## Constants and macros

| Name                                            | Purpose                  |
|:------------------------------------------------|:-------------------------|
|  `cset_inits`                                   | Initializer const        |
|  `cset_empty(set)`                              | Test for empty set       |
|  `cset_size(set)`                               | Get set size             |
|  `cset_capacity(set)`                           | Get set capacity         |
|  `c_try_emplace(self, ctype, key, val)`         | Emplace if key exist     |

## Header file

All cset definitions and prototypes may be included in your C source file by including a single header file.

```c
#include "stc/cset.h"
```
## Methods

```c
cset_X              cset_X_init(void);
cset_X              cset_X_with_capacity(size_t cap);
void                cset_X_set_load_factors(cset_X* self, float max, float shrink);

void                cset_X_clear(cset_X* self);
void                cset_X_reserve(cset_X* self, size_t size);
void                cset_X_swap(cset_X* a, cset_X* b);

void                cset_X_del(cset_X* self);

bool                cset_X_empty(cset_X m);
size_t              cset_X_size(cset_X m);
size_t              cset_X_bucket_count(cset_X m);
size_t              cset_X_capacity(cset_X m);

void                cset_X_push_n(cset_X* self, const cset_X_input_t in[], size_t size);

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

cset_bucket_t       cset_X_bucket(const cset_X* self, const cset_X_rawkey_t* rkeyPtr);

uint32_t            c_default_hash(const void *data, size_t len);
uint32_t            c_default_hash32(const void* data, size_t len);
```

## Example
```c
#include <stdio.h>
#include "stc/cstr.h"
#include "stc/cset.h"
using_cset_str();

int main ()
{
  cset_str first = cset_inits;                                                             // empty
  cset_str second = cset_inits; c_push_items(&second, cset_str, {"red","green","blue"});   // init list
  cset_str third = cset_inits; c_push_items(&third, cset_str, {"orange","pink","yellow"}); // init list
  cset_str fourth = cset_inits;
  cset_str_emplace(&fourth, "potatoes");
  cset_str_emplace(&fourth, "milk");
  cset_str_emplace(&fourth, "flour");

  cset_str fifth = cset_inits;
  c_foreach (x, cset_str, second) cset_str_emplace(&fifth, x.val->str);
  c_foreach (x, cset_str, third) cset_str_emplace(&fifth, x.val->str);
  c_foreach (x, cset_str, fourth) cset_str_emplace(&fifth, x.val->str);
  c_del(cset_str, &first, &second, &third, &fourth);

  printf("fifth contains:\n\n");
  c_foreach (x, cset_str, fifth) printf("%s\n", x.val->str);
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
