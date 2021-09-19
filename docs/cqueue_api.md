# STC [cqueue](../include/stc/cqueue.h): Queue
![Queue](pics/queue.jpg)

The **cqueue** is container that gives the programmer the functionality of a queue - specifically, a FIFO (first-in, first-out) data structure. The queue pushes the elements on the back of the underlying container and pops them from the front.

See the c++ class [std::queue](https://en.cppreference.com/w/cpp/container/queue) for a functional reference.

## Header file and declaration
```c
#define i_tag       // defaults to i_val name
#define i_val       // value: REQUIRED
#define i_cmp       // three-way compare two i_valraw* : REQUIRED IF i_valraw is a non-integral type
#define i_valraw    // convertion "raw" type - defaults to i_val
#define i_valfrom   // convertion func i_valraw => i_val - defaults to plain copy
#define i_valto     // convertion func i_val* => i_valraw - defaults to plain copy
#define i_valdel    // destroy value func - defaults to empty destruct
#include <stc/cqueue.h>
```
`X` should be replaced by the value of ***i_tag*** in all of the following documentation.


## Methods

```c
cqueue_X                cqueue_X_init(void);
cqueue_X                cqueue_X_clone(cqueue_X q);

void                    cqueue_X_clear(cqueue_X* self);
void                    cqueue_X_del(cqueue_X* self);      // destructor

size_t                  cqueue_X_size(cqueue_X q);
bool                    cqueue_X_empty(cqueue_X q);
cqueue_X_value_t*       cqueue_X_front(const cqueue_X* self);
cqueue_X_value_t*       cqueue_X_back(const cqueue_X* self);

void                    cqueue_X_push(cqueue_X* self, cqueue_X_value_t value);
void                    cqueue_X_emplace(cqueue_X* self, cqueue_X_rawvalue_t raw);
void                    cqueue_X_emplace_items(cqueue_X *self, const cqueue_X_rawvalue_t arr[], size_t n);

void                    cqueue_X_pop(cqueue_X* self);

cqueue_X_iter_t         cqueue_X_begin(const cqueue_X* self);
cqueue_X_iter_t         cqueue_X_end(const cqueue_X* self);
void                    cqueue_X_next(cqueue_X_iter_t* it);

cqueue_X_value_t        cqueue_X_value_clone(cqueue_X_value_t val);
```

## Types

| Type name             | Type definition                        | Used to represent...     |
|:----------------------|:---------------------------------------|:-------------------------|
| `cqueue_X`            | Depends on underlying container type   | The cqueue type          |
| `cqueue_X_value_t`    |                   "                    | The cqueue element type  |
| `cqueue_X_rawvalue_t` |                   "                    | cqueue raw value type    |
| `cqueue_X_iter_t`     |                   "                    | cqueue iterator          |

## Examples
```c
#define i_tag i
#define i_val int
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

    cqueue_i_del(&Q);
}
```
Output:
```
5 6 7 8 9 10 11 12 13 14 15 16 17 18 19
```
