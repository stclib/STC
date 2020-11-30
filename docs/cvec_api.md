# Introduction

UNDER CONSTRUCTION!

This describes the API of vector type **cvec**.

## Instantiation

```c
#define using_cvec_str()

#define using_cvec(z, Value, valueDestroy=c_default_del,
                             valueCompareRaw=c_default_compare,
                             RawValue=Value,
                             valueToRaw=c_default_to_raw,
                             valueFromRaw=c_default_from_raw)
```
The macro `using_cvec()` can be instantiated with 2, 3, 4, or 7 arguments. Defaults are given above for args not specified.
Note that `z` can be any name, it's a tag and will affect the names of all cvec types and methods,
e.g. for `using_cvec(my, int);` `_z` should be replaced by `_my` in all the following documentation.
`using_cvec_str()` is a predefined macro for `cvec_using(str, cstr_t, ...)`.

## Types

| Type name            | Type definition                         | Used to represent...                |
|:---------------------|:------------------------------------- --|:------------------------------------|
| `cvec_z`             | `struct { cvec_z_value_t* data; }`      | The cvec type                       |
| `cvec_z_value_t`     | `Value`                                 | The cvec element type               |
| `cvec_z_input_t`     | `cvec_z_value_t`                        | cvec input type                     |
| `cvec_z_rawvalue_t`  | `RawValue`                              | cvec raw value type                 |
| `cvec_z_iter_t`      | `struct { cvec_z_value_t* val; }`       | cvec iterator                       |

## Constants and macros

| Name                       | Value            |
|:---------------------------|:-----------------|
|  `cvec_inits`              | `{NULL}`         |
|  `cvec_size(v)             |                  |
|  `cvec_capacity(v)`        |                  |
|  `cvec_empty(v)`           |                  |

## Header file

All cvec definitions and prototypes may be included in your C source file by including a single header file.

```c
#include "stc/cvec.h"
```
## Methods

### Construction

The interface for cvec:
```c
cvec_z              cvec_z_init(void);
cvec_z              cvec_z_with_size(size_t size, Value null_val);
cvec_z              cvec_z_with_capacity(size_t size);

void                cvec_z_clear(cvec_z* self);
void                cvec_z_reserve(cvec_z* self, size_t cap);
void                cvec_z_resize(cvec_z* self, size_t size, Value fill_val);
void                cvec_z_swap(cvec_z* a, cvec_z* b);

void                cvec_z_del(cvec_z* self);

bool                cvec_z_empty(cvec_z vec);
size_t              cvec_z_size(cvec_z vec);
size_t              cvec_z_capacity(cvec_z vec);
Value               cvec_z_value_from_raw(RawValue val);

void                cvec_z_push_n(cvec_z *self, const cvec_z_input_t in[], size_t size);
void                cvec_z_push_back(cvec_z* self, Value value);
void                cvec_z_emplace_back(cvec_z* self, RawValue val);
void                cvec_z_pop_back(cvec_z* self);

cvec_z_iter_t       cvec_z_insert_range(cvec_z* self, cvec_z_iter_t pos, cvec_z_iter_t first, cvec_z_iter_t finish);
cvec_z_iter_t       cvec_z_insert_range_p(cvec_z* self, cvec_z_value_t* pos, const cvec_z_value_t* pfirst, const cvec_z_value_t* pfinish);
cvec_z_iter_t       cvec_z_insert_at(cvec_z* self, cvec_z_iter_t pos, Value value);
cvec_z_iter_t       cvec_z_insert_at_idx(cvec_z* self, size_t idx, Value value);
cvec_z_iter_t       cvec_z_emplace_at(cvec_z* self, cvec_z_iter_t pos, RawValue val);
cvec_z_iter_t       cvec_z_emplace_at_idx(cvec_z* self, size_t idx, RawValue val);

cvec_z_iter_t       cvec_z_erase_range(cvec_z* self, cvec_z_iter_t first, cvec_z_iter_t finish);
cvec_z_iter_t       cvec_z_erase_range_p(cvec_z* self, cvec_z_value_t* first, cvec_z_value_t* finish);
cvec_z_iter_t       cvec_z_erase_at(cvec_z* self, cvec_z_iter_t pos);
cvec_z_iter_t       cvec_z_erase_at_idx(cvec_z* self, size_t idx);
cvec_z_iter_t       cvec_z_erase_range_idx(cvec_z* self, size_t ifirst, size_t ifinish);

cvec_z_iter_t       cvec_z_find(const cvec_z* self, RawValue val);
cvec_z_iter_t       cvec_z_find_in_range(const cvec_z* self, cvec_z_iter_t first, cvec_z_iter_t finish, RawValue val);

cvec_z_value_t*     cvec_z_at(cvec_z* self, size_t i);

void                cvec_z_sort(cvec_z* self);
void                cvec_z_sort_with(cvec_z* self, size_t ifirst, size_t ifinish, int(*cmp)(const cvec_z_value_t*, const cvec_z_value_t*));

Value*              cvec_z_front(cvec_z* self);
Value*              cvec_z_back(cvec_z* self);

cvec_z_iter_t       cvec_z_begin(const cvec_z* self);
cvec_z_iter_t       cvec_z_last(const cvec_z* self);
cvec_z_iter_t       cvec_z_end(const cvec_z* self);
void                cvec_z_next(cvec_z_iter_t* it);
cvec_z_value_t*     cvec_z_itval(cvec_z_iter_t it);
```
