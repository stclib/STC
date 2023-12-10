# STC [queue](../include/stc/queue.h): Queue
![Queue](pics/queue.jpg)

The **queue** is container that gives the programmer the functionality of a queue - specifically, a FIFO (first-in, first-out) data structure. The queue pushes the elements on the back of the underlying container and pops them from the front.

See the c++ class [std::queue](https://en.cppreference.com/w/cpp/container/queue) for a functional reference.

## Header file and declaration
```c
#define i_TYPE <ct>,<kt> // shorthand to define i_type,i_key
#define i_type <t>       // queue container type name (default: queue_{i_key})
#define i_key <t>        // element type: REQUIRED. Defines queue_X_value
#define i_keydrop <f>    // destroy value func - defaults to empty destruct
#define i_keyclone <f>   // REQUIRED IF i_keydrop defined

#define i_keyraw <t>     // convertion "raw" type - defaults to i_key
#define i_keyfrom <f>    // convertion func i_keyraw => i_key
#define i_keyto <f>      // convertion func i_key* => i_keyraw

#define i_tag <s>        // alternative typename: queue_{i_tag}. i_tag defaults to i_key
#include "stc/queue.h"
```
`X` should be replaced by the value of `i_tag` in all of the following documentation.


## Methods

```c
queue_X             queue_X_init(void);
queue_X             queue_X_with_capacity(intptr_t size);
queue_X             queue_X_clone(queue_X q);

void                queue_X_clear(queue_X* self);
void                queue_X_copy(queue_X* self, const queue_X* other);
bool                queue_X_reserve(queue_X* self, intptr_t cap);
void                queue_X_shrink_to_fit(queue_X* self);
void                queue_X_drop(queue_X* self);       // destructor

intptr_t            queue_X_size(const queue_X* self);
intptr_t            queue_X_capacity(const queue_X* self);
bool                queue_X_empty(const queue_X* self);

queue_X_value*      queue_X_front(const queue_X* self);
queue_X_value*      queue_X_back(const queue_X* self);

queue_X_value*      queue_X_push(queue_X* self, i_key value);
queue_X_value*      queue_X_emplace(queue_X* self, i_keyraw raw);
void                queue_X_pop(queue_X* self);
queue_X_value       queue_X_pull(queue_X* self);                       // move out last element

queue_X_iter        queue_X_begin(const queue_X* self);
queue_X_iter        queue_X_end(const queue_X* self);
void                queue_X_next(queue_X_iter* it);
queue_X_iter        queue_X_advance(queue_X_iter it, intptr_t n);

bool                queue_X_eq(const queue_X* c1, const queue_X* c2); //  require i_eq/i_cmp/i_less.
i_key               queue_X_value_clone(i_key value);
queue_X_raw         queue_X_value_toraw(const queue_X_value* pval);
void                queue_X_value_drop(queue_X_value* pval);
```

## Types

| Type name          | Type definition     | Used to represent...    |
|:-------------------|:--------------------|:------------------------|
| `queue_X`          | `deq_X`             | The queue type          |
| `queue_X_value`    | `i_key`             | The queue element type  |
| `queue_X_raw`      | `i_keyraw`          | queue raw value type    |
| `queue_X_iter`     | `deq_X_iter`        | queue iterator          |

## Examples
```c
#define i_TYPE queue,int
#include "stc/queue.h"

#include <stdio.h>

int main(void) {
    queue Q = {0};

    // push() and pop() a few.
    c_forrange (i, 20)
        queue_push(&Q, i);

    c_forrange (5)
        queue_pop(&Q);

    c_foreach (i, queue, Q)
        printf(" %d", *i.ref);

    queue_drop(&Q);
}

```
Output:
```
5 6 7 8 9 10 11 12 13 14 15 16 17 18 19
```
