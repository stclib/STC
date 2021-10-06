# STC [cdeq](../include/stc/cdeq.h): Double Ended Queue
![Deque](pics/deque.jpg)

A **cdeq** is an indexed sequence container that allows fast insertion and deletion at both its beginning and its end. Note that this container is implemented similar to a vector, but has the same performance profile for both *push_back()* and *push_front()* as *cdeq_X_push_back()*. Iterators may be invalidated after push-operations.

See the c++ class [std::deque](https://en.cppreference.com/w/cpp/container/deque) for a functional description.

## Header file and declaration

```c
#define i_val       // value: REQUIRED
#define i_cmp       // three-way compare two i_valraw* : REQUIRED IF i_valraw is a non-integral type
#define i_del       // destroy value func - defaults to empty destruct
#define i_valraw    // convertion "raw" type - defaults to i_val
#define i_valfrom   // convertion func i_valraw => i_val - defaults to plain copy
#define i_valto     // convertion func i_val* => i_valraw - defaults to plain copy
#define i_tag       // defaults to i_val
#include <stc/cdeq.h>
```
`X` should be replaced by the value of `i_tag` in all of the following documentation.

## Methods

```c
cdeq_X              cdeq_X_init(void);
cdeq_X              cdeq_X_with_capacity(size_t size);
cdeq_X              cdeq_X_clone(cdeq_X deq);

void                cdeq_X_clear(cdeq_X* self);
void                cdeq_X_copy(cdeq_X* self, cdeq_X other);
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

cdeq_X_value_t*     cdeq_X_push_front(cdeq_X* self, i_val value);
cdeq_X_value_t*     cdeq_X_emplace_front(cdeq_X* self, i_valraw raw);
void                cdeq_X_pop_front(cdeq_X* self);

cdeq_X_value_t*     cdeq_X_push_back(cdeq_X* self, i_val value);
cdeq_X_value_t*     cdeq_X_emplace_back(cdeq_X* self, i_valraw raw);
void                cdeq_X_pop_back(cdeq_X* self); 

cdeq_X_iter_t       cdeq_X_insert(cdeq_X* self, size_t idx, i_val value);                        // move value 
cdeq_X_iter_t       cdeq_X_insert_n(cdeq_X* self, size_t idx, const i_val[] arr, size_t n);      // move arr values
cdeq_X_iter_t       cdeq_X_insert_at(cdeq_X* self, cdeq_X_iter_t it, i_val value);               // move value 

cdeq_X_iter_t       cdeq_X_emplace(cdeq_X* self, size_t idx, i_valraw raw);
cdeq_X_iter_t       cdeq_X_emplace_n(cdeq_X* self, size_t idx, const i_valraw[] arr, size_t n);
cdeq_X_iter_t       cdeq_X_emplace_at(cdeq_X* self, cdeq_X_iter_t it, i_valraw raw);
cdeq_X_iter_t       cdeq_X_emplace_range(cdeq_X* self, cdeq_X_iter_t it, 
                                         cdeq_X_iter_t it1, cdeq_X_iter_t it2);                 // will clone
cdeq_X_iter_t       cdeq_X_emplace_range_p(cdeq_X* self, i_val* pos, 
                                           const i_val* p1, const i_val* p2);

cdeq_X_iter_t       cdeq_X_erase(cdeq_X* self, size_t idx);
cdeq_X_iter_t       cdeq_X_erase_n(cdeq_X* self, size_t idx, size_t n);
cdeq_X_iter_t       cdeq_X_erase_at(cdeq_X* self, cdeq_X_iter_t it);
cdeq_X_iter_t       cdeq_X_erase_range(cdeq_X* self, cdeq_X_iter_t it1, cdeq_X_iter_t it2);

cdeq_X_iter_t       cdeq_X_find(const cdeq_X* self, i_valraw raw);
cdeq_X_iter_t       cdeq_X_find_in(cdeq_X_iter_t i1, cdeq_X_iter_t i2, i_valraw raw);
cdeq_X_value_t*     cdeq_X_get(const cdeq_X* self, i_valraw raw);                            // returns NULL if not found

void                cdeq_X_sort(cdeq_X* self);
void                cdeq_X_sort_range(cdeq_X_iter_t i1, cdeq_X_iter_t i2,
                                      int(*cmp)(const i_val*, const i_val*));

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
| `cdeq_X_value_t`     | `i_val`                             | The cdeq value type    |
| `cdeq_X_rawvalue_t`  | `i_valraw`                          | The raw value type     |
| `cdeq_X_iter_t`      | `struct { cdeq_X_value_t* ref; }`   | The iterator type      |

## Examples
```c
#define i_val int
#define i_tag i
#include <stc/cdeq.h>

#include <stdio.h>

int main() {
    cdeq_i q = cdeq_i_init();
    cdeq_i_push_front(&q, 10);
    c_foreach (i, cdeq_i, q)
        printf(" %d", *i.ref);
    puts("");

    c_apply(cdeq_i, push_back, &q, {1, 4, 5, 22, 33, 2});
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
