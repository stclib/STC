# STC [pqueue](../include/stc/pqueue.h): Priority Queue

A priority queue is a container adaptor that provides constant time lookup of the largest (by default) element, at the expense of logarithmic insertion and extraction.
A user-provided ***i_cmp*** may be defined to set the ordering, e.g. using ***-c_default_cmp*** would cause the smallest element to appear as the top() value.

See the c++ class [std::priority_queue](https://en.cppreference.com/w/cpp/container/priority_queue) for a functional reference.

## Header file and declaration

```c++
#define T <ct>, <kt>[, (<opt>)] // shorthand for defining pqueue name, i_key, and i_opt
// Common <opt> traits:
//   c_keycomp  - Key <kt> is a comparable typedef'ed type.
//                Binds <kt>_cmp() "member" function name.
//   c_keyclass - Additionally binds <kt>_clone() and <kt>_drop() function names.
//                All containers used as keys themselves can be specified with the c_keyclass trait.
//   c_keypro   - "Pro" key type, use e.g. for built-in `cstr`, `zsview`, `arc`, and `box` as keys.
//                These support conversion to/from a "raw" input type (such as const char*) when
//                using <ct>_emplace*() functions, and may do optimized lookups via the raw type.

// Alternative to defining T:
#define i_key <kt>       // Key type. Container type name <ct> defaults to pqueue_<kt>.

// Override/define when not the <opt> traits are specified:
#define i_cmp <fn>       // Three-way comparison of two i_key (or i_keyraw)
#define i_less <fn>      // Alternative less-comparison. i_cmp is deduced from i_less.
#define i_eq <fn>        // Optional equality comparison, otherwise deduced from given i_cmp.
#define i_keydrop <fn>   // Destroy-element function - defaults to empty destruct
#define i_keyclone <fn>  // Required if i_keydrop is defined

#include <stc/pqueue.h>
```
In the following, `X` is the value of `i_key` unless `T` is defined.

## Methods

```c++
pqueue_X        pqueue_X_init(void);                                // create empty pri-queue.
pqueue_X        pqueue_X_with_capacity(isize_t cap);

pqueue_X        pqueue_X_clone(pqueue_X pq);
void            pqueue_X_copy(pqueue_X* self, const pqueue_X* other);
void            pqueue_X_take(pqueue_X* self, pqueue_X unowned);    // take ownership of unowned
pqueue_X        pqueue_X_move(pqueue_X* self);                      // move
void            pqueue_X_drop(const pqueue_X* self);                // destructor

void            pqueue_X_clear(pqueue_X* self);
bool            pqueue_X_reserve(pqueue_X* self, isize_t n);
void            pqueue_X_shrink_to_fit(pqueue_X* self);

isize_t         pqueue_X_size(const pqueue_X* self);
bool            pqueue_X_is_empty(const pqueue_X* self);
const i_key*    pqueue_X_top(const pqueue_X* self);

void            pqueue_X_make_heap(pqueue_X* self);                 // heapify the vector.
void            pqueue_X_push(pqueue_X* self, i_key value);
void            pqueue_X_emplace(pqueue_X* self, i_keyraw raw);     // converts from raw

void            pqueue_X_pop(pqueue_X* self);
i_key           pqueue_X_pull(const pqueue_X* self);
void            pqueue_X_erase_at(pqueue_X* self, isize_t idx);

bool            pqueue_X_eq(const pqueue_X* c1, const pqueue_X* c2); // equality comp.
i_key           pqueue_X_value_clone(const pqueue_X* self, i_key value);
```

## Types

| Type name         | Type definition                      | Used to represent...    |
|:------------------|:-------------------------------------|:------------------------|
| `pqueue_X`        | `struct {pqueue_X_value* data; ...}` | The pqueue type          |
| `pqueue_X_value`  | `i_key`                              | The pqueue element type  |

## Example

[ [Run this code](https://godbolt.org/z/bGb3n4sE8) ]
```c++
#include <stdio.h>
#include <stc/random.h>

#define T PriorityQ, int
#define i_cmp -c_default_cmp // min-heap
#include <stc/pqueue.h>

int main(void)
{
    isize_t N = 10000000;
    PriorityQ numbers = {0};

    // Push ten million random numbers to priority queue.
    for (c_range(N/2))
        PriorityQ_push(&numbers, (int)(crand32_uint() >> 6));

    // Add some negative ones.
    int nums[] = {-231, -32, -873, -4, -343};
    for (c_range(i, c_countof(nums)))
        PriorityQ_push(&numbers, nums[i]);

    for (c_range(N/2))
        PriorityQ_push(&numbers, (int)(crand32_uint() >> 6));

    // Extract and display the fifty smallest.
    for (c_range(50)) {
        printf("%d ", *PriorityQ_top(&numbers));
        PriorityQ_pop(&numbers);
    }
    PriorityQ_drop(&numbers);
}
```
