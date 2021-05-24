# STC [cpque](../include/stc/cpque.h): Priority Queue

A priority queue is a container adaptor that provides constant time lookup of the largest (by default) element, at the expense of logarithmic insertion and extraction.
A user-provided argument `<`or `>` must be supplied to set the ordering, e.g. using `>` would cause the smallest element to appear as the top().

See the c++ class [std::priority_queue](https://en.cppreference.com/w/cpp/container/priority_queue) for a functional reference.

## Header file and declaration

```c
#include <stc/cpque.h>

using_cpque(X, ctype);                // uses valueCompare from ctype
using_cpque(X, ctype, valueCompare);
```
The macro `using_cpque()` must be instantiated in the global scope. **cpque** uses normally **cvec_X**
or **cdeq_X** as underlying implementation, specified as `ctype`. The *valueCompare* can be specified
to control the order in the priority queue.

By default, the function *`ctype`_value_compare(x, y)* from the underlying vector type is used for
comparing values (priorities). `X` is a type tag name and will affect the names of all cpque types and methods.
When declaring `using_cpque(i, cvec_i)`, `X` should be replaced by `i` in the following documentation.

## Methods

```c
cpque_X                 cpque_X_init(void);
cpque_X                 cpque_X_clone(cpque_X pq);

void                    cpque_X_clear(cpque_X* self);
void                    cpque_X_make_heap(cpque_X* self);
void                    cpque_X_del(cpque_X* self);      // destructor

size_t                  cpque_X_size(cpque_X pq);
bool                    cpque_X_empty(cpque_X pq);
const cpque_X_value_t*  cpque_X_top(const cpque_X* self);

void                    cpque_X_push(cpque_X* self, cpque_X_value_t value);
void                    cpque_X_emplace(cpque_X* self, cpque_X_rawvalue_t raw);
void                    cpque_X_emplace_items(cpque_X *self, const cpque_X_rawvalue_t arr[], size_t n);

void                    cpque_X_pop(cpque_X* self);
void                    cpque_X_erase_at(cpque_X* self, size_t idx);

cpque_X_value_t         cpque_X_value_clone(cpque_X_value_t val);
```

## Types

| Type name            | Type definition                       | Used to represent...    |
|:---------------------|:--------------------------------------|:------------------------|
| `cpque_X`            | `struct {cpque_X_value_t* data; ...}` | The cpque type          |
| `cpque_X_value_t`    | Depends on underlying container type  | The cpque element type  |
| `cpque_X_rawvalue_t` |                   "                   | cpque raw value type    |

## Example
```c
#include <stc/cpque.h>
#include <stc/crandom.h>
#include <stdio.h>

using_cvec(i, int64_t);
using_cpque(i, cvec_i, -c_default_compare); // min-heap

int main()
{
    size_t N = 10000000;
    stc64_t rng = stc64_init(1234);
    stc64_uniform_t dist = stc64_uniform_init(0, N * 10);

    // Declare heap, with defered del()
    c_forvar (cpque_i heap = cpque_i_init(), cpque_i_del(&heap))
    {
        // Push ten million random numbers to priority queue, plus some negative ones.
        c_forrange (N)
            cpque_i_push(&heap, stc64_uniform(&rng, &dist));
        c_emplace(cpque_i, heap, {-231, -32, -873, -4, -343});

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
