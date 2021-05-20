# STC [cqueue](../include/stc/cqueue.h): Queue
![Queue](pics/queue.jpg)

The **cqueue** is container adapter that gives the programmer the functionality of a queue - specifically, a FIFO (first-in, first-out) data structure. The class template acts as a wrapper to the underlying container - only a specific set of functions is provided. The queue pushes the elements on the back of the underlying container and pops them from the front.

See the c++ class [std::queue](https://en.cppreference.com/w/cpp/container/queue) for a functional reference.

## Header file and declaration
```c
#include <stc/cqueue.h> /* includes default underlying implementation header cdeq.h */

using_cqueue(X, ctype)
```
The macro `using_cqueue()` must be instantiated in the global scope. **cqueue** uses normally
a **cdeq_X** or **clist_X** type as underlying implementation, given as `ctype`. See example below for usage.
`X` is a type tag name and will affect the names of all cqueue types and methods. E.g. declaring
`using_cqueue(i, clist_i)`, `X` should be replaced by `i` in all of the following documentation.

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
#include <stc/cqueue.h>
#include <stdio.h>

using_cdeq(i, int);
using_cqueue(i, cdeq_i);

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
### Example 2
Use clist as underlying cqueue implementation.
```c
#include <stc/cqueue.h>
#include <stc/clist.h>
#include <stdio.h>

using_clist(i, int);
using_cqueue(i, clist_i);

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
