# STC [cpque](../include/stc/cpque.h): Priority Queue

A priority queue is a container adaptor that provides constant time lookup of the largest (by default) element, at the expense of logarithmic insertion and extraction.
A user-provided ***i_cmp*** may be defined to set the ordering, e.g. using ***-c_default_cmp*** would cause the smallest element to appear as the top() value.

See the c++ class [std::priority_queue](https://en.cppreference.com/w/cpp/container/priority_queue) for a functional reference.

## Header file and declaration

```c
#define i_key <t>      // element type: REQUIRED. Note: i_val* may be specified instead of i_key*.
#define i_type <t>     // cpque container type name
#define i_less <f>     // compare two i_key* : REQUIRED IF i_key/i_keyraw is a non-integral type
#define i_keydrop <f>  // destroy value func - defaults to empty destruct
#define i_keyclone <f> // REQUIRED IF i_keydrop defined

#define i_keyraw <t>   // convertion type
#define i_keyfrom <f>  // convertion func i_keyraw => i_key
#define i_keyto <f>    // convertion func i_key* => i_keyraw.

#define i_tag <s>      // alternative typename: cpque_{i_tag}. i_tag defaults to i_key
#include <stc/cpque.h>
```
`X` should be replaced by the value of `i_tag` in all of the following documentation.

## Methods

```c
cpque_X             cpque_X_init(void);                    // create empty pri-queue.
cpque_X             cpque_X_with_capacity(intptr_t cap);
cpque_X             cpque_X_with_size(intptr_t size, i_key null);
cpque_X             cpque_X_clone(cpque_X pq);

void                cpque_X_clear(cpque_X* self);
bool                cpque_X_reserve(cpque_X* self, intptr_t n);
void                cpque_X_shrink_to_fit(cpque_X* self);
void                cpque_X_copy(cpque_X* self, const cpque_X* other);
void                cpque_X_drop(cpque_X* self);        // destructor

intptr_t            cpque_X_size(const cpque_X* self);
bool                cpque_X_empty(const cpque_X* self);
i_key*              cpque_X_top(const cpque_X* self);

void                cpque_X_make_heap(cpque_X* self);  // heapify the vector.
void                cpque_X_push(cpque_X* self, i_key value);
void                cpque_X_emplace(cpque_X* self, i_keyraw raw); // converts from raw

void                cpque_X_pop(cpque_X* self);
void                cpque_X_erase_at(cpque_X* self, intptr_t idx);

i_key               cpque_X_value_clone(i_key value);
```

## Types

| Type name          | Type definition                       | Used to represent...    |
|:-------------------|:--------------------------------------|:------------------------|
| `cpque_X`          | `struct {cpque_X_value* data; ...}`   | The cpque type          |
| `cpque_X_value`    | `i_key`                               | The cpque element type  |

## Example
```c
#include <stc/crand.h>
#include <stdio.h>

#define i_key int64_t
#define i_cmp -c_default_cmp // min-heap
#define i_tag i
#include <stc/cpque.h>

int main(void)
{
    intptr_t N = 10000000;
    crand_t rng = crand_init(1234);
    crand_uniform_t dist = crand_uniform_init(0, N * 10);

    // Define heap
    cpque_i heap = {0};

    // Push ten million random numbers to priority queue.
    c_forrange (N)
        cpque_i_push(&heap, crand_uniform(&rng, &dist));

    // Add some negative ones.
    int nums[] = {-231, -32, -873, -4, -343};
    c_forrange (i, c_arraylen(nums)) 
        cpque_i_push(&heap, nums[i]);

    // Extract and display the fifty smallest.
    c_forrange (50) {
        printf("%" PRId64 " ", *cpque_i_top(&heap));
        cpque_i_pop(&heap);
    }
    cpque_i_drop(&heap);
}
```
Output:
```
 -873 -343 -231 -32 -4 3 5 6 18 23 31 54 68 87 99 105 107 125 128 147 150 155 167 178 181 188 213 216 272 284 287 302 306 311 313 326 329 331 344 348 363 367 374 385 396 399 401 407 412 477
```
