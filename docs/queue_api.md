# STC [queue](../include/stc/queue.h): Queue
![Queue](pics/queue.jpg)

The **queue** is container that gives the programmer the functionality of a queue - specifically, a FIFO (first-in, first-out) data structure. The queue pushes the elements on the back of the underlying container and pops them from the front.

See the c++ class [std::queue](https://en.cppreference.com/w/cpp/container/queue) for a functional reference.

## Header file and declaration
```c++
#define T <ct>, <kt>[, (<opt>)] // shorthand for defining queue name, i_key, and i_opt
// Common <opt> traits:
//   c_comp_key  - Key type <kt> is a comparable;
//                Binds <kt>_less() (and <kt>_cmp()) "member" function names.
//   c_class_key - Binds <kt>_clone() and <kt>_drop() function names.
//                All containers used as keys themselves can be specified with the c_class_key trait.
//   c_pro_key   - "Pro" key type, use e.g. for built-in `cstr`, `zsview`, `arc`, and `box` as i_key.
//                These support conversion to/from a "raw" input type (such as const char*) when
//                using <ct>_emplace*() functions.


// Alternative to defining T:
#define i_key <t>             // define key type. container type name <ct> defaults to queue_<kt>.

// Override/define when not the <opt> traits are specified:
#define i_cmp <fn>            // three-way compare two i_keyraw* : REQUIRED IF i_keyraw is a non-integral type
#define i_less <fn>           // optional/alternative less-comparison. Is transformed to an i_cmp
#define i_eq <fn>             // optional equality comparison. Implicitly defined with i_cmp, but not i_less.
#define i_keydrop <fn>        // destroy key func - defaults to empty destruct
#define i_keyclone <fn>       // Required if i_keydrop is defined

#include <stc/queue.h>
```
In the following, `X` is the value of `i_key` unless `T` is defined.


## Methods

```c++
queue_X         queue_X_init(void);
queue_X         queue_X_with_capacity(isize cap);
queue_X         queue_X_with_size(isize size, i_keyraw rawval);
queue_X         queue_X_with_size_uninit(isize size);

queue_X         queue_X_clone(queue_X q);
void            queue_X_copy(queue_X* self, const queue_X* other);
void            queue_X_take(queue_X* self, queue_X unowned);      // take ownership of unowned
queue_X         queue_X_move(queue_X* self);                       // move
void            queue_X_drop(const queue_X* self);                 // destructor

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
i_key           queue_X_value_clone(const queue_X* self, i_key value);
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
#define T queue, int
#include <stc/queue.h>

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
