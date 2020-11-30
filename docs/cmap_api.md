# Introduction

UNDER CONSTRUCTION!

This describes the API of circular singly linked list type **cmap**.

## Declaration

```c
#define using_cmap_str()

#define using_cmap_strkey(Mapped, mappedDestroy=c_default_del)

#define using_cmap_strval($, Key, keyEquals=c_default_equals,
                                  keyHash=c_default_hash16,
                                  keyDestroy=c_default_del,
                                  RawKey=Key,
                                  keyToRaw=c_default_to_raw,
                                  keyFromRaw=c_default_from_raw)

#define using_cmap($, Key, Mapped, mappedDestroy=c_default_del,
                                   keyEqualsRaw=c_default_equals,
                                   keyHashRaw=c_default_hash16,
                                   keyDestroy=c_default_del,
                                   RawKey=Key,
                                   keyToRaw=c_default_to_raw,
                                   keyFromRaw=c_default_from_raw,
                                   RawMapped=Mapped,
                                   mappedFromRaw=c_default_from_raw)
```
The macro `using_cmap()` can be instantiated with 3, 4, 6, 10, or 12 arguments in the global scope.
Default values are given above for args not specified. `$` is a type tag name and
will affect the names of all cmap types and methods. E.g. declaring `using_cmap(my, int);`, `$` should
be replaced by `my` in all of the following documentation.

`using_cmap_str()` is a predefined macro for `using_cmap(str, cstr_t, ...)`.

## Types

| Type name            | Type definition                       | Used to represent...               |
|:---------------------|:--------------------------------------|:-----------------------------------|
| `cmap_$`             | `struct {`                            | The cmap type                      |
|                      | `  cmap_$_value_t* table;             |                                    |
|                      | `  uint8_t* _hashx;`                  |                                    |
|                      | `  ...;`                              |                                    |
|                      | `}`                                   |                                    |
| `cmap_$_key_t`       | `Key`                                 | The cmap key type                  |
| `cmap_$_mapped_t`    | `Mapped`                              | cmap mapped type                   |
| `cmap_$_value_t`     | `struct {`                            | The cmap value type                |
|                      | `  cmap_$_key_t first;                |                                    |
|                      | `  cmap_$_mapped_t second;`           |                                    |
|                      | `}`                                   |                                    |
| `cmap_$_input_t`     | `cmap_$_value_t`                      | cmap input type                    |
| `cmap_$_rawvalue_t`  | `RawMapped`                           | cmap raw value type                |
| `cmap_$_iter_t`      | `struct {`                            | cmap iterator                      |
|                      | `  cmap_$_value_t* val;`              |                                    |
|                      | `  ...;`                              |                                    |
|                      | `}`                                   |                                    |

## Constants and macros

| Name                       | Value            |
|:---------------------------|:-----------------|
|  `cmap_inits`              | `{...}`          |
|  `cmap_empty(map)`         | `true` if empty  |
|  `cmap_size(map)`          |                  |
|  `cmap_capacity(map)`      |                  |


## Header file

All cmap definitions and prototypes may be included in your C source file by including a single header file.

```c
#include "stc/cmap.h"
```
## Methods

### Construction

The interfaces to create a cmap_$ object:
```c
cmap_$              cmap_$_init(void);
cmap_$              cmap_$_with_capacity(size_t cap);
void                cmap_$_set_load_factors(cmap_$* self, float max, float shrink);

void                cmap_$_clear(cmap_$* self);
void                cmap_$_reserve(cmap_$* self, size_t size);
void                cmap_$_swap(cmap_$* a, cmap_$* b);

void                cmap_$_del(cmap_$* self);

bool                cmap_$_empty(cmap_$ m);
size_t              cmap_$_size(cmap_$ m);
size_t              cmap_$_bucket_count(cmap_$ m);
size_t              cmap_$_capacity(cmap_$ m);

void                cmap_$_push_n(cmap_$* self, const cmap_$_input_t in[], size_t size);

cmap_$_result_t     cmap_$_emplace(cmap_$* self, RawKey rawKey RawMapped rawVal);
cmap_$_result_t     cmap_$_insert(cmap_$* self, cmap_$_input_t in);
cmap_$_result_t     cmap_$_insert_or_assign(cmap_$* self, RawKey rawKey, RawMapped rawVal);
cmap_$_result_t     cmap_$_put(cmap_$* self, RawKey rawKey, RawMapped rawVal);
cmap_$_result_t     cmap_$_putv(cmap_$* self, RawKey rawKey, Mapped mapped);
cmap_$_mapped_t*    cmap_$_at(const cmap_$* self, RawKey rawKey);

size_t              cmap_$_erase(cmap_$* self, RawKey rawKey)
void                cmap_$_erase_entry(cmap_$* self, cmap_$_value_t* val);
cmap_$_iter_t       cmap_$_erase_at(cmap_$* self, cmap_$_iter_t pos);

cmap_$_value_t*     cmap_$_find(const cmap_$* self, RawKey rawKey);
bool                cmap_$_contains(const cmap_$* self, RawKey rawKey);

cmap_$_iter_t       cmap_$_begin(cmap_$* self);
cmap_$_iter_t       cmap_$_end(cmap_$* self)
void                cmap_$_next(cmap_$_iter_t* it);
cmap_$_mapped_t*    cmap_$_itval(cmap_$_iter_t it);

cmap_bucket_t       cmap_$_bucket(const cmap_$* self, const cmap_$_rawkey_t* rawKeyPtr);

uint32_t            c_default_hash16(const void *data, size_t len);
uint32_t            c_default_hash32(const void* data, size_t len);
```
