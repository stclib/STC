# Container type cvec

This describes the API of vector type **cvec**.

## Declaration

```c
#define using_cvec(X, Value, valueDestroy=c_default_del,
                             valueCompareRaw=c_default_compare,
                             RawValue=Value,
                             valueToRaw=c_default_to_raw,
                             valueFromRaw=c_default_from_raw)
#define using_cvec_str()
```
The macro `using_cvec()` can be instantiated with 2, 3, 4, or 7 arguments in the global scope.
Defaults values are given above for args not specified. `X` is a type tag name and
will affect the names of all cvec types and methods. E.g. declaring `using_cvec(my, int);`, `X` should
be replaced by `my` in all of the following documentation.

`using_cvec_str()` is a shorthand, expands to:
```
using_cvec(str, cstr_t, cstr_del, cstr_compare_raw, const char*, cstr_to_raw, cstr_from)
```

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
void                cvec_X_emplace_back(cvec_X* self, RawValue val);
void                cvec_X_push_back(cvec_X* self, Value value);
void                cvec_X_pop_back(cvec_X* self);

cvec_X_iter_t       cvec_X_emplace_at(cvec_X* self, cvec_X_iter_t pos, RawValue val);
cvec_X_iter_t       cvec_X_emplace_at_idx(cvec_X* self, size_t idx, RawValue val);
cvec_X_iter_t       cvec_X_insert_at(cvec_X* self, cvec_X_iter_t pos, Value value);
cvec_X_iter_t       cvec_X_insert_at_idx(cvec_X* self, size_t idx, Value value);
cvec_X_iter_t       cvec_X_insert_range(cvec_X* self, cvec_X_iter_t pos,
                                        cvec_X_iter_t first, cvec_X_iter_t finish);
cvec_X_iter_t       cvec_X_insert_range_ptr(cvec_X* self, cvec_X_value_t* pos,
                                            const cvec_X_value_t* pfirst, const cvec_X_value_t* pfinish);

cvec_X_iter_t       cvec_X_erase_at(cvec_X* self, cvec_X_iter_t pos);
cvec_X_iter_t       cvec_X_erase_at_idx(cvec_X* self, size_t idx);
cvec_X_iter_t       cvec_X_erase_range(cvec_X* self, cvec_X_iter_t first, cvec_X_iter_t finish);
cvec_X_iter_t       cvec_X_erase_range_ptr(cvec_X* self, cvec_X_value_t* first, cvec_X_value_t* finish);
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

## Examples
```c
#include <stdio.h>
#include "stc/cvec.h"
using_cvec(i, int);

int main()
{
    // Create a vector containing integers
    cvec_i vec = cvec_inits;
    c_push_items(&vec, cvec_i, {7, 5, 16, 8});

    // Add two more integers to vector
    cvec_i_push_back(&vec, 25);
    cvec_i_push_back(&vec, 13);

    printf("initial: ");
    c_foreach (n, cvec_i, vec) {
        printf(" %d", *n.val);
    }

    // Sort the vector
    cvec_i_sort(&vec);

    printf("\nsorted: ");
    c_foreach (n, cvec_i, vec) {
        printf(" %d", *n.val);
    }

    cvec_i_del(&vec);
}
```
Output:
```
initial:  7 5 16 8 25 13
sorted:  5 7 8 13 16 25
```
### Example 2
```c
#include "stc/cstr.h"
#include "stc/cvec.h"

using_cvec_str();

int main() {
    cvec_str names = cvec_str_init();
    cvec_str_emplace_back(&names, "Mary");
    cvec_str_emplace_back(&names, "Joe");
    cstr_assign(&names.data[1], "Jake"); // replace "Joe".

    cstr_t tmp = cstr_from_fmt("%d elements so far", cvec_str_size(names));

    // Emplace_back() will not compile when adding a new cstr_t type. Use push_back():
    cvec_str_push_back(&names, tmp); // tmp is moved to names, do not del() it.

    printf("%s\n", names.data[1].str); // Access the second element

    c_foreach (i, cvec_str, names)
        printf("item: %s\n", i.val->str);
    cvec_str_del(&names);
}
```
Output:
```
Jake
item: Mary
item: Jake
item: 2 elements so far
```