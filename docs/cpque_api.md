# STC [cpque](../include/stc/cpque.h): Priority Queue

A priority queue is a container adaptor that provides constant time lookup of the largest (by default) element, at the expense of logarithmic insertion and extraction.
A user-provided ***i_cmp*** may be defined to set the ordering, e.g. using ***-c_default_cmp*** would cause the smallest element to appear as the top() value.

See the c++ class [std::priority_queue](https://en.cppreference.com/w/cpp/container/priority_queue) for a functional reference.

## Header file and declaration

```c
#define i_val       // value: REQUIRED
#define i_cmp       // three-way compare two i_valraw* : REQUIRED IF i_valraw is a non-integral type
#define i_del       // destroy value func - defaults to empty destruct
#define i_valraw    // convertion "raw" type - defaults to i_val
#define i_valfrom   // convertion func i_valraw => i_val - defaults to plain copy
#define i_valto     // convertion func i_val* => i_valraw - defaults to plain copy
#define i_tag       // defaults to i_val
#include <stc/cpque.h>
```
`X` should be replaced by the value of `i_tag` in all of the following documentation.

## Methods

```c
cpque_X             cpque_X_init(void);
cpque_X             cpque_X_clone(cpque_X pq);

void                cpque_X_clear(cpque_X* self);
void                cpque_X_reserve(cpque_X* self, size_t n);
void                cpque_X_copy(cpque_X* self, cpque_X other);
void                cpque_X_del(cpque_X* self);        // destructor

size_t              cpque_X_size(cpque_X pq);
bool                cpque_X_empty(cpque_X pq);
cpque_X_value_t*    cpque_X_top(const cpque_X* self);

void                cpque_X_make_heap(cpque_X* self);  // call after using push_back().
void                cpque_X_push(cpque_X* self, cpque_X_value_t value);
void                cpque_X_emplace(cpque_X* self, cpque_X_rawvalue_t raw);

void                cpque_X_pop(cpque_X* self);
void                cpque_X_erase_at(cpque_X* self, size_t idx);

void                cpque_X_push_back(cpque_X* self, cpque_X_value_t value); // breaks heap-property
cpque_X_value_t     cpque_X_value_clone(cpque_X_value_t val);
```

## Types

| Type name            | Type definition                       | Used to represent...    |
|:---------------------|:--------------------------------------|:------------------------|
| `cpque_X`            | `struct {cpque_X_value_t* data; ...}` | The cpque type          |
| `cpque_X_value_t`    | `i_val`                               | The cpque element type  |
| `cpque_X_rawvalue_t` | `i_valraw`                            | cpque raw value type    |

## Example
```c
#include <stc/crandom.h>
#include <stdio.h>

#define i_val int64_t
#define i_cmp -c_default_compare // min-heap
#define i_tag i
#include <stc/cpque.h>

int main()
{
    size_t N = 10000000;
    stc64_t rng = stc64_init(1234);
    stc64_uniform_t dist = stc64_uniform_init(0, N * 10);

    // Declare heap, with defered del()
    c_auto (cpque_i, heap)
    {
        // Push ten million random numbers to priority queue, plus some negative ones.
        c_forrange (N)
            cpque_i_push(&heap, stc64_uniform(&rng, &dist));
        c_apply(cpque_i, push, &heap, {-231, -32, -873, -4, -343});

        // Extract and display the fifty smallest.
        c_forrange (50) {
            printf("%zd ", *cpque_i_top(&heap));
            cpque_i_pop(&heap);
        }
    }
}
```
Output:
```
 -873 -343 -231 -32 -4 3 5 6 18 23 31 54 68 87 99 105 107 125 128 147 150 155 167 178 181 188 213 216 272 284 287 302 306 311 313 326 329 331 344 348 363 367 374 385 396 399 401 407 412 477
```
