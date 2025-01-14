# STC [deque](../include/stc/deque.h): Double Ended Queue
![Deque](pics/deque.jpg)

A **deque** is an indexed sequence container that allows fast insertion and deletion at both
its beginning and its end, but has also fast random access to elements. The container is
implemented as a circular dynamic buffer. Iterators may be invalidated after push-operations.

See the c++ class [std::deque](https://en.cppreference.com/w/cpp/container/deque) for a functional description.

## Header file and declaration

```c++
#define i_type <ct>,<kt> // shorthand for defining i_type, i_key
#define i_type <t>       // deque container type name (default: deque_{i_key})
// One of the following:
#define i_key <t>        // key type
#define i_keyclass <t>   // key type, and bind <t>_clone() and <t>_drop() function names
#define i_keypro <t>     // key "pro" type, use for cstr, arc, box types

#define i_keydrop <fn>   // destroy value func - defaults to empty destruct
#define i_keyclone <fn>  // REQUIRED IF i_keydrop is defined

#define i_use_cmp        // enable sorting, binary_search and lower_bound
#define i_cmp <fn>       // three-way compare two i_keyraw's
#define i_less <fn>      // less comparison. Alternative to i_cmp
#define i_eq <fn>        // equality comparison. Implicitly defined with i_cmp, but not i_less.

#define i_keyraw <t>     // convertion "raw" type - defaults to i_key
#define i_rawclass <t>   // convertion "raw class". binds <t>_cmp(),  <t>_eq(),  <t>_hash()
#define i_keyfrom <fn>   // convertion func i_keyraw => i_key
#define i_keytoraw <fn>  // convertion func i_key* => i_keyraw

#include "stc/deque.h"
```
- Defining either `i_use_cmp`, `i_less` or `i_cmp` will enable sorting, binary_search and lower_bound
- **emplace**-functions are only available when `i_keyraw` is implicitly or explicitly defined.
- In the following, `X` is the value of `i_key` unless `i_type` is defined.

## Methods

```c++
deque_X         deque_X_init(void);
deque_X         deque_X_with_capacity(isize size);

deque_X         deque_X_clone(deque_X deque);
void            deque_X_copy(deque_X* self, deque_X other);
void            deque_X_take(deque_X* self, deque_X unowned);                    // take ownership of unowned
deque_X         deque_X_move(deque_X* self);                                     // move
void            deque_X_drop(deque_X* self);                                     // destructor

void            deque_X_clear(deque_X* self);
bool            deque_X_reserve(deque_X* self, isize cap);
void            deque_X_shrink_to_fit(deque_X* self);

bool            deque_X_is_empty(const deque_X* self);
isize           deque_X_size(const deque_X* self);
isize           deque_X_capacity(const deque_X* self);

deque_X_iter    deque_X_find(const deque_X* self, i_keyraw raw);
deque_X_iter    deque_X_find_in(deque_X_iter i1, deque_X_iter i2, i_keyraw raw); // return vec_X_end() if not found

const i_key*    deque_X_at(const deque_X* self, isize idx);
const i_key*    deque_X_front(const deque_X* self);
const i_key*    deque_X_back(const deque_X* self);

i_key*          deque_X_at_mut(deque_X* self, isize idx);
i_key*          deque_X_front_mut(deque_X* self);
i_key*          deque_X_back_mut(deque_X* self);

                // Requires either i_use_cmp, i_cmp or i_less defined:
void            deque_X_sort(deque_X* self);                                     // quicksort from sort.h
isize           deque_X_lower_bound(const deque_X* self, const i_keyraw raw);    // return c_NPOS if not found
isize           deque_X_binary_search(const deque_X* self, const i_keyraw raw);  // return c_NPOS if not found

i_key*          deque_X_push_front(deque_X* self, i_key value);
i_key*          deque_X_emplace_front(deque_X* self, i_keyraw raw);
void            deque_X_pop_front(deque_X* self);
i_key           deque_X_pull_front(deque_X* self);                               // move out front element

i_key*          deque_X_push_back(deque_X* self, i_key value);
i_key*          deque_X_push(deque_X* self, i_key value);                        // alias for push_back()
i_key*          deque_X_emplace_back(deque_X* self, i_keyraw raw);
i_key*          deque_X_emplace(deque_X* self, i_keyraw raw);                    // alias for emplace_back()
void            deque_X_pop_back(deque_X* self);                                 // remove and destroy back()
i_key           deque_X_pull_back(deque_X* self);                                // move out last element

deque_X_iter    deque_X_insert_n(deque_X* self, isize idx, const i_key[] arr, isize n);  // move values
deque_X_iter    deque_X_insert_at(deque_X* self, deque_X_iter it, i_key value);  // move value
deque_X_iter    deque_X_insert_uninit(deque_X* self, isize idx, isize n);        // uninitialized data

deque_X_iter    deque_X_emplace_n(deque_X* self, isize idx, const i_keyraw[] arr, isize n);
deque_X_iter    deque_X_emplace_at(deque_X* self, deque_X_iter it, i_keyraw raw);

void            deque_X_erase_n(deque_X* self, isize idx, isize n);
deque_X_iter    deque_X_erase_at(deque_X* self, deque_X_iter it);
deque_X_iter    deque_X_erase_range(deque_X* self, deque_X_iter it1, deque_X_iter it2);

deque_X_iter    deque_X_begin(const deque_X* self);
deque_X_iter    deque_X_end(const deque_X* self);
void            deque_X_next(deque_X_iter* it);
deque_X_iter    deque_X_advance(deque_X_iter it, isize n);

bool            deque_X_eq(const deque_X* c1, const deque_X* c2); // require i_eq/i_cmp/i_less.
i_key           deque_X_value_clone(i_key val);
deque_X_raw     deque_X_value_toraw(const i_key* pval);
void            deque_X_value_drop(i_key* pval);
```
## Types

| Type name         | Type definition                    | Used to represent...   |
|:------------------|:-----------------------------------|:-----------------------|
| `deque_X`         | `struct { deque_X_value* data; }`  | The deque type         |
| `deque_X_value`   | `i_key`                            | The deque value type   |
| `deque_X_raw`     | `i_keyraw`                         | The raw value type     |
| `deque_X_iter`    | `struct { deque_X_value* ref; }`   | The iterator type      |

## Examples

[ [Run this code](https://godbolt.org/z/jjsdWaKj6) ]
```c++
#define i_type Deque, int32_t
#include "stc/deque.h"
#include <stdio.h>

int main(void) {
    Deque q = {0};
    Deque_push_front(&q, 10);
    for (c_each(i, Deque, q))
        printf(" %d", *i.ref);
    puts("");

    for (c_items(i, int, {1, 4, 5, 22, 33, 2}))
        Deque_push_back(&q, *i.ref);

    for (c_each(i, Deque, q))
        printf(" %d", *i.ref);
    puts("");

    Deque_push_front(&q, 9);
    Deque_push_front(&q, 20);
    Deque_push_back(&q, 11);
    Deque_push_front(&q, 8);

    for (c_each(i, Deque, q))
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
