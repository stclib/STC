# STC [pque](../include/stc/pque.h): Priority Queue

A priority queue is a container adaptor that provides constant time lookup of the largest (by default) element, at the expense of logarithmic insertion and extraction.
A user-provided ***i_cmp*** may be defined to set the ordering, e.g. using ***-c_default_cmp*** would cause the smallest element to appear as the top() value.

See the c++ class [std::priority_queue](https://en.cppreference.com/w/cpp/container/priority_queue) for a functional reference.

## Header file and declaration

```c
#define i_TYPE <ct>,<kt> // shorthand to define i_type,i_key
#define i_type <t>       // pque container type name (default: pque_{i_key})
#define i_key <t>        // element type: REQUIRED. Defines pque_X_value
#define i_less <f>       // compare two i_key* : REQUIRED IF i_key/i_keyraw is a non-integral type
#define i_keydrop <f>    // destroy value func - defaults to empty destruct
#define i_keyclone <f>   // REQUIRED IF i_keydrop defined

#define i_keyraw <t>     // convertion type
#define i_keyfrom <f>    // convertion func i_keyraw => i_key
#define i_keyto <f>      // convertion func i_key* => i_keyraw.

#define i_tag <s>        // alternative typename: pque_{i_tag}. i_tag defaults to i_key
#include "stc/pque.h"
```
`X` should be replaced by the value of `i_tag` in all of the following documentation.

## Methods

```c
pque_X              pque_X_init(void);                    // create empty pri-queue.
pque_X              pque_X_with_capacity(intptr_t cap);
pque_X              pque_X_with_size(intptr_t size, i_key null);
pque_X              pque_X_clone(pque_X pq);

void                pque_X_clear(pque_X* self);
bool                pque_X_reserve(pque_X* self, intptr_t n);
void                pque_X_shrink_to_fit(pque_X* self);
void                pque_X_copy(pque_X* self, const pque_X* other);
void                pque_X_drop(pque_X* self);        // destructor

intptr_t            pque_X_size(const pque_X* self);
bool                pque_X_empty(const pque_X* self);
const i_key*        pque_X_top(const pque_X* self);

void                pque_X_make_heap(pque_X* self);  // heapify the vector.
void                pque_X_push(pque_X* self, i_key value);
void                pque_X_emplace(pque_X* self, i_keyraw raw); // converts from raw

void                pque_X_pop(pque_X* self);
i_key               pque_X_pull(const pque_X* self);
void                pque_X_erase_at(pque_X* self, intptr_t idx);

i_key               pque_X_value_clone(i_key value);
```

## Types

| Type name         | Type definition                      | Used to represent...    |
|:------------------|:-------------------------------------|:------------------------|
| `pque_X`          | `struct {pque_X_value* data; ...}`   | The pque type          |
| `pque_X_value`    | `i_key`                              | The pque element type  |

## Example
```c
#include "stc/crand.h"
#include <stdio.h>

#define i_TYPE PriorityQ, int64_t
#define i_cmp -c_default_cmp // min-heap
#include "stc/pque.h"

int main(void)
{
    intptr_t N = 10000000;
    crand_t rng = crand_init(1234);
    crand_uniform_t dist = crand_uniform_init(0, N * 10);

    PriorityQ numbers = {0};

    // Push ten million random numbers to priority queue.
    c_forrange (N)
        PriorityQ_push(&numbers, crand_uniform(&rng, &dist));

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
