# Introduction

UNDER CONSTRUCTION!

This describes the API of vector type **cvec**.

## Declaration

```c
#define using_cvec_str()

#define using_cvec($, Value, valueDestroy=c_default_del,
                             valueCompareRaw=c_default_compare,
                             RawValue=Value,
                             valueToRaw=c_default_to_raw,
                             valueFromRaw=c_default_from_raw)
```
The macro `using_cvec()` can be instantiated with 2, 3, 4, or 7 arguments in the global scope.
Defaults are given above for args not specified. Note that `z` can be any name, it's a tag and
will affect the names of all cvec types and methods, e.g. for `using_cvec(my, int);` `$` should
be replaced by `my` in all of the following documentation. `using_cvec_str()` is a predefined
macro for `using_cvec(str, cstr_t, ...)`.

## Types

| Type name            | Type definition                        | Used to represent...                |
|:---------------------|:---------------------------------------|:------------------------------------|
| `cvec_$`             | `struct { cvec_$_value_t* data; }`     | The cvec type                       |
| `cvec_$_value_t`     | `Value`                                | The cvec element type               |
| `cvec_$_input_t`     | `cvec_$_value_t`                       | cvec input type                     |
| `cvec_$_rawvalue_t`  | `RawValue`                             | cvec raw value type                 |
| `cvec_$_iter_t`      | `struct { cvec_$_value_t* val; }`      | cvec iterator                       |

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
cvec_$              cvec_$_init(void);
cvec_$              cvec_$_with_size(size_t size, Value fill_val);
cvec_$              cvec_$_with_capacity(size_t size);

void                cvec_$_clear(cvec_$* self);
void                cvec_$_reserve(cvec_$* self, size_t cap);
void                cvec_$_resize(cvec_$* self, size_t size, Value fill_val);
void                cvec_$_swap(cvec_$* a, cvec_$* b);

void                cvec_$_del(cvec_$* self);

bool                cvec_$_empty(cvec_$ vec);
size_t              cvec_$_size(cvec_$ vec);
size_t              cvec_$_capacity(cvec_$ vec);
Value               cvec_$_value_from_raw(RawValue val);

cvec_$_value_t*     cvec_$_at(cvec_$* self, size_t i);
cvec_$_value_t*     cvec_$_front(cvec_$* self);
cvec_$_value_t*     cvec_$_back(cvec_$* self);

void                cvec_$_push_n(cvec_$ *self, const cvec_$_input_t in[], size_t size);
void                cvec_$_push_back(cvec_$* self, Value value);
void                cvec_$_emplace_back(cvec_$* self, RawValue val);
void                cvec_$_pop_back(cvec_$* self);

cvec_$_iter_t       cvec_$_insert_at(cvec_$* self, cvec_$_iter_t pos, Value value);
cvec_$_iter_t       cvec_$_insert_at_idx(cvec_$* self, size_t idx, Value value);
cvec_$_iter_t       cvec_$_emplace_at(cvec_$* self, cvec_$_iter_t pos, RawValue val);
cvec_$_iter_t       cvec_$_emplace_at_idx(cvec_$* self, size_t idx, RawValue val);
cvec_$_iter_t       cvec_$_insert_range(cvec_$* self, cvec_$_iter_t pos,
                                        cvec_$_iter_t first, cvec_$_iter_t finish);
cvec_$_iter_t       cvec_$_insert_range_p(cvec_$* self, cvec_$_value_t* pos,
                                          const cvec_$_value_t* pfirst, const cvec_$_value_t* pfinish);

cvec_$_iter_t       cvec_$_erase_at(cvec_$* self, cvec_$_iter_t pos);
cvec_$_iter_t       cvec_$_erase_at_idx(cvec_$* self, size_t idx);
cvec_$_iter_t       cvec_$_erase_range(cvec_$* self, cvec_$_iter_t first, cvec_$_iter_t finish);
cvec_$_iter_t       cvec_$_erase_range_p(cvec_$* self, cvec_$_value_t* first, cvec_$_value_t* finish);
cvec_$_iter_t       cvec_$_erase_range_idx(cvec_$* self, size_t ifirst, size_t ifinish);

cvec_$_iter_t       cvec_$_find(const cvec_$* self, RawValue val);
cvec_$_iter_t       cvec_$_find_in_range(const cvec_$* self,
                                         cvec_$_iter_t first, cvec_$_iter_t finish, RawValue val);

void                cvec_$_sort(cvec_$* self);
void                cvec_$_sort_with(cvec_$* self, size_t ifirst, size_t ifinish,
                                     int(*cmp)(const cvec_$_value_t*, const cvec_$_value_t*));

cvec_$_iter_t       cvec_$_begin(const cvec_$* self);
cvec_$_iter_t       cvec_$_last(const cvec_$* self);
cvec_$_iter_t       cvec_$_end(const cvec_$* self);
void                cvec_$_next(cvec_$_iter_t* it);
cvec_$_value_t*     cvec_$_itval(cvec_$_iter_t it);
```
