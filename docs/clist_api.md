# STC [clist](../stc/clist.h): Forward List
![List](pics/list.jpg)

The **clist** container supports fast insertion and removal of elements from anywhere in the container.
Fast random access is not supported. Adding, removing and moving the elements within the list, or across
several lists, does not invalidate the iterators currently referring to other elements in the list. However,
an iterator or reference referring to an element is invalidated when the corresponding element is removed
(via *erase_after*) from the list.

Unlike the similar c++ class *std::forward_list*, **clist** also supports *push_back()* (**O**(1) time).
It is implemented as a circular singly-linked list. A **clist** object occupies only one pointer in memory,
and like *std::forward_list* the length of the list is not stored. The method *clist_X_size()* is available,
however computed in **O**(*n*) time.

See the c++ class [std::forward_list](https://en.cppreference.com/w/cpp/container/forward_list) for a functional description.

## Declaration

```c
using_clist(X, Value);
using_clist(X, Value, valueCompareRaw);
using_clist(X, Value, valueCompareRaw, valueDestroy);
using_clist(X, Value, valueCompareRaw, valueDestroy, valueFromRaw, valueToRaw, RawValue);

using_clist_str()
```
The macro `using_clist()` can be instantiated with 2, 3, 5, or 7 arguments in the global scope.
Default values are given above for args not specified. `X` is a type tag name and
will affect the names of all clist types and methods. E.g. declaring `using_clist(my, int);`, `X` should
be replaced by `my` in all of the following documentation. `using_clist_str()` is a shorthand for
```c
using_clist(str, cstr_t, cstr_compare_raw, cstr_del, cstr_from, cstr_c_str, const char*)
```

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

void                clist_X_push_front(clist_X* self, Value value);
void                clist_X_emplace_front(clist_X* self, RawValue raw);

                    // non-std:
void                clist_X_push_back(clist_X* self, Value value);
void                clist_X_emplace_back(clist_X* self, RawValue raw);
void                clist_X_emplace_n(clist_X *self, const clist_X_rawvalue_t arr[], size_t size);

void                clist_X_pop_front(clist_X* self);

clist_X_iter_t      clist_X_insert_after(clist_X* self, clist_X_iter_t it, Value value);
clist_X_iter_t      clist_X_emplace_after(clist_X* self, clist_X_iter_t it, RawValue raw);

clist_X_iter_t      clist_X_erase_after(clist_X* self, clist_X_iter_t it);
clist_X_iter_t      clist_X_erase_range_after(clist_X* self, clist_X_iter_t it1, clist_X_iter_t it2);

clist_X_iter_t      clist_X_splice_after(clist_X* self, clist_X_iter_t it, clist_X* other);
                    // non-std:
clist_X_iter_t      clist_X_splice_front(clist_X* self, clist_X* other);
clist_X_iter_t      clist_X_splice_back(clist_X* self, clist_X* other);

                    // non-std: note: returns range (it1, it2] - excluding it1, including it2:
clist_X             clist_X_splice_out(clist_X* self, clist_X_iter_t it1, clist_X_iter_t it2);

clist_X_iter_t      clist_X_find(const clist_X* self, RawValue raw);
clist_X_iter_t      clist_X_find_before(const clist_X* self, RawValue raw);
clist_X_iter_t      clist_X_find_before_in_range(const clist_X* self,
                                                 clist_X_iter_t it1, clist_X_iter_t it2, RawValue raw);

                    // std: removes all elements equal to raw
size_t              clist_X_remove(clist_X* self, RawValue raw);

void                clist_X_sort(clist_X* self);

clist_X_iter_t      clist_X_before_begin(const clist_X* self);
clist_X_iter_t      clist_X_last(const clist_X* self);
clist_X_iter_t      clist_X_begin(const clist_X* self);
clist_X_iter_t      clist_X_end(const clist_X* self);
void                clist_X_next(clist_X_iter_t* it);
clist_X_value_t*    clist_X_itval(clist_X_iter_t it);

                    // non-std: return iterator n elements forward:
clist_X_iter_t      clist_X_fwd(clist_X_iter it, size_t n);

clist_X_value_t     clist_X_value_clone(clist_X_value_t val);
```

## Types

| Type name             | Type definition                     | Used to represent...      |
|:----------------------|:------------------------------------|:--------------------------|
| `clist_X`             | `struct { clist_X_node_t* last; }`  | The clist type            |
| `clist_X_value_t`     | `Value`                             | The clist element type    |
| `clist_X_rawvalue_t`  | `RawValue`                          | clist raw value type      |
| `clist_X_iter_t`      | `struct { clist_value_t *ref; ... }`| clist iterator            |

The `clist_X_splice_out(self, it1, it2)` can be combined with `clist_X_splice_after(self, it, other)` to mimic c++ `std::forward_list::splice_after(it, other, it1, it2)`. Note however that *it2* is included in elements to be spliced, unlike with *std::forward_list()*. E.g. splice in `[2, 3]` from *L1* after `10` in *L2*:
```c
c_init (clist_i, L1, {1, 2, 3, 4, 5});
c_init (clist_i, L2, {10, 20, 30, 40, 50});

clist_i_iter_t it = clist_i_fwd(clist_i_begin(&L1), 2);
clist_i tmp = clist_i_splice_out(&L1, clist_i_begin(&L1), it);
clist_i_splice_after(&L2, clist_i_begin(&L2), &tmp);

// C++:
// auto it = L1.begin(); std::advance(it, 3);
// L2.splice_after(L2.cbegin(), L1, L1.cbegin(), it);

// L1: 1 4 5
// L2: 10 2 3 20 30 40 50
```

## Example

Interleave *push_front()* / *push_back()* then *sort()*:
```c
#include <stdio.h>
#include "stc/clist.h"
using_clist(d, double);

int main() {
    c_init (clist_d, list, {
        10.0, 20.0, 30.0, 40.0, 50.0, 60.0, 70.0, 80.0, 90.0
    });

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
### Example 2
Show *erase_after()*, *erase_range_after()*:
```c
// erasing from clist
#include <stc/clist.h>
#include <stdio.h>

using_clist(i, int);

int main ()
{
    c_init (clist_i, L, {10, 20, 30, 40, 50});
                                                 // 10 20 30 40 50
    clist_i_iter_t it = clist_i_begin(&L);       // ^
    it = clist_i_erase_after(&L, it);            // 10 30 40 50
                                                 //    ^
    clist_i_iter_t end = clist_i_end(&L);        //
    it = clist_i_erase_range_after(&L, it, end); // 10 30
                                                 //       ^
    printf("mylist contains:");
    c_foreach (x, clist_i, L) printf(" %d", *x.ref);
    puts("");

    clist_i_del(&L);
}
```
Output:
```
mylist contains: 10 30
```
