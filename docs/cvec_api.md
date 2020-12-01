# Container type cvec

This describes the API of vector type **cvec**.

## Declaration

```c
#define using_cvec_str()

#define using_cvec(X, Value, valueDestroy=c_default_del,
                             valueCompareRaw=c_default_compare,
                             RawValue=Value,
                             valueToRaw=c_default_to_raw,
                             valueFromRaw=c_default_from_raw)
```
The macro `using_cvec()` can be instantiated with 2, 3, 4, or 7 arguments in the global scope.
Defaults values are given above for args not specified. `X` is a type tag name and
will affect the names of all cvec types and methods. E.g. declaring `using_cvec(my, int);`, `X` should
be replaced by `my` in all of the following documentation.

`using_cvec_str()` is a predefined macro for `using_cvec(str, cstr_t, ...)`.

## Types

| Type name            | Type definition                        | Used to represent...                |
|:---------------------|:---------------------------------------|:------------------------------------|
| `cvec_X`             | `struct { cvec_X_value_t* data; }`     | The cvec type                       |
| `cvec_X_value_t`     | `Value`                                | The cvec value type                 |
| `cvec_X_input_t`     | `cvec_X_value_t`                       | The input type                      |
| `cvec_X_rawvalue_t`  | `RawValue`                             | The raw value type                  |
| `cvec_X_iter_t`      | `struct { cvec_X_value_t* val; }`      | The iterator                        |

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
cvec_X              cvec_X_init(void);
cvec_X              cvec_X_with_size(size_t size, Value fill_val);
cvec_X              cvec_X_with_capacity(size_t size);

void                cvec_X_clear(cvec_X* self);
void                cvec_X_reserve(cvec_X* self, size_t cap);
void                cvec_X_resize(cvec_X* self, size_t size, Value fill_val);
void                cvec_X_swap(cvec_X* a, cvec_X* b);

void                cvec_X_del(cvec_X* self);

bool                cvec_X_empty(cvec_X vec);
size_t              cvec_X_size(cvec_X vec);
size_t              cvec_X_capacity(cvec_X vec);
Value               cvec_X_value_from_raw(RawValue val);

cvec_X_value_t*     cvec_X_at(cvec_X* self, size_t i);
cvec_X_value_t*     cvec_X_front(cvec_X* self);
cvec_X_value_t*     cvec_X_back(cvec_X* self);

void                cvec_X_push_n(cvec_X *self, const cvec_X_input_t in[], size_t size);
void                cvec_X_push_back(cvec_X* self, Value value);
void                cvec_X_emplace_back(cvec_X* self, RawValue val);
void                cvec_X_pop_back(cvec_X* self);

cvec_X_iter_t       cvec_X_insert_at(cvec_X* self, cvec_X_iter_t pos, Value value);
cvec_X_iter_t       cvec_X_insert_at_idx(cvec_X* self, size_t idx, Value value);
cvec_X_iter_t       cvec_X_emplace_at(cvec_X* self, cvec_X_iter_t pos, RawValue val);
cvec_X_iter_t       cvec_X_emplace_at_idx(cvec_X* self, size_t idx, RawValue val);
cvec_X_iter_t       cvec_X_insert_range(cvec_X* self, cvec_X_iter_t pos,
                                        cvec_X_iter_t first, cvec_X_iter_t finish);
cvec_X_iter_t       cvec_X_insert_range_p(cvec_X* self, cvec_X_value_t* pos,
                                          const cvec_X_value_t* pfirst, const cvec_X_value_t* pfinish);

cvec_X_iter_t       cvec_X_erase_at(cvec_X* self, cvec_X_iter_t pos);
cvec_X_iter_t       cvec_X_erase_at_idx(cvec_X* self, size_t idx);
cvec_X_iter_t       cvec_X_erase_range(cvec_X* self, cvec_X_iter_t first, cvec_X_iter_t finish);
cvec_X_iter_t       cvec_X_erase_range_p(cvec_X* self, cvec_X_value_t* first, cvec_X_value_t* finish);
cvec_X_iter_t       cvec_X_erase_range_idx(cvec_X* self, size_t ifirst, size_t ifinish);

cvec_X_iter_t       cvec_X_find(const cvec_X* self, RawValue val);
cvec_X_iter_t       cvec_X_find_in_range(const cvec_X* self,
                                         cvec_X_iter_t first, cvec_X_iter_t finish, RawValue val);

void                cvec_X_sort(cvec_X* self);
void                cvec_X_sort_with(cvec_X* self, size_t ifirst, size_t ifinish,
                                     int(*cmp)(const cvec_X_value_t*, const cvec_X_value_t*));

cvec_X_iter_t       cvec_X_begin(const cvec_X* self);
cvec_X_iter_t       cvec_X_last(const cvec_X* self);
cvec_X_iter_t       cvec_X_end(const cvec_X* self);
void                cvec_X_next(cvec_X_iter_t* it);
cvec_X_value_t*     cvec_X_itval(cvec_X_iter_t it);
```
