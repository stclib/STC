# STC [pqueue](../include/stc/pqueue.h): Priority Queue

A priority queue is a container adaptor that provides constant time lookup of the largest (by default) element, at the expense of logarithmic insertion and extraction.
A user-provided ***i_cmp*** may be defined to set the ordering, e.g. using ***-c_default_cmp*** would cause the smallest element to appear as the top() value.

See the c++ class [std::priority_queue](https://en.cppreference.com/w/cpp/container/priority_queue) for a functional reference.

## Header file and declaration

```c
#define i_type <ct>,<kt> // shorthand to define i_type, i_key
#define i_type <t>       // pqueue container type name (default: pqueue_{i_key})
#define i_key <t>        // element type: REQUIRED. Defines pqueue_X_value
#define i_less <fn>      // compare two i_key* : REQUIRED IF i_key/i_keyraw is a non-integral type
#define i_keydrop <fn>   // destroy value func - defaults to empty destruct
#define i_keyclone <fn>  // REQUIRED IF i_keydrop defined

#define i_keyraw <t>     // convertion type
#define i_keyfrom <fn>   // convertion func i_keyraw => i_key
#define i_keytoraw <fn>  // convertion func i_key* => i_keyraw.

#include "stc/pqueue.h"
```
In the following, `X` is the value of `i_key` unless `i_type` is specified.

## Methods

```c
pqueue_X        pqueue_X_init(void);                            // create empty pri-queue.
pqueue_X        pqueue_X_with_capacity(isize cap);
pqueue_X        pqueue_X_with_size(isize size, i_key null);
pqueue_X        pqueue_X_clone(pqueue_X pq);

void            pqueue_X_clear(pqueue_X* self);
bool            pqueue_X_reserve(pqueue_X* self, isize n);
void            pqueue_X_shrink_to_fit(pqueue_X* self);
void            pqueue_X_copy(pqueue_X* self, const pqueue_X* other);
void            pqueue_X_drop(pqueue_X* self);                  // destructor

isize           pqueue_X_size(const pqueue_X* self);
bool            pqueue_X_is_empty(const pqueue_X* self);
const i_key*    pqueue_X_top(const pqueue_X* self);

void            pqueue_X_make_heap(pqueue_X* self);             // heapify the vector.
void            pqueue_X_push(pqueue_X* self, i_key value);
void            pqueue_X_emplace(pqueue_X* self, i_keyraw raw); // converts from raw

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
```c
#include "stc/random.h"
#include <stdio.h>

#define i_type PriorityQ, int64_t
#define i_cmp -c_default_cmp // min-heap
#include "stc/pqueue.h"

int main(void)
{
    isize N = 10000000;
    PriorityQ numbers = {0};

    // Push ten million random numbers to priority queue.
    c_forrange (N)
        PriorityQ_push(&numbers, crand64_uint() & ((1 << 21) - 1));

    // Add some negative ones.
    int nums[] = {-231, -32, -873, -4, -343};
    c_forrange (i, c_arraylen(nums))
        PriorityQ_push(&numbers, nums[i]);

    // Extract and display the fifty smallest.
    c_forrange (50) {
        printf("%" PRId64 " ", *PriorityQ_top(&numbers));
        PriorityQ_pop(&numbers);
    }
    PriorityQ_drop(&numbers);
}
```
Output:
```
 -873 -343 -231 -32 -4 3 5 6 18 23 31 54 68 87 99 105 107 125 128 147 150 155 167 178 181 188 213 216 272 284 287 302 306 311 313 326 329 331 344 348 363 367 374 385 396 399 401 407 412 477
```
