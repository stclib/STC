# STC [cdeq](../include/stc/cdeq.h): Double Ended Queue
![Deque](pics/deque.jpg)

A **cdeq** is an indexed sequence container that allows fast insertion and deletion at both its beginning and its end. Note that this container is implemented similar to a vector, but has the same performance profile for both *push_back()* and *push_front()* as *cdeq_X_push_back()*. Iterators may be invalidated after push-operations.

See the c++ class [std::deque](https://en.cppreference.com/w/cpp/container/deque) for a functional description.

## Header file and declaration

```c
#include <stc/cdeq.h>

using_cdeq(X, Value);
using_cdeq(X, Value, valueCompare);
using_cdeq(X, Value, valueCompare, valueDel, valueClone = c_no_clone);
using_cdeq(X, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue);

using_cdeq_str();
```
The macro `using_cdeq()` must be instantiated in the global scope. `X` is a type tag name and
will affect the names of all cdeq types and methods. E.g. declaring `using_cdeq(i, int);`, `X` should
be replaced by `i` in all of the following documentation.

`using_cdeq_str()` is a shorthand for:
```
using_cdeq(str, cstr, c_rawstr_compare, cstr_del, cstr_from, cstr_str, const char*)
```

## Methods

```c
cdeq_X              cdeq_X_init(void);
cdeq_X              cdeq_X_with_capacity(size_t size);
cdeq_X              cdeq_X_clone(cdeq_X deq);

void                cdeq_X_clear(cdeq_X* self);
void                cdeq_X_shrink_to_fit(cdeq_X* self);
void                cdeq_X_reserve(cdeq_X* self, size_t cap);
void                cdeq_X_swap(cdeq_X* a, cdeq_X* b);

void                cdeq_X_del(cdeq_X* self);      // destructor

bool                cdeq_X_empty(cdeq_X deq);
size_t              cdeq_X_size(cdeq_X deq);
size_t              cdeq_X_capacity(cdeq_X deq);

cdeq_X_value_t*     cdeq_X_at(const cdeq_X* self, size_t idx);
cdeq_X_value_t*     cdeq_X_front(const cdeq_X* self);
cdeq_X_value_t*     cdeq_X_back(const cdeq_X* self);

void                cdeq_X_push_front(cdeq_X* self, Value value);
void                cdeq_X_emplace_front(cdeq_X* self, RawValue raw);
void                cdeq_X_pop_front(cdeq_X* self);

void                cdeq_X_push_back(cdeq_X* self, Value value);
void                cdeq_X_emplace_back(cdeq_X* self, RawValue raw);
void                cdeq_X_pop_back(cdeq_X* self); 

cdeq_X_iter_t       cdeq_X_insert(cdeq_X* self, size_t idx, Value value);                        // move value 
cdeq_X_iter_t       cdeq_X_insert_n(cdeq_X* self, size_t idx, const Value[] arr, size_t n);      // move arr values
cdeq_X_iter_t       cdeq_X_insert_at(cdeq_X* self, cdeq_X_iter_t it, Value value);               // move value 

cdeq_X_iter_t       cdeq_X_emplace(cdeq_X* self, size_t idx, RawValue raw);
cdeq_X_iter_t       cdeq_X_emplace_n(cdeq_X* self, size_t idx, const RawValue[] arr, size_t n);
cdeq_X_iter_t       cdeq_X_emplace_at(cdeq_X* self, cdeq_X_iter_t it, RawValue raw);
cdeq_X_iter_t       cdeq_X_emplace_range(cdeq_X* self, cdeq_X_iter_t it, 
                                         cdeq_X_iter_t it1, cdeq_X_iter_t it2);                 // will clone
cdeq_X_iter_t       cdeq_X_emplace_range_p(cdeq_X* self, Value* pos, 
                                           const Value* p1, const Value* p2);

cdeq_X_iter_t       cdeq_X_erase(cdeq_X* self, size_t idx);
cdeq_X_iter_t       cdeq_X_erase_n(cdeq_X* self, size_t idx, size_t n);
cdeq_X_iter_t       cdeq_X_erase_at(cdeq_X* self, cdeq_X_iter_t it);
cdeq_X_iter_t       cdeq_X_erase_range(cdeq_X* self, cdeq_X_iter_t it1, cdeq_X_iter_t it2);

cdeq_X_iter_t       cdeq_X_find(const cdeq_X* self, RawValue raw);
cdeq_X_iter_t       cdeq_X_find_in(cdeq_X_iter_t i1, cdeq_X_iter_t i2, RawValue raw);
cdeq_X_value_t*     cdeq_X_get(const cdeq_X* self, RawValue raw);                            // returns NULL if not found

void                cdeq_X_sort(cdeq_X* self);
void                cdeq_X_sort_range(cdeq_X_iter_t i1, cdeq_X_iter_t i2,
                                      int(*cmp)(const Value*, const Value*));

cdeq_X_iter_t       cdeq_X_begin(const cdeq_X* self);
cdeq_X_iter_t       cdeq_X_end(const cdeq_X* self);
void                cdeq_X_next(cdeq_X_iter_t* it);

cdeq_X_rawvalue_t   cdeq_X_value_toraw(cdeq_X_value_t* pval);
cdeq_X_value_t      cdeq_X_value_clone(cdeq_X_value_t val);
```

## Types

| Type name            | Type definition                     | Used to represent...   |
|:---------------------|:------------------------------------|:-----------------------|
| `cdeq_X`             | `struct { cdeq_X_value_t* data; }`  | The cdeq type          |
| `cdeq_X_value_t`     | `Value`                             | The cdeq value type    |
| `cdeq_X_rawvalue_t`  | `RawValue`                          | The raw value type     |
| `cdeq_X_iter_t`      | `struct { cdeq_X_value_t* ref; }`   | The iterator type      |

## Examples
```c
#include <stc/cdeq.h>
#include <stdio.h>

using_cdeq(i, int);

int main() {
    cdeq_i q = cdeq_i_init();
    cdeq_i_push_front(&q, 10);
    c_foreach (i, cdeq_i, q)
        printf(" %d", *i.ref);
    puts("");

    c_emplace(cdeq_i, q, {1, 4, 5, 22, 33, 2});
    c_foreach (i, cdeq_i, q)
        printf(" %d", *i.ref);
    puts("");

    cdeq_i_push_front(&q, 9);
    cdeq_i_push_front(&q, 20);
    cdeq_i_push_back(&q, 11);
    cdeq_i_push_front(&q, 8);

    c_foreach (i, cdeq_i, q)
        printf(" %d", *i.ref);
    puts("");
    cdeq_i_del(&q);
}
```
Output:
```
 10
 10 1 4 5 22 33 2
 8 20 9 10 1 4 5 22 33 2 1
```
