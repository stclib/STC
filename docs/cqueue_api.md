# STC [cqueue](../include/stc/cqueue.h): Queue
![Queue](pics/queue.jpg)

The **cqueue** is container that gives the programmer the functionality of a queue - specifically, a FIFO (first-in, first-out) data structure. The queue pushes the elements on the back of the underlying container and pops them from the front.

See the c++ class [std::queue](https://en.cppreference.com/w/cpp/container/queue) for a functional reference.

## Header file and declaration
```c
#define i_val       // value: REQUIRED
#define i_cmp       // three-way compare two i_valraw* : REQUIRED IF i_valraw is a non-integral type
#define i_valdrop   // destroy value func - defaults to empty destruct
#define i_valraw    // convertion "raw" type - defaults to i_val
#define i_valfrom   // convertion func i_valraw => i_val - defaults to plain copy
#define i_valto     // convertion func i_val* => i_valraw - defaults to plain copy
#define i_tag       // defaults to i_val
#include <stc/cqueue.h>
```
`X` should be replaced by the value of `i_tag` in all of the following documentation.


## Methods

```c
cqueue_X            cqueue_X_init(void);
cqueue_X            cqueue_X_clone(cqueue_X q);

void                cqueue_X_clear(cqueue_X* self);
void                cqueue_X_copy(cqueue_X* self, cqueue_X other);
void                cqueue_X_drop(cqueue_X* self);       // destructor

size_t              cqueue_X_size(cqueue_X q);
bool                cqueue_X_empty(cqueue_X q);
cqueue_X_value*     cqueue_X_front(const cqueue_X* self);
cqueue_X_value*     cqueue_X_back(const cqueue_X* self);

cqueue_X_value*     cqueue_X_push(cqueue_X* self, i_val value);
cqueue_X_value*     cqueue_X_emplace(cqueue_X* self, i_valraw raw);

void                cqueue_X_pop(cqueue_X* self);

cqueue_X_iter       cqueue_X_begin(const cqueue_X* self);
cqueue_X_iter       cqueue_X_end(const cqueue_X* self);
void                cqueue_X_next(cqueue_X_iter* it);

i_val               cqueue_X_value_clone(i_val value);
```

## Types

| Type name           | Type definition      | Used to represent...     |
|:--------------------|:---------------------|:-------------------------|
| `cqueue_X`          | `cdeq_X`             | The cqueue type          |
| `cqueue_X_value`    | `i_val`              | The cqueue element type  |
| `cqueue_X_raw`      | `i_valraw`           | cqueue raw value type    |
| `cqueue_X_iter`     | `cdeq_X_iter`        | cqueue iterator          |

## Examples
```c
#define i_val int
#define i_tag i
#include <stc/cqueue.h>

#include <stdio.h>

int main() {
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
