# STC [cpque](../include/stc/cpque.h): Priority Queue

A priority queue is a container adaptor that provides constant time lookup of the largest (by default) element, at the expense of logarithmic insertion and extraction.
A user-provided ***i_cmp*** may be defined to set the ordering, e.g. using ***-c_default_cmp*** would cause the smallest element to appear as the top() value.

See the c++ class [std::priority_queue](https://en.cppreference.com/w/cpp/container/priority_queue) for a functional reference.

## Header file and declaration

```c
#define i_type      // define type name of the container (default cpque_{i_val})
#define i_val       // value: REQUIRED
#define i_less      // compare two i_val* : REQUIRED IF i_val/i_valraw is a non-integral type
#define i_valdrop   // destroy value func - defaults to empty destruct
#define i_valclone  // REQUIRED IF i_valdrop defined

#define i_valraw    // convertion type
#define i_valfrom   // convertion func i_valraw => i_val
#define i_valto     // convertion func i_val* => i_valraw.

#define i_tag       // alternative typename: cpque_{i_tag}. i_tag defaults to i_val
#include <stc/cpque.h>
```
`X` should be replaced by the value of `i_tag` in all of the following documentation.

## Methods

```c
cpque_X             cpque_X_init(void);                    // create empty pri-queue.
cpque_X             cpque_X_with_capacity(intptr_t cap);
cpque_X             cpque_X_with_size(intptr_t size, i_val null);
cpque_X             cpque_X_clone(cpque_X pq);

void                cpque_X_clear(cpque_X* self);
bool                cpque_X_reserve(cpque_X* self, intptr_t n);
void                cpque_X_shrink_to_fit(cpque_X* self);
void                cpque_X_copy(cpque_X* self, const cpque_X* other);
void                cpque_X_drop(cpque_X* self);        // destructor

intptr_t            cpque_X_size(const cpque_X* self);
bool                cpque_X_empty(const cpque_X* self);
i_val*              cpque_X_top(const cpque_X* self);

void                cpque_X_make_heap(cpque_X* self);  // heapify the vector.
void                cpque_X_push(cpque_X* self, i_val value);
void                cpque_X_emplace(cpque_X* self, i_valraw raw); // converts from raw

void                cpque_X_pop(cpque_X* self);
void                cpque_X_erase_at(cpque_X* self, intptr_t idx);

i_val               cpque_X_value_clone(i_val value);
```

## Types

| Type name          | Type definition                       | Used to represent...    |
|:-------------------|:--------------------------------------|:------------------------|
| `cpque_X`          | `struct {cpque_X_value* data; ...}`   | The cpque type          |
| `cpque_X_value`    | `i_val`                               | The cpque element type  |

## Example
```c
#include <stc/crand.h>
#include <stdio.h>

#define i_val int64_t
#define i_cmp -c_default_cmp // min-heap
#define i_tag i
#include <stc/cpque.h>

int main()
{
    intptr_t N = 10000000;
    crand_t rng = crand_init(1234);
    crand_unif_t dist = crand_unif_init(0, N * 10);

    // Define heap
    cpque_i heap = {0};

    // Push ten million random numbers to priority queue.
    c_forrange (N)
        cpque_i_push(&heap, crand_unif(&rng, &dist));

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
