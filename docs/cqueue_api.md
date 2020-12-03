# Container cqueue: Queue

This describes the API of the queue type **cqueue**.

## Declaration

```c
#define using_cqueue(X, ctype)
```
The macro `using_cqueue()` must be instantiated in the global scope.
**cqueue** uses normally a **clist** type as underlying implementation, given as `ctype`.
Default values are given above for args not specified. `X` is a type tag name and
will affect the names of all cqueue types and methods. E.g. declaring `using_cqueue(my, clist_my);`,
`X` should be replaced by `my` in all of the following documentation.

## Types

| Type name             | Type definition                        | Used to represent...     |
|:----------------------|:---------------------------------------|:-------------------------|
| `cqueue_X`            | Depends on underlying container type   | The cqueue type          |
| `cqueue_X_value_t`    |                   "                    | The cqueue element type  |
| `cqueue_X_input_t`    |                   "                    | cqueue input type        |
| `cqueue_X_rawvalue_t` |                   "                    | cqueue raw value type    |
| `cqueue_X_iter_t`     |                   "                    | cqueue iterator          |

## Header file

All cqueue definitions and prototypes may be included in your C source file by including a single header file.

```c
#include "stc/cqueue.h"
```

## Methods

```c
cqueue_X                cqueue_X_init(void);
void                    cqueue_X_del(cqueue_X* self);

size_t                  cqueue_X_size(cqueue_X pq);
bool                    cqueue_X_empty(cqueue_X pq);
cqueue_X_value_t*       cqueue_X_front(cqueue_X* self);
cqueue_X_value_t*       cqueue_X_back(cqueue_X* self);

void                    cqueue_X_push_n(cqueue_X *self, const cqueue_X_input_t in[], size_t size);
void                    cqueue_X_emplace(cqueue_X* self, cqueue_X_rawvalue_t raw);
void                    cqueue_X_push(cqueue_X* self, cqueue_X_value_t value);
void                    cqueue_X_pop(cqueue_X* self);

cqueue_X_iter_t         cqueue_X_begin(cqueue_X* self);
cqueue_X_iter_t         cqueue_X_end(cqueue_X* self);
void                    cqueue_X_next(cqueue_X_iter_t* it);
cqueue_X_value_t*       cqueue_X_itval(cqueue_X_iter_t it);
```

## Examples
```c
#include <stdio.h>
#include "stc/cqueue.h"

using_clist(i, int);
using_cqueue(i, clist_i);

int main() {
    cqueue_i queue = cqueue_i_init();

    // push() and pop() a few.
    c_forrange (i, 20)
        cqueue_i_push(&queue, i);

    c_forrange (5)
        cqueue_i_pop(&queue);

    c_foreach (i, cqueue_i, queue)
        printf(" %d", *i.val);

    cqueue_i_del(&queue);
}
```
Output:
```
5 6 7 8 9 10 11 12 13 14 15 16 17 18 19
```