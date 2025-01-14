# STC [pqueue](../include/stc/pqueue.h): Priority Queue

A priority queue is a container adaptor that provides constant time lookup of the largest (by default) element, at the expense of logarithmic insertion and extraction.
A user-provided ***i_cmp*** may be defined to set the ordering, e.g. using ***-c_default_cmp*** would cause the smallest element to appear as the top() value.

See the c++ class [std::priority_queue](https://en.cppreference.com/w/cpp/container/priority_queue) for a functional reference.

## Header file and declaration

```c++
#define i_type <ct>,<kt> // shorthand for defining i_type, i_key
#define i_type <t>       // pqueue container type name (default: pqueue_{i_key})
// One of the following:
#define i_key <t>        // key type
#define i_keyclass <t>   // key type, and bind <t>_clone() and <t>_drop() function names
#define i_keypro <t>     // key "pro" type, use for cstr, arc, box types

#define i_use_cmp        // may be defined instead of i_cmp when i_key is an integral/native-type.
#define i_less <fn>      // less comparison. REQUIRED for non-integral types.
#define i_cmp <fn>       // three-way compare two i_keyraw*. Alternative to i_less.

#define i_keydrop <fn>   // destroy value func - defaults to empty destruct
#define i_keyclone <fn>  // REQUIRED IF i_keydrop defined

#define i_keyraw <t>     // convertion type
#define i_rawclass <t>   // convertion "raw class". binds <t>_cmp(),  <t>_eq(),  <t>_hash()
#define i_keyfrom <fn>   // convertion func i_keyraw => i_key
#define i_keytoraw <fn>  // convertion func i_key* => i_keyraw.

#include "stc/pqueue.h"
```
In the following, `X` is the value of `i_key` unless `i_type` is defined.

## Methods

```c++
pqueue_X        pqueue_X_init(void);                                // create empty pri-queue.
pqueue_X        pqueue_X_with_capacity(isize cap);
pqueue_X        pqueue_X_with_size(isize size, i_key null);

pqueue_X        pqueue_X_clone(pqueue_X pq);
void            pqueue_X_copy(pqueue_X* self, pqueue_X other);
void            pqueue_X_take(pqueue_X* self, pqueue_X unowned);    // take ownership of unowned
pqueue_X        pqueue_X_move(pqueue_X* self);                      // move
void            pqueue_X_drop(pqueue_X* self);                      // destructor

void            pqueue_X_clear(pqueue_X* self);
bool            pqueue_X_reserve(pqueue_X* self, isize n);
void            pqueue_X_shrink_to_fit(pqueue_X* self);

isize           pqueue_X_size(const pqueue_X* self);
bool            pqueue_X_is_empty(const pqueue_X* self);
const i_key*    pqueue_X_top(const pqueue_X* self);

void            pqueue_X_make_heap(pqueue_X* self);                 // heapify the vector.
void            pqueue_X_push(pqueue_X* self, i_key value);
void            pqueue_X_emplace(pqueue_X* self, i_keyraw raw);     // converts from raw

void            pqueue_X_pop(pqueue_X* self);
i_key           pqueue_X_pull(const pqueue_X* self);
void            pqueue_X_erase_at(pqueue_X* self, isize idx);

i_key           pqueue_X_value_clone(i_key value);
```

## Types

| Type name         | Type definition                      | Used to represent...    |
|:------------------|:-------------------------------------|:------------------------|
| `pqueue_X`        | `struct {pqueue_X_value* data; ...}` | The pqueue type          |
| `pqueue_X_value`  | `i_key`                              | The pqueue element type  |

## Example

[ [Run this code](https://godbolt.org/z/5rYPEaavW) ]
```c++
#include <stdio.h>
#include "stc/random.h"

#define i_type PriorityQ, int32
#define i_cmp -c_default_cmp // min-heap
#include "stc/pqueue.h"

int main(void)
{
    isize N = 10000000;
    PriorityQ numbers = {0};

    // Push ten million random numbers to priority queue.
    for (c_range(N/2))
        PriorityQ_push(&numbers, (int32)(crand32_uint() >> 6));

    // Add some negative ones.
    int32 nums[] = {-231, -32, -873, -4, -343};
    for (c_range(i, c_arraylen(nums)))
        PriorityQ_push(&numbers, nums[i]);

    for (c_range(N/2))
        PriorityQ_push(&numbers, (int32)(crand32_uint() >> 6));

    // Extract and display the fifty smallest.
    for (c_range(50)) {
        printf("%d ", *PriorityQ_top(&numbers));
        PriorityQ_pop(&numbers);
    }
    PriorityQ_drop(&numbers);
}
```
