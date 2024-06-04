# STC [deq](../include/stc/deq.h): Double Ended Queue
![Deque](pics/deque.jpg)

A **deq** is an indexed sequence container that allows fast insertion and deletion at both
its beginning and its end, but has also fast random access to elements. The container is
implemented as a circular dynamic buffer. Iterators may be invalidated after push-operations.

See the c++ class [std::deque](https://en.cppreference.com/w/cpp/container/deque) for a functional description.

## Header file and declaration

```c
#define i_TYPE <ct>,<kt> // shorthand to define i_type,i_key
#define i_type <t>       // deq container type name (default: deq_{i_key})
#define i_key <t>        // element type: REQUIRED. Defines deq_X_value
#define i_cmp <f>        // three-way compare of two i_keyraw*.
#define i_use_cmp        // may be defined instead of i_cmp when i_key is an integral/native-type.
#define i_keydrop <f>    // destroy value func - defaults to empty destruct
#define i_keyclone <f>   // REQUIRED IF i_keydrop is defined

#define i_keyraw <t>     // convertion "raw" type - defaults to i_key
#define i_keyfrom <f>    // convertion func i_keyraw => i_key
#define i_keyto <f>      // convertion func i_key* => i_keyraw

#define i_tag <s>        // alternative typename: deq_{i_tag}. i_tag defaults to i_key
#include "stc/deq.h"
```
`X` should be replaced by the value of `i_tag` in all of the following documentation.

## Methods

```c
deq_X              deq_X_init(void);
deq_X              deq_X_with_capacity(intptr_t size);
deq_X              deq_X_clone(deq_X deq);

void               deq_X_clear(deq_X* self);
void               deq_X_copy(deq_X* self, const deq_X* other);
bool               deq_X_reserve(deq_X* self, intptr_t cap);
void               deq_X_shrink_to_fit(deq_X* self);
void               deq_X_drop(deq_X* self);                                      // destructor

bool               deq_X_is_empty(const deq_X* self);
intptr_t           deq_X_size(const deq_X* self);
intptr_t           deq_X_capacity(const deq_X* self);

const deq_X_value* deq_X_at(const deq_X* self, intptr_t idx);
deq_X_value*       deq_X_at_mut(deq_X* self, intptr_t idx);
const deq_X_value* deq_X_get(const deq_X* self, i_keyraw raw);                   // return NULL if not found
deq_X_value*       deq_X_get_mut(deq_X* self, i_keyraw raw);                     // mutable get
deq_X_iter         deq_X_find(const deq_X* self, i_keyraw raw);
deq_X_iter         deq_X_find_in(deq_X_iter i1, deq_X_iter i2, i_keyraw raw);   // return vec_X_end() if not found

deq_X_value*       deq_X_front(const deq_X* self);
deq_X_value*       deq_X_back(const deq_X* self);

deq_X_value*       deq_X_push_front(deq_X* self, i_key value);
deq_X_value*       deq_X_emplace_front(deq_X* self, i_keyraw raw);
void               deq_X_pop_front(deq_X* self);
deq_X_value        deq_X_pull_front(deq_X* self);                                // move out front element

deq_X_value*       deq_X_push_back(deq_X* self, i_key value);
deq_X_value*       deq_X_push(deq_X* self, i_key value);                         // alias for push_back()
deq_X_value*       deq_X_emplace_back(deq_X* self, i_keyraw raw);
deq_X_value*       deq_X_emplace(deq_X* self, i_keyraw raw);                     // alias for emplace_back()
void               deq_X_pop_back(deq_X* self);
deq_X_value        deq_X_pull_back(deq_X* self);                                 // move out last element

deq_X_iter         deq_X_insert_n(deq_X* self, intptr_t idx, const i_key[] arr, intptr_t n);  // move values
deq_X_iter         deq_X_insert_at(deq_X* self, deq_X_iter it, i_key value);    // move value
deq_X_iter         deq_X_insert_uninit(deq_X* self, intptr_t idx, intptr_t n);   // uninitialized data
                    // copy values:
deq_X_iter         deq_X_emplace_n(deq_X* self, intptr_t idx, const i_keyraw[] arr, intptr_t n);
deq_X_iter         deq_X_emplace_at(deq_X* self, deq_X_iter it, i_keyraw raw);

void               deq_X_erase_n(deq_X* self, intptr_t idx, intptr_t n);
deq_X_iter         deq_X_erase_at(deq_X* self, deq_X_iter it);
deq_X_iter         deq_X_erase_range(deq_X* self, deq_X_iter it1, deq_X_iter it2);

deq_X_iter         deq_X_begin(const deq_X* self);
deq_X_iter         deq_X_end(const deq_X* self);
void               deq_X_next(deq_X_iter* it);
deq_X_iter         deq_X_advance(deq_X_iter it, intptr_t n);

bool               deq_X_eq(const deq_X* c1, const deq_X* c2); // require i_eq/i_cmp/i_less.
deq_X_value        deq_X_value_clone(deq_X_value val);
deq_X_raw          deq_X_value_toraw(const deq_X_value* pval);
void               deq_X_value_drop(deq_X_value* pval);
```

## Types

| Type name         | Type definition                    | Used to represent...   |
|:------------------|:-----------------------------------|:-----------------------|
| `deq_X`           | `struct { deq_X_value* data; }`    | The deq type          |
| `deq_X_value`     | `i_key`                            | The deq value type    |
| `deq_X_raw`       | `i_keyraw`                         | The raw value type     |
| `deq_X_iter`      | `struct { deq_X_value* ref; }`     | The iterator type      |

## Examples
```c
#define i_TYPE Deque,int32_t
#include "stc/deq.h"
#include <stdio.h>

int main(void) {
    Deque q = {0};
    Deque_push_front(&q, 10);
    c_foreach (i, Deque, q)
        printf(" %d", *i.ref);
    puts("");

    c_foritems (i, int, {1, 4, 5, 22, 33, 2})
        Deque_push_back(&q, *i.ref)

    c_foreach (i, Deque, q)
        printf(" %d", *i.ref);
    puts("");

    Deque_push_front(&q, 9);
    Deque_push_front(&q, 20);
    Deque_push_back(&q, 11);
    Deque_push_front(&q, 8);

    c_foreach (i, Deque, q)
        printf(" %d", *i.ref);
    puts("");
    Deque_drop(&q);
}
```
Output:
```
 10
 10 1 4 5 22 33 2
 8 20 9 10 1 4 5 22 33 2 1
```
