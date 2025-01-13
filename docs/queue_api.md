# STC [queue](../include/stc/queue.h): Queue
![Queue](pics/queue.jpg)

The **queue** is container that gives the programmer the functionality of a queue - specifically, a FIFO (first-in, first-out) data structure. The queue pushes the elements on the back of the underlying container and pops them from the front.

See the c++ class [std::queue](https://en.cppreference.com/w/cpp/container/queue) for a functional reference.

## Header file and declaration
```c++
#define i_type <ct>,<kt> // shorthand for defining i_type, i_key
#define i_type <t>       // queue container type name (default: queue_{i_key})
// One of the following:
#define i_key <t>        // key type
#define i_keyclass <t>   // key type, and bind <t>_clone() and <t>_drop() function names
#define i_keypro <t>     // key "pro" type, use for cstr, arc, box types

#define i_keydrop <fn>   // destroy value func - defaults to empty destruct
#define i_keyclone <fn>  // REQUIRED IF i_keydrop defined

#define i_keyraw <t>     // convertion "raw" type - defaults to i_key
#define i_rawclass <t>   // convertion "raw class". binds <t>_cmp(),  <t>_eq(),  <t>_hash()
#define i_keyfrom <fn>   // convertion func i_keyraw => i_key
#define i_keytoraw <fn>  // convertion func i_key* => i_keyraw

#include "stc/queue.h"
```
In the following, `X` is the value of `i_key` unless `i_type` is defined.


## Methods

```c++
queue_X         queue_X_init(void);
queue_X         queue_X_with_capacity(isize size);

queue_X         queue_X_clone(queue_X q);
void            queue_X_copy(queue_X* self, queue_X other);
void            queue_X_take(queue_X* self, queue_X unowned);      // take ownership of unowned
queue_X         queue_X_move(queue_X* self);                       // move
void            queue_X_drop(queue_X* self);                       // destructor

void            queue_X_clear(queue_X* self);
bool            queue_X_reserve(queue_X* self, isize cap);
void            queue_X_shrink_to_fit(queue_X* self);

isize           queue_X_size(const queue_X* self);
isize           queue_X_capacity(const queue_X* self);
bool            queue_X_is_empty(const queue_X* self);

i_key*          queue_X_front(const queue_X* self);
i_key*          queue_X_back(const queue_X* self);

i_key*          queue_X_push(queue_X* self, i_key value);
i_key*          queue_X_emplace(queue_X* self, i_keyraw raw);
void            queue_X_pop(queue_X* self);
i_key           queue_X_pull(queue_X* self);                       // move out last element

queue_X_iter    queue_X_begin(const queue_X* self);
queue_X_iter    queue_X_end(const queue_X* self);
void            queue_X_next(queue_X_iter* it);
queue_X_iter    queue_X_advance(queue_X_iter it, isize n);

bool            queue_X_eq(const queue_X* c1, const queue_X* c2);  //  require i_eq/i_cmp/i_less.
i_key           queue_X_value_clone(i_key value);
queue_X_raw     queue_X_value_toraw(const i_key* pval);
void            queue_X_value_drop(i_key* pval);
```

## Types

| Type name          | Type definition     | Used to represent...    |
|:-------------------|:--------------------|:------------------------|
| `queue_X`          | `deque_X`           | The queue type          |
| `queue_X_value`    | `i_key`             | The queue element type  |
| `queue_X_raw`      | `i_keyraw`          | queue raw value type    |
| `queue_X_iter`     | `deque_X_iter`      | queue iterator          |

## Examples
```c++
#define i_type queue, int
#include "stc/queue.h"

#include <stdio.h>

int main(void) {
    queue Q = {0};

    // push() and pop() a few.
    for (c_range(i, 20))
        queue_push(&Q, i);

    for (c_range(5))
        queue_pop(&Q);

    for (c_each(i, queue, Q))
        printf(" %d", *i.ref);

    queue_drop(&Q);
}

```
Output:
```
5 6 7 8 9 10 11 12 13 14 15 16 17 18 19
```
