# Container cpqueue: Priority Queue

This describes the API of the queue type **cpqueue**. Implemented as a heap.

## Declaration

```c
#define using_cpqueue(X, CType, MaxOrMinHeap)
```
The macro `using_cpqueue()` must be instantiated in the global scope.
**cpqueue** uses normally a **cvec** type as underlying implementation, specified as CType.
The MaxOrMinHeap argument should be specified as a < or >, representing a max-heap or a min-heap.
`X` is a type tag name and will affect the names of all cpqueue types and methods.
Declaring `using_cpqueue(my, cvec_my, >);`, `X` should be replaced by `my` in the following documentation.

## Types

| Type name              | Type definition                        | Used to represent...                |
|:-----------------------|:---------------------------------------|:------------------------------------|
| `cpqueue_X`            | Depends on underlying container type   | The cpqueue type                     |
| `cpqueue_X_value_t`    |                                        | The cpqueue element type             |
| `cpqueue_X_input_t`    |                                        | cpqueue input type                   |
| `cpqueue_X_rawvalue_t` |                                        | cpqueue raw value type               |
| `cpqueue_X_iter_t`     |                                        | cpqueue iterator                     |

## Header file

All cpqueue definitions and prototypes may be included in your C source file by including a single header file.

```c
#include "stc/cpqueue.h"
```

## Methods

```c
cpqueue_X               cpqueue_X_init(void);
void                    cpqueue_X_make_heap(cpqueue_X* self);
void                    cpqueue_X_del(cpqueue_X* self);

size_t                  cpqueue_X_size(cpqueue_X pq);
bool                    cpqueue_X_empty(cpqueue_X pq);
const
cpqueue_X_value_t*      cpqueue_X_top(const cpqueue_X* self);

void                    cpqueue_X_push_n(cpqueue_X *self, const cpqueue_X_input_t in[], size_t size);
void                    cpqueue_X_emplace(cpqueue_X* self, cpqueue_X_rawvalue_t raw);
void                    cpqueue_X_push(cpqueue_X* self, cpqueue_X_value_t value);
void                    cpqueue_X_pop(cpqueue_X* self);
void                    cpqueue_X_erase_at(cpqueue_X* self, size_t i);

cpqueue_X_iter_t        cpqueue_X_begin(cpqueue_X* self);
cpqueue_X_iter_t        cpqueue_X_end(cpqueue_X* self);
void                    cpqueue_X_next(cpqueue_X_iter_t* it);
cpqueue_X_value_t*      cpqueue_X_itval(cpqueue_X_iter_t it);
```

## Example
```c
#include <stdio.h>
#include "stc/cpqueue.h"
#include "stc/crandom.h"

using_cvec(i, int64_t);
using_cpqueue(i, cvec_i, >); // adaptor type, '>' = min-heap

int main()
{
    size_t N = 10000000;
    crand_rng64_t rng = crand_rng64_init(1234);
    crand_uniform_i64_t dist = crand_uniform_i64_init(0, N * 10);

    cpqueue_i heap = cpqueue_i_init();
    // Push ten million random numbers to priority queue, plus some negative ones.
    c_forrange (N)
        cpqueue_i_push(&heap, crand_uniform_i64(&rng, &dist));
    c_push_items(&heap, cpqueue_i, {-231, -32, -873, -4, -343});

    // Extract and display the fifty smallest.
    c_forrange (50) {
        printf("%zd ", *cpqueue_i_top(&heap));
        cpqueue_i_pop(&heap);
    }
    cpqueue_i_del(&heap);
}
```
Output:
```
 -873 -343 -231 -32 -4 3 5 6 18 23 31 54 68 87 99 105 107 125 128 147 150 155 167 178 181 188 213 216 272 284 287 302 306 311 313 326 329 331 344 348 363 367 374 385 396 399 401 407 412 477
```