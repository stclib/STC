# Container [cdeq](../stc/cdeq.h): Double Ended Queue (Deque)

This describes the API of deque type **cdeq**.
See [std::deque](https://en.cppreference.com/w/cpp/container/deque) for corresponding c++ documentation.

## Declaration

```c
#define using_cdeq(X, Value, valueCompareRaw=c_default_compare,
                             valueDestroy=c_default_del,
                             RawValue=Value,
                             valueToRaw=c_default_to_raw,
                             valueFromRaw=c_default_from_raw)
#define using_cdeq_str()
```
The macro `using_cdeq()` can be instantiated with 2, 3, 4, or 7 arguments in the global scope.
Defaults values are given above for args not specified. `X` is a type tag name and
will affect the names of all cdeq types and methods. E.g. declaring `using_cdeq(my, int);`, `X` should
be replaced by `my` in all of the following documentation.

`using_cdeq_str()` is a shorthand for:
```
using_cdeq(str, cstr_t, cstr_compare_raw, cstr_del, const char*, cstr_to_raw, cstr_from)
```

## Types

| Type name            | Type definition                     | Used to represent...   |
|:---------------------|:------------------------------------|:-----------------------|
| `cdeq_X`             | `struct { cdeq_X_value_t* data; }`  | The cdeq type          |
| `cdeq_X_value_t`     | `Value`                             | The cdeq value type    |
| `cdeq_X_input_t`     | `cdeq_X_value_t`                    | The input type         |
| `cdeq_X_rawvalue_t`  | `RawValue`                          | The raw value type     |
| `cdeq_X_iter_t`      | `struct { cdeq_X_value_t* ref; }`   | The iterator type      |

## Constants and macros

| Name                       | Purpose              |
|:---------------------------|:---------------------|
|  `cdeq_inits`              | Initializer constant |

## Header file

All cdeq definitions and prototypes may be included in your C source file by including a single header file.

```c
#include "stc/cdeq.h"
```
## Methods

```c
cdeq_X              cdeq_X_init(void);
cdeq_X              cdeq_X_with_size(size_t size, Value fill);
cdeq_X              cdeq_X_with_capacity(size_t size);
cdeq_X              cdeq_X_clone(cdeq_X vec);

void                cdeq_X_clear(cdeq_X* self);
void                cdeq_X_shrink_to_fit(cdeq_X* self);
void                cdeq_X_reserve(cdeq_X* self, size_t cap);
void                cdeq_X_resize(cdeq_X* self, size_t size, Value fill);
void                cdeq_X_swap(cdeq_X* a, cdeq_X* b);

void                cdeq_X_del(cdeq_X* self);

bool                cdeq_X_empty(cdeq_X vec);
size_t              cdeq_X_size(cdeq_X vec);
size_t              cdeq_X_capacity(cdeq_X vec);

cdeq_X_value_t*     cdeq_X_at(cdeq_X* self, size_t idx);
cdeq_X_value_t*     cdeq_X_front(cdeq_X* self);
cdeq_X_value_t*     cdeq_X_back(cdeq_X* self);

void                cdeq_X_push_n(cdeq_X *self, const cdeq_X_input_t arr[], size_t size);
void                cdeq_X_push_front(cdeq_X* self, Value value);
void                cdeq_X_push_back(cdeq_X* self, Value value);
void                cdeq_X_emplace_front(cdeq_X* self, RawValue raw);
void                cdeq_X_emplace_back(cdeq_X* self, RawValue raw);
void                cdeq_X_pop_front(cdeq_X* self);
void                cdeq_X_pop_back(cdeq_X* self);

cdeq_X_iter_t       cdeq_X_emplace(cdeq_X* self, cdeq_X_iter_t pos, RawValue raw);
cdeq_X_iter_t       cdeq_X_emplace_at(cdeq_X* self, size_t idx, RawValue raw);
cdeq_X_iter_t       cdeq_X_insert(cdeq_X* self, cdeq_X_iter_t pos, Value value);
cdeq_X_iter_t       cdeq_X_insert_at(cdeq_X* self, size_t idx, Value value);
cdeq_X_iter_t       cdeq_X_insert_range(cdeq_X* self, cdeq_X_iter_t pos,
                                        cdeq_X_iter_t first, cdeq_X_iter_t finish);
cdeq_X_iter_t       cdeq_X_insert_range_p(cdeq_X* self, cdeq_X_value_t* pos,
                                          const cdeq_X_value_t* pfirst, const cdeq_X_value_t* pfinish);

cdeq_X_iter_t       cdeq_X_erase(cdeq_X* self, cdeq_X_iter_t pos);
cdeq_X_iter_t       cdeq_X_erase_n(cdeq_X* self, size_t idx, size_t n);
cdeq_X_iter_t       cdeq_X_erase_range(cdeq_X* self, cdeq_X_iter_t first, cdeq_X_iter_t finish);
cdeq_X_iter_t       cdeq_X_erase_range_p(cdeq_X* self, cdeq_X_value_t* pfirst, cdeq_X_value_t* pfinish);

cdeq_X_iter_t       cdeq_X_find(const cdeq_X* self, RawValue raw);
cdeq_X_iter_t       cdeq_X_find_in_range(const cdeq_X* self,
                                         cdeq_X_iter_t first, cdeq_X_iter_t finish, RawValue raw);

void                cdeq_X_sort(cdeq_X* self);
void                cdeq_X_sort_with(cdeq_X* self, size_t ifirst, size_t ifinish,
                                     int(*cmp)(const cdeq_X_value_t*, const cdeq_X_value_t*));

cdeq_X_iter_t       cdeq_X_begin(const cdeq_X* self);
cdeq_X_iter_t       cdeq_X_end(const cdeq_X* self);
void                cdeq_X_next(cdeq_X_iter_t* it);
cdeq_X_value_t*     cdeq_X_itval(cdeq_X_iter_t it);
size_t              cdeq_X_index(const cdeq_X vec, cdeq_X_iter_t it);

Value               cdeq_X_value_from_raw(RawValue raw);
```

## Examples
```c
#include <stdio.h>
#include <stc/cdeq.h>

using_cdeq(i, int);

int main() {
    cdeq_i x = cdeq_i_init();
    cdeq_i_push_front(&x, 10);
    c_foreach (i, cdeq_i, x)
        printf(" %d", *i.ref);
    puts("");

    c_push_items(&x, cdeq_i, {1, 4, 5, 22, 33, 2});
    c_foreach (i, cdeq_i, x)
        printf(" %d", *i.ref);
    puts("");

    cdeq_i_push_front(&x, 9);
    cdeq_i_push_front(&x, 20);
    cdeq_i_push_back(&x, 11);
    cdeq_i_push_front(&x, 8);

    c_foreach (i, cdeq_i, x)
        printf(" %d", *i.ref);
    puts("");
    cdeq_i_del(&x);
}
```
Output:
```
 10
 10 1 4 5 22 33 2
 8 20 9 10 1 4 5 22 33 2 1
```
