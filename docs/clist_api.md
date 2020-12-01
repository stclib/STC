# Container type clist

This describes the API of circular singly linked list type **clist**.

## Declaration

```c
#define using_clist_str()

#define using_clist(X, Value, valueDestroy=c_default_del,
                              valueCompareRaw=c_default_compare,
                              RawValue=Value,
                              valueToRaw=c_default_to_raw,
                              valueFromRaw=c_default_from_raw)
```
The macro `using_clist()` can be instantiated with 2, 3, 4, or 7 arguments in the global scope.
Default values are given above for args not specified. `X` is a type tag name and
will affect the names of all clist types and methods. E.g. declaring `using_clist(my, int);`, `X` should
be replaced by `my` in all of the following documentation.

`using_clist_str()` is a predefined macro for `using_clist(str, cstr_t, ...)`.

## Types

| Type name             | Type definition                        | Used to represent...                |
|:----------------------|:---------------------------------------|:------------------------------------|
| `clist_X`             | `struct { clist_X_node_t* last; }`     | The clist type                      |
| `clist_X_node_t`      | `struct {`                             | clist node                          |
|                       | `  struct clist_X_node* next;`         |                                     |
|                       | `  clist_X_value_t value;`             |                                     |
|                       | `}`                                    |                                     |
| `clist_X_value_t`     | `Value`                                | The clist element type              |
| `clist_X_input_t`     | `clist_X_value_t`                      | clist input type                    |
| `clist_X_rawvalue_t`  | `RawValue`                             | clist raw value type                |
| `clist_X_iter_t`      | `struct {`                             | clist iterator                      |
|                       | `  clist_X_value_t* val;`              |                                     |
|                       | `  ...;`                               |                                     |
|                       | `}`                                    |                                     |


## Constants and macros

| Name                       | Value            |
|:---------------------------|:-----------------|
|  `clist_inits`             | `{NULL}`         |
|  `clist_empty(list)`       | `true` if empty  |

## Header file

All clist definitions and prototypes may be included in your C source file by including a single header file.

```c
#include "stc/clist.h"
```
## Methods

### Construction

The interfaces to create a clist_X object:
```c
clist_X             clist_X_init(void);

void                clist_X_clear(clist_X* self);
void                clist_X_del(clist_X* self);

bool                clist_X_empty(clist_X list);
size_t              clist_X_size(clist_X list);
Value               clist_X_value_from_raw(RawValue val);

clist_X_value_t*    clist_X_front(clist_X* self);
clist_X_value_t*    clist_X_back(clist_X* self);

void                clist_X_push_n(clist_X *self, const clist_X_input_t in[], size_t size);
void                clist_X_emplace_back(clist_X* self, RawValue val);
void                clist_X_push_back(clist_X* self, Value value);

void                clist_X_emplace_front(clist_X* self, RawValue val);
void                clist_X_push_front(clist_X* self, Value value);
void                clist_X_pop_front(clist_X* self);

clist_X_iter_t      clist_X_emplace_after(clist_X* self, clist_X_iter_t pos, RawValue val);
clist_X_iter_t      clist_X_insert_after(clist_X* self, clist_X_iter_t pos, Value val);

clist_X_iter_t      clist_X_erase_after(clist_X* self, clist_X_iter_t pos);
clist_X_iter_t      clist_X_erase_range_after(clist_X* self, clist_X_iter_t pos, clist_X_iter_t finish);

void                clist_X_splice_front(clist_X* self, clist_X* other);
void                clist_X_splice_back(clist_X* self, clist_X* other);
void                clist_X_splice_after(clist_X* self, clist_X_iter_t pos, clist_X* other);

clist_X_iter_t      clist_X_find(const clist_X* self, RawValue val);
clist_X_iter_t      clist_X_find_before(const clist_X* self,
                                        clist_X_iter_t first, clist_X_iter_t finish, RawValue val);

size_t              clist_X_remove(clist_X* self, RawValue val);

void                clist_X_sort(clist_X* self);

clist_X_iter_t      clist_X_before_begin(const clist_X* self);
clist_X_iter_t      clist_X_begin(const clist_X* self);
clist_X_iter_t      clist_X_last(const clist_X* self);
clist_X_iter_t      clist_X_end(const clist_X* self);
void                clist_X_next(clist_X_iter_t* it);
clist_X_value_t*    clist_X_itval(clist_X_iter_t it);
```

Example:
```c
#include <stdio.h>
#include "stc/clist.h"

using_clist(fx, double);

int main() {
    clist_fx list = clist_fx_init();
    c_push_items(&list, clist_fx, {
        10.0, 20.0, 30.0, 40.0, 50.0, 60.0, 70.0, 80.0, 90.0
    });
    // interleave push_front / push_back:
    c_forrange (i, int, 1, 10) {
        if (i & 1) clist_fx_push_front(&list, (float) i);
        else       clist_fx_push_back(&list, (float) i);
    }

    printf("initial: ");
    c_foreach (i, clist_fx, list)
        printf(" %g", *i.val);

    clist_fx_sort(&list); // mergesort O(n*log n)

    printf("\nsorted: ");
    c_foreach (i, clist_fx, list)
        printf(" %g", *i.val);

    clist_fx_del(&list);
}

Output:
initial:  9 7 5 3 1 10 20 30 40 50 60 70 80 90 2 4 6 8
sorted:  1 2 3 4 5 6 7 8 9 10 20 30 40 50 60 70 80 90
```