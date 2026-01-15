# STC [deque](../include/stc/deque.h): Double Ended Queue
![Deque](pics/deque.jpg)

A **deque** is an indexed sequence container that allows fast insertion and deletion at both
its beginning and its end, but has also fast random access to elements. The container is
implemented as a circular dynamic buffer. Iterators may be invalidated after push-operations.

See the c++ class [std::deque](https://en.cppreference.com/w/cpp/container/deque) for a functional description.

## Header file and declaration

```c++
#define T <ct>, <kt>[, (<opt>)] // shorthand for defining deque name, i_key, and i_opt
// Common <opt> traits:
//   c_keycomp  - Key <kt> is a comparable typedef'ed type.
//                Binds <kt>_cmp() "member" function name.
//   c_keyclass - Additionally binds <kt>_clone() and <kt>_drop() function names.
//                All containers used as keys themselves can be specified with the c_keyclass trait.
//   c_keypro   - "Pro" key type, use e.g. for built-in `cstr`, `zsview`, `arc`, and `box` as keys.
//                These support conversion to/from a "raw" input type (such as const char*) when
//                using <ct>_emplace*() functions, and may do optimized lookups via the raw type.
//   c_use_cmp  - Enable element sorting with  <kt>_cmp() function. Implies c_use_eq.
//                If <kt> is a basic type, operators '<' and '==' are used by default.
//   c_use_eq   - Enable <kt>_eq() for linear search and equality comparison of the container itself.
//
// To enable multiple traits, specify e.g. (c_keyclass | c_use_cmp) as <opt>.

// Alternative to defining T:
#define i_key <kt>       // Key type. Container type name <ct> defaults to deque_<kt>.

// Override/define when not the <opt> traits are specified:
#define i_cmp <fn>       // Three-way comparison of two i_key (or i_keyraw)
#define i_less <fn>      // Alternative less-comparison. i_cmp is deduced from i_less.
#define i_eq <fn>        // Optional equality comparison, otherwise deduced from given i_cmp.
#define i_keydrop <fn>   // Destroy-element function - defaults to empty destruct
#define i_keyclone <fn>  // Required if i_keydrop is defined

#include <stc/deque.h>
```
- Defining either `i_use_cmp`, `i_less` or `i_cmp` enables sort(), binary_search() and lower_bound().
- **emplace**-functions are only available when `i_keyraw` is explicitly or implicitly defined (e.g. via c_keypro).
- In the following, `X` is the value of `i_key` unless `T` is defined.

## Methods

```c++
deque_X         deque_X_init(void);
deque_X         deque_X_with_capacity(isize_t cap);
deque_X         deque_X_with_size(isize_t size, i_keyraw value);
deque_X         deque_X_with_size_uninit(isize_t size);

deque_X         deque_X_clone(deque_X deque);
void            deque_X_copy(deque_X* self, const deque_X* other);
void            deque_X_take(deque_X* self, deque_X unowned);                    // take ownership of unowned
deque_X         deque_X_move(deque_X* self);                                     // move
void            deque_X_drop(const deque_X* self);                               // destructor

void            deque_X_clear(deque_X* self);
bool            deque_X_reserve(deque_X* self, isize_t cap);
void            deque_X_shrink_to_fit(deque_X* self);

bool            deque_X_is_empty(const deque_X* self);
isize_t         deque_X_size(const deque_X* self);
isize_t         deque_X_capacity(const deque_X* self);

deque_X_iter    deque_X_find(const deque_X* self, i_keyraw raw);
deque_X_iter    deque_X_find_in(deque_X_iter i1, deque_X_iter i2, i_keyraw raw); // return vec_X_end() if not found

const i_key*    deque_X_at(const deque_X* self, isize_t idx);
const i_key*    deque_X_front(const deque_X* self);
const i_key*    deque_X_back(const deque_X* self);

i_key*          deque_X_at_mut(deque_X* self, isize_t idx);
i_key*          deque_X_front_mut(deque_X* self);
i_key*          deque_X_back_mut(deque_X* self);

                // Requires either i_use_cmp, i_cmp or i_less defined:
void            deque_X_sort(deque_X* self);                                     // quicksort from sort.h
isize_t         deque_X_lower_bound(const deque_X* self, const i_keyraw raw);    // return c_NPOS if not found
isize_t         deque_X_binary_search(const deque_X* self, const i_keyraw raw);  // return c_NPOS if not found

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

deque_X_iter    deque_X_insert_n(deque_X* self, isize_t idx, const i_key[] arr, isize_t n);  // move values
deque_X_iter    deque_X_insert_at(deque_X* self, deque_X_iter it, i_key value);  // move value
deque_X_iter    deque_X_insert_uninit(deque_X* self, isize_t idx, isize_t n);        // uninitialized data

deque_X_iter    deque_X_emplace_n(deque_X* self, isize_t idx, const i_keyraw[] arr, isize_t n);
deque_X_iter    deque_X_emplace_at(deque_X* self, deque_X_iter it, i_keyraw raw);

void            deque_X_erase_n(deque_X* self, isize_t idx, isize_t n);
deque_X_iter    deque_X_erase_at(deque_X* self, deque_X_iter it);
deque_X_iter    deque_X_erase_range(deque_X* self, deque_X_iter it1, deque_X_iter it2);

deque_X_iter    deque_X_begin(const deque_X* self);
deque_X_iter    deque_X_end(const deque_X* self);
void            deque_X_next(deque_X_iter* it);
deque_X_iter    deque_X_advance(deque_X_iter it, isize_t n);

bool            deque_X_eq(const deque_X* c1, const deque_X* c2); // require i_eq/i_cmp/i_less.
i_key           deque_X_value_clone(const deque_X* self, i_key val);
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

[ [Run this code](https://godbolt.org/z/1TTT68fv5) ]
```c++
#define T Deque, int32_t
#include <stc/deque.h>
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
