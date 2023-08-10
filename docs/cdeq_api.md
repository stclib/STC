# STC [cdeq](../include/stc/cdeq.h): Double Ended Queue
![Deque](pics/deque.jpg)

A **cdeq** is an indexed sequence container that allows fast insertion and deletion at both
its beginning and its end, but has also fast random access to elements. The container is
implemented as a circular dynamic buffer. Iterators may be invalidated after push-operations.

See the c++ class [std::deque](https://en.cppreference.com/w/cpp/container/deque) for a functional description.

## Header file and declaration

```c
#define i_key <t>      // element type: REQUIRED. Note: i_val* may be specified instead of i_key*.
#define i_type <t>     // cdeq container type name
#define i_cmp <f>      // three-way compare of two i_keyraw*.
#define i_use_cmp      // define instead of i_cmp only when i_key is an integral/native-type.
#define i_keydrop <f>  // destroy value func - defaults to empty destruct
#define i_keyclone <f> // REQUIRED IF i_keydrop is defined

#define i_keyraw <t>   // convertion "raw" type - defaults to i_key
#define i_keyfrom <f>  // convertion func i_keyraw => i_key
#define i_keyto <f>    // convertion func i_key* => i_keyraw

#define i_tag <s>      // alternative typename: cdeq_{i_tag}. i_tag defaults to i_key
#include <stc/cdeq.h>
```
`X` should be replaced by the value of `i_tag` in all of the following documentation.

## Methods

```c
cdeq_X              cdeq_X_init(void);
cdeq_X              cdeq_X_with_capacity(intptr_t size);
cdeq_X              cdeq_X_clone(cdeq_X deq);

void                cdeq_X_clear(cdeq_X* self);
void                cdeq_X_copy(cdeq_X* self, const cdeq_X* other);
bool                cdeq_X_reserve(cdeq_X* self, intptr_t cap);
void                cdeq_X_shrink_to_fit(cdeq_X* self);
void                cdeq_X_drop(cdeq_X* self);                                      // destructor

bool                cdeq_X_empty(const cdeq_X* self);
intptr_t            cdeq_X_size(const cdeq_X* self);
intptr_t            cdeq_X_capacity(const cdeq_X* self);

const cdeq_X_value* cdeq_X_at(const cdeq_X* self, intptr_t idx);
cdeq_X_value*       cdeq_X_at_mut(cdeq_X* self, intptr_t idx);
const cdeq_X_value* cdeq_X_get(const cdeq_X* self, i_keyraw raw);                   // return NULL if not found
cdeq_X_value*       cdeq_X_get_mut(cdeq_X* self, i_keyraw raw);                     // mutable get
cdeq_X_iter         cdeq_X_find(const cdeq_X* self, i_keyraw raw);
cdeq_X_iter         cdeq_X_find_in(cdeq_X_iter i1, cdeq_X_iter i2, i_keyraw raw);   // return cvec_X_end() if not found

cdeq_X_value*       cdeq_X_front(const cdeq_X* self);
cdeq_X_value*       cdeq_X_back(const cdeq_X* self);

cdeq_X_value*       cdeq_X_push_front(cdeq_X* self, i_key value);
cdeq_X_value*       cdeq_X_emplace_front(cdeq_X* self, i_keyraw raw);
void                cdeq_X_pop_front(cdeq_X* self);
cdeq_X_value        cdeq_X_pull_front(cdeq_X* self);                                // move out front element

cdeq_X_value*       cdeq_X_push_back(cdeq_X* self, i_key value);
cdeq_X_value*       cdeq_X_push(cdeq_X* self, i_key value);                         // alias for push_back()
cdeq_X_value*       cdeq_X_emplace_back(cdeq_X* self, i_keyraw raw);
cdeq_X_value*       cdeq_X_emplace(cdeq_X* self, i_keyraw raw);                     // alias for emplace_back()
void                cdeq_X_pop_back(cdeq_X* self);
cdeq_X_value        cdeq_X_pull_back(cdeq_X* self);                                 // move out last element

cdeq_X_iter         cdeq_X_insert_n(cdeq_X* self, intptr_t idx, const i_key[] arr, intptr_t n);  // move values
cdeq_X_iter         cdeq_X_insert_at(cdeq_X* self, cdeq_X_iter it, i_key value);    // move value 
cdeq_X_iter         cdeq_X_insert_uninit(cdeq_X* self, intptr_t idx, intptr_t n);   // uninitialized data 
                    // copy values:
cdeq_X_iter         cdeq_X_emplace_n(cdeq_X* self, intptr_t idx, const i_keyraw[] arr, intptr_t n);
cdeq_X_iter         cdeq_X_emplace_at(cdeq_X* self, cdeq_X_iter it, i_keyraw raw);

void                cdeq_X_erase_n(cdeq_X* self, intptr_t idx, intptr_t n);
cdeq_X_iter         cdeq_X_erase_at(cdeq_X* self, cdeq_X_iter it);
cdeq_X_iter         cdeq_X_erase_range(cdeq_X* self, cdeq_X_iter it1, cdeq_X_iter it2);

cdeq_X_iter         cdeq_X_begin(const cdeq_X* self);
cdeq_X_iter         cdeq_X_end(const cdeq_X* self);
void                cdeq_X_next(cdeq_X_iter* it);
cdeq_X_iter         cdeq_X_advance(cdeq_X_iter it, intptr_t n);

bool                cdeq_X_eq(const cdeq_X* c1, const cdeq_X* c2); // require i_eq/i_cmp/i_less.
cdeq_X_value        cdeq_X_value_clone(cdeq_X_value val);
cdeq_X_raw          cdeq_X_value_toraw(const cdeq_X_value* pval);
void                cdeq_X_value_drop(cdeq_X_value* pval);
```

## Types

| Type name          | Type definition                     | Used to represent...   |
|:-------------------|:------------------------------------|:-----------------------|
| `cdeq_X`           | `struct { cdeq_X_value* data; }`    | The cdeq type          |
| `cdeq_X_value`     | `i_key`                             | The cdeq value type    |
| `cdeq_X_raw`       | `i_keyraw`                          | The raw value type     |
| `cdeq_X_iter`      | `struct { cdeq_X_value* ref; }`     | The iterator type      |

## Examples
```c
#define i_key int
#define i_tag i
#include <stc/cdeq.h>

#include <stdio.h>

int main(void) {
    cdeq_i q = cdeq_i_init();
    cdeq_i_push_front(&q, 10);
    c_foreach (i, cdeq_i, q)
        printf(" %d", *i.ref);
    puts("");

    c_forlist (i, int, {1, 4, 5, 22, 33, 2})
        cdeq_i_push_back(&q, *i.ref)

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
    cdeq_i_drop(&q);
}
```
Output:
```
 10
 10 1 4 5 22 33 2
 8 20 9 10 1 4 5 22 33 2 1
```
