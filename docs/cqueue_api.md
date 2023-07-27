# STC [cqueue](../include/stc/cqueue.h): Queue
![Queue](pics/queue.jpg)

The **cqueue** is container that gives the programmer the functionality of a queue - specifically, a FIFO (first-in, first-out) data structure. The queue pushes the elements on the back of the underlying container and pops them from the front.

See the c++ class [std::queue](https://en.cppreference.com/w/cpp/container/queue) for a functional reference.

## Header file and declaration
```c
#define i_key <t>      // element type: REQUIRED. Note: i_val* may be specified instead of i_key*.
#define i_type <t>     // cqueue container type name
#define i_keydrop <f>  // destroy value func - defaults to empty destruct
#define i_keyclone <f> // REQUIRED IF i_keydrop defined

#define i_keyraw <t>   // convertion "raw" type - defaults to i_key
#define i_keyfrom <f>  // convertion func i_keyraw => i_key
#define i_keyto <f>    // convertion func i_key* => i_keyraw

#define i_tag <s>      // alternative typename: cqueue_{i_tag}. i_tag defaults to i_key
#include <stc/cqueue.h>
```
`X` should be replaced by the value of `i_tag` in all of the following documentation.


## Methods

```c
cqueue_X            cqueue_X_init(void);
cqueue_X            cqueue_X_with_capacity(intptr_t size);
cqueue_X            cqueue_X_clone(cqueue_X q);

void                cqueue_X_clear(cqueue_X* self);
void                cqueue_X_copy(cqueue_X* self, const cqueue_X* other);
bool                cqueue_X_reserve(cqueue_X* self, intptr_t cap);
void                cqueue_X_shrink_to_fit(cqueue_X* self);
void                cqueue_X_drop(cqueue_X* self);       // destructor

intptr_t            cqueue_X_size(const cqueue_X* self);
intptr_t            cqueue_X_capacity(const cqueue_X* self);
bool                cqueue_X_empty(const cqueue_X* self);

cqueue_X_value*     cqueue_X_front(const cqueue_X* self);
cqueue_X_value*     cqueue_X_back(const cqueue_X* self);

cqueue_X_value*     cqueue_X_push(cqueue_X* self, i_key value);
cqueue_X_value*     cqueue_X_emplace(cqueue_X* self, i_keyraw raw);
void                cqueue_X_pop(cqueue_X* self);
cqueue_X_value      cqueue_X_pull(cqueue_X* self);                       // move out last element

cqueue_X_iter       cqueue_X_begin(const cqueue_X* self);
cqueue_X_iter       cqueue_X_end(const cqueue_X* self);
void                cqueue_X_next(cqueue_X_iter* it);
cqueue_X_iter       cqueue_X_advance(cqueue_X_iter it, intptr_t n);

bool                cqueue_X_eq(const cqueue_X* c1, const cqueue_X* c2); //  require i_eq/i_cmp/i_less.
i_key               cqueue_X_value_clone(i_key value);
cqueue_X_raw        cqueue_X_value_toraw(const cqueue_X_value* pval);
void                cqueue_X_value_drop(cqueue_X_value* pval);
```

## Types

| Type name           | Type definition      | Used to represent...     |
|:--------------------|:---------------------|:-------------------------|
| `cqueue_X`          | `cdeq_X`             | The cqueue type          |
| `cqueue_X_value`    | `i_key`              | The cqueue element type  |
| `cqueue_X_raw`      | `i_keyraw`           | cqueue raw value type    |
| `cqueue_X_iter`     | `cdeq_X_iter`        | cqueue iterator          |

## Examples
```c
#define i_key int
#define i_tag i
#include <stc/cqueue.h>

#include <stdio.h>

int main(void) {
    cqueue_i Q = cqueue_i_init();

    // push() and pop() a few.
    c_forrange (i, 20)
        cqueue_i_push(&Q, i);

    c_forrange (5)
        cqueue_i_pop(&Q);

    c_foreach (i, cqueue_i, Q)
        printf(" %d", *i.ref);

    cqueue_i_drop(&Q);
}
```
Output:
```
5 6 7 8 9 10 11 12 13 14 15 16 17 18 19
```
