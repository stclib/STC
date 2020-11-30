# Introduction

UNDER CONSTRUCTION!

This describes the API of the unordered set type **cset**.

## Declaration

```c
#define using_cset_str()

#define using_cset(T, Key, keyEqualsRaw=c_default_equals,
                           keyHashRaw=c_default_hash16,
                           keyDestroy=c_default_del,
                           RawKey=Key,
                           keyToRaw=c_default_to_raw,
                           keyFromRaw=c_default_from_raw)
```
The macro `using_cset()` can be instantiated with 2, 4, 5, or 8 arguments in the global scope.
Default values are given above for args not specified. `T` is a type tag name and
will affect the names of all cset types and methods. E.g. declaring `using_cset(my, int);`, `T` should
be replaced by `my` in all of the following documentation.

`using_cset_str()` is a predefined macro for `using_cset(str, cstr_t, ...)`.

## Types

| Type name            | Type definition                       | Used to represent...               |
|:---------------------|:--------------------------------------|:-----------------------------------|
| `cset_T`             | `struct {`                            | The cset type                      |
|                      | `  cset_T_value_t* table;             |                                    |
|                      | `  uint8_t* _hashx;`                  |                                    |
|                      | `  ...;`                              |                                    |
|                      | `}`                                   |                                    |
| `cset_T_key_t`       | `Key`                                 | The cset key type                  |
| `cset_T_mapped_t`    | `Mapped`                              | cset mapped type                   |
| `cset_T_value_t`     | `Key`                                 | The cset value type                |
| `cset_T_result_t`    | `struct {`                            | Result of insert/emplace           |
|                      | `  cset_T_value_t* first;`            |                                    |
|                      | `  bool second;` /* inserted */       |                                    |
|                      | `}`                                   |                                    |
| `cset_T_input_t`     | `cset_T_value_t`                      | cset input type                    |
| `cset_T_iter_t`      | `struct {`                            | cset iterator                      |
|                      | `  cset_T_value_t* val;`              |                                    |
|                      | `  ...;`                              |                                    |
|                      | `}`                                   |                                    |

## Constants and macros

| Name                       | Value            |
|:---------------------------|:-----------------|
|  `cset_inits`              | `{...}`          |
|  `cset_empty(map)`         | `true` if empty  |
|  `cset_size(map)`          |                  |
|  `cset_capacity(map)`      |                  |


## Header file

All cset definitions and prototypes may be included in your C source file by including a single header file.

```c
#include "stc/cset.h"
```
## Methods

### Construction

The interface for cset_T:
```c
cset_T              cset_T_init(void);
cset_T              cset_T_with_capacity(size_t cap);
void                cset_T_set_load_factors(cset_T* self, float max, float shrink);

void                cset_T_clear(cset_T* self);
void                cset_T_reserve(cset_T* self, size_t size);
void                cset_T_swap(cset_T* a, cset_T* b);

void                cset_T_del(cset_T* self);

bool                cset_T_empty(cset_T m);
size_t              cset_T_size(cset_T m);
size_t              cset_T_bucket_count(cset_T m);
size_t              cset_T_capacity(cset_T m);

void                cset_T_push_n(cset_T* self, const cset_T_input_t in[], size_t size);

cset_T_result_t     cset_T_emplace(cset_T* self, cset_T_rawkey_t rawKey);
cset_T_result_t     cset_T_insert(cset_T* self, cset_T_rawkey_t rawKey);

size_t              cset_T_erase(cset_T* self, cset_T_rawkey_t rawKey);
void                cset_T_erase_entry(cset_T* self, cset_T_key_t* key);
cset_T_iter_t       cset_T_erase_at(cset_T* self, cset_T_iter_t pos);

cset_T_value_t*     cset_T_find(const cset_T* self, cset_T_rawkey_t rawKey);
bool                cset_T_contains(const cset_T* self, cset_T_rawkey_t rawKey);

cset_T_iter_t       cset_T_begin(cset_T* self);
cset_T_iter_t       cset_T_end(cset_T* self);
void                cset_T_next(cset_T_iter_t* it);
cset_T_mapped_t*    cset_T_itval(cset_T_iter_t it);

cset_bucket_t       cset_T_bucket(const cset_T* self, const cset_T_rawkey_t* rawKeyPtr);

uint32_t            c_default_hash16(const void *data, size_t len);
uint32_t            c_default_hash32(const void* data, size_t len);
```
