# Introduction

UNDER CONSTRUCTION!

This describes the API of vector type **cvec**.

## Declaration

```c
#define using_cvec_str()

#define using_cvec(T, Value, valueDestroy=c_default_del,
                             valueCompareRaw=c_default_compare,
                             RawValue=Value,
                             valueToRaw=c_default_to_raw,
                             valueFromRaw=c_default_from_raw)
```
The macro `using_cvec()` can be instantiated with 2, 3, 4, or 7 arguments in the global scope.
Defaults values are given above for args not specified. `T` is a type tag name and
will affect the names of all cvec types and methods. E.g. declaring `using_cvec(my, int);`, `T` should
be replaced by `my` in all of the following documentation.

`using_cvec_str()` is a predefined macro for `using_cvec(str, cstr_t, ...)`.

## Types

| Type name            | Type definition                        | Used to represent...                |
|:---------------------|:---------------------------------------|:------------------------------------|
| `cvec_T`             | `struct { cvec_T_value_t* data; }`     | The cvec type                       |
| `cvec_T_value_t`     | `Value`                                | The cvec element type               |
| `cvec_T_input_t`     | `cvec_T_value_t`                       | cvec input type                     |
| `cvec_T_rawvalue_t`  | `RawValue`                             | cvec raw value type                 |
| `cvec_T_iter_t`      | `struct { cvec_T_value_t* val; }`      | cvec iterator                       |

## Constants and macros

| Name                       | Value            |
|:---------------------------|:-----------------|
|  `cvec_inits`              | `{NULL}`         |
|  `cvec_empty(vec)`         |                  |
|  `cvec_size(vec)`          |                  |
|  `cvec_capacity(vec)`      |                  |


## Header file

All cvec definitions and prototypes may be included in your C source file by including a single header file.

```c
#include "stc/cvec.h"
```
## Methods

### Construction

The interface for cvec:
```c
cvec_T              cvec_T_init(void);
cvec_T              cvec_T_with_size(size_t size, Value fill_val);
cvec_T              cvec_T_with_capacity(size_t size);

void                cvec_T_clear(cvec_T* self);
void                cvec_T_reserve(cvec_T* self, size_t cap);
void                cvec_T_resize(cvec_T* self, size_t size, Value fill_val);
void                cvec_T_swap(cvec_T* a, cvec_T* b);

void                cvec_T_del(cvec_T* self);

bool                cvec_T_empty(cvec_T vec);
size_t              cvec_T_size(cvec_T vec);
size_t              cvec_T_capacity(cvec_T vec);
Value               cvec_T_value_from_raw(RawValue val);

cvec_T_value_t*     cvec_T_at(cvec_T* self, size_t i);
cvec_T_value_t*     cvec_T_front(cvec_T* self);
cvec_T_value_t*     cvec_T_back(cvec_T* self);

void                cvec_T_push_n(cvec_T *self, const cvec_T_input_t in[], size_t size);
void                cvec_T_push_back(cvec_T* self, Value value);
void                cvec_T_emplace_back(cvec_T* self, RawValue val);
void                cvec_T_pop_back(cvec_T* self);

cvec_T_iter_t       cvec_T_insert_at(cvec_T* self, cvec_T_iter_t pos, Value value);
cvec_T_iter_t       cvec_T_insert_at_idx(cvec_T* self, size_t idx, Value value);
cvec_T_iter_t       cvec_T_emplace_at(cvec_T* self, cvec_T_iter_t pos, RawValue val);
cvec_T_iter_t       cvec_T_emplace_at_idx(cvec_T* self, size_t idx, RawValue val);
cvec_T_iter_t       cvec_T_insert_range(cvec_T* self, cvec_T_iter_t pos,
                                        cvec_T_iter_t first, cvec_T_iter_t finish);
cvec_T_iter_t       cvec_T_insert_range_p(cvec_T* self, cvec_T_value_t* pos,
                                          const cvec_T_value_t* pfirst, const cvec_T_value_t* pfinish);

cvec_T_iter_t       cvec_T_erase_at(cvec_T* self, cvec_T_iter_t pos);
cvec_T_iter_t       cvec_T_erase_at_idx(cvec_T* self, size_t idx);
cvec_T_iter_t       cvec_T_erase_range(cvec_T* self, cvec_T_iter_t first, cvec_T_iter_t finish);
cvec_T_iter_t       cvec_T_erase_range_p(cvec_T* self, cvec_T_value_t* first, cvec_T_value_t* finish);
cvec_T_iter_t       cvec_T_erase_range_idx(cvec_T* self, size_t ifirst, size_t ifinish);

cvec_T_iter_t       cvec_T_find(const cvec_T* self, RawValue val);
cvec_T_iter_t       cvec_T_find_in_range(const cvec_T* self,
                                         cvec_T_iter_t first, cvec_T_iter_t finish, RawValue val);

void                cvec_T_sort(cvec_T* self);
void                cvec_T_sort_with(cvec_T* self, size_t ifirst, size_t ifinish,
                                     int(*cmp)(const cvec_T_value_t*, const cvec_T_value_t*));

cvec_T_iter_t       cvec_T_begin(const cvec_T* self);
cvec_T_iter_t       cvec_T_last(const cvec_T* self);
cvec_T_iter_t       cvec_T_end(const cvec_T* self);
void                cvec_T_next(cvec_T_iter_t* it);
cvec_T_value_t*     cvec_T_itval(cvec_T_iter_t it);
```
