# STC Container [clist](../stc/clist.h): Forward List
![List](pics/list.jpg)

This is similar to c++ [std::forward_list](https://en.cppreference.com/w/cpp/container/forward_list), but supports both
*push_front()* and *push_back()* as well as *pop_front()* in **O**(1) time. Implemented as a circular singly linked list.
Also supports various *splice* functions and *merge-sort*. Note that like std::forward_list, the representation size of **clist**
is only one pointer, and length of the list is not stored. The method *clist_X_size()* is therefore computed in **O**(*n*) time.

## Declaration

```c
#define using_clist(X, Value, valueCompareRaw=c_default_compare,
                              valueDestroy=c_default_del,
                              valueFromRaw=c_default_clone,
                              valueToRaw=c_default_to_raw,
                              RawValue=Value)
#define using_clist_str()
```
The macro `using_clist()` can be instantiated with 2, 3, 5, or 7 arguments in the global scope.
Default values are given above for args not specified. `X` is a type tag name and
will affect the names of all clist types and methods. E.g. declaring `using_clist(my, int);`, `X` should
be replaced by `my` in all of the following documentation. `using_clist_str()` is a shorthand for
```c
using_clist(str, cstr_t, cstr_compare_raw, cstr_del, cstr_from, cstr_to_raw, const char*)
```

## Types

| Type name             | Type definition                     | Used to represent...      |
|:----------------------|:------------------------------------|:--------------------------|
| `clist_X`             | `struct { clist_X_node_t* last; }`  | The clist type            |
| `clist_X_value_t`     | `Value`                             | The clist element type    |
| `clist_X_rawvalue_t`  | `RawValue`                          | clist raw value type      |
| `clist_X_iter_t`      | `struct { clist_value_t *ref; ... }`| clist iterator            |

## Constants and macros

| Name                       | Purpose              |
|:---------------------------|:---------------------|
|  `clist_inits`             | Initializer constant |

## Header file

All clist definitions and prototypes may be included in your C source file by including a single header file.

```c
#include "stc/clist.h"
```
## Methods

```c
clist_X             clist_X_init(void);
clist_X             clist_X_clone(clist_X list);

void                clist_X_clear(clist_X* self);
void                clist_X_del(clist_X* self);

bool                clist_X_empty(clist_X list);
size_t              clist_X_size(clist_X list); // note: O(n)

clist_X_value_t*    clist_X_front(clist_X* self);
clist_X_value_t*    clist_X_back(clist_X* self);

void                clist_X_push_n(clist_X *self, const clist_X_rawvalue_t arr[], size_t size);
void                clist_X_emplace_back(clist_X* self, RawValue ref);
void                clist_X_push_back(clist_X* self, Value value);

void                clist_X_emplace_front(clist_X* self, RawValue raw);
void                clist_X_push_front(clist_X* self, Value value);
void                clist_X_pop_front(clist_X* self);

clist_X_iter_t      clist_X_emplace_after(clist_X* self, clist_X_iter_t pos, RawValue raw);
clist_X_iter_t      clist_X_insert_after(clist_X* self, clist_X_iter_t pos, Value raw);

clist_X_iter_t      clist_X_erase_after(clist_X* self, clist_X_iter_t pos);
clist_X_iter_t      clist_X_erase_range_after(clist_X* self, clist_X_iter_t pos, clist_X_iter_t finish);

clist_X_iter_t      clist_X_splice_after(clist_X* self, clist_X_iter_t pos, clist_X* other);
clist_X_iter_t      clist_X_splice_front(clist_X* self, clist_X* other);
clist_X_iter_t      clist_X_splice_back(clist_X* self, clist_X* other);

clist_X             clist_X_split_after(clist_X* self, clist_X_iter_t pos1, clist_X_iter_t pos2);

clist_X_iter_t      clist_X_find(const clist_X* self, RawValue raw);
clist_X_iter_t      clist_X_find_before(const clist_X* self,
                                        clist_X_iter_t first, clist_X_iter_t finish, RawValue ref);

size_t              clist_X_remove(clist_X* self, RawValue raw);

void                clist_X_sort(clist_X* self);

clist_X_iter_t      clist_X_before_begin(const clist_X* self);
clist_X_iter_t      clist_X_last(const clist_X* self);
clist_X_iter_t      clist_X_begin(const clist_X* self);
clist_X_iter_t      clist_X_end(const clist_X* self);
void                clist_X_next(clist_X_iter_t* it);
clist_X_value_t*    clist_X_itval(clist_X_iter_t it);

clist_X_value_t     clist_X_value_clone(clist_X_value_t val);
```
The `clist_X_split_after(self, it1, it2)` can be combined with `clist_X_splice_after(self, pos, other)` to mimic c++ `std::forward_list::splice_after(pos, other, it1, it2)`. Note however, that `it2` is included in elements to be spliced, unlike with `std::forward_list()`. E.g, splice in 2, 3 after 10 in L2:
```c
c_init (clist_i, L1, {1, 2, 3, 4, 5});
c_init (clist_i, L2, {10, 20, 30, 40, 50});

clist_i_iter_t it = clist_i_fwd(clist_i_begin(&L1), 2);
clist_i tmp = clist_i_split_after(&L1, clist_i_begin(&L1), it);
clist_i_splice_after(&L2, clist_i_begin(&L2), &tmp);

// C++:
// auto it = L1.begin(); std::advance(it, 3);
// L2.splice_after(L2.cbegin(), L1, L1.cbegin(), it);

// L1: 1 4 5
// L2: 10 2 3 20 30 40 50
```

## Example
```c
#include <stdio.h>
#include "stc/clist.h"
using_clist(d, double);

int main() {
    clist_d list = clist_inits;
    c_push_items(&list, clist_d, {
        10.0, 20.0, 30.0, 40.0, 50.0, 60.0, 70.0, 80.0, 90.0
    });

    // Interleave push_front / push_back:
    c_forrange (i, int, 1, 10) {
        if (i & 1) clist_d_push_front(&list, (float) i);
        else       clist_d_push_back(&list, (float) i);
    }

    printf("initial: ");
    c_foreach (i, clist_d, list)
        printf(" %g", *i.ref);

    clist_d_sort(&list); // mergesort O(n*log n)

    printf("\nsorted: ");
    c_foreach (i, clist_d, list)
        printf(" %g", *i.ref);

    clist_d_del(&list);
}
```
Output:
```
initial:  9 7 5 3 1 10 20 30 40 50 60 70 80 90 2 4 6 8
sorted:  1 2 3 4 5 6 7 8 9 10 20 30 40 50 60 70 80 90
```
