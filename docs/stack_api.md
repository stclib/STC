# STC [stack](../include/stc/stack.h): Stack
![Stack](pics/stack.jpg)

The **stack** is a container that gives the programmer the functionality of a stack - specifically, a LIFO (last-in, first-out) data structure. The stack pushes and pops the element from the back of the container, known as the top of the stack.

See the c++ class [std::stack](https://en.cppreference.com/w/cpp/container/stack) for a functional description.

## Header file and declaration

```c++
#define i_type <ct>,<kt> // shorthand for defining i_type, i_key
#define i_type <t>       // container type name (default: stack_{i_key})
// One of the following:
#define i_key <t>        // key type
#define i_keyclass <t>   // key type, and bind <t>_clone() and <t>_drop() function names
#define i_keypro <t>     // key "pro" type, use for cstr, arc, box types

#define i_keydrop <fn>   // destroy value func - defaults to empty destruct
#define i_keyclone <fn>  // REQUIRED IF i_keydrop defined

#define i_use_cmp        // enable sorting, binary_search and lower_bound
#define i_cmp <fn>       // three-way compare two i_keyraw's
#define i_less <fn>      // less comparison. Alternative to i_cmp
#define i_eq <fn>        // equality comparison. Implicitly defined with i_cmp, but not i_less.

#define i_keyraw <t>     // convertion "raw" type - defaults to i_key
#define i_rawclass <t>   // convertion "raw class". binds <t>_cmp(),  <t>_eq(),  <t>_hash()
#define i_keyfrom <fn>   // convertion func i_keyraw => i_key
#define i_keytoraw <fn>  // convertion func i_key* => i_keyraw


#include "stc/stack.h"
```
- Defining either `i_use_cmp`, `i_less` or `i_cmp` will enable sorting, binary_search and lower_bound
- **emplace**-functions are only available when `i_keyraw` is implicitly or explicitly defined.
- In the following, `X` is the value of `i_key` unless `i_type` is defined.

## Methods

```c++
stack_X         stack_X_init(void);
stack_X         stack_X_with_capacity(isize cap);
stack_X         stack_X_with_size(isize size, i_key fill);

stack_X         stack_X_clone(stack_X st);
void            stack_X_copy(stack_X* self, stack_X other);
stack_X         stack_X_move(stack_X* self);                                    // move
void            stack_X_take(stack_X* self, stack_X unowned);                   // take ownership of unowned
void            stack_X_drop(stack_X* self);                                    // destructor

void            stack_X_clear(stack_X* self);
bool            stack_X_reserve(stack_X* self, isize n);
void            stack_X_shrink_to_fit(stack_X* self);

isize           stack_X_size(const stack_X* self);
isize           stack_X_capacity(const stack_X* self);
bool            stack_X_is_empty(const stack_X* self);

const i_key*    stack_X_at(const stack_X* self, isize idx);
const i_key*    stack_X_top(const stack_X* self);
const i_key*    stack_X_front(const stack_X* self);
const i_key*    stack_X_back(const stack_X* self);

i_key*          stack_X_at_mut(stack_X* self, isize idx);
i_key*          stack_X_top_mut(stack_X* self);
i_key*          stack_X_front_mut(stack_X* self);
i_key*          stack_X_back_mut(stack_X* self);

i_key*          stack_X_push(stack_X* self, i_key value);
i_key*          stack_X_emplace(stack_X* self, i_keyraw raw);
i_key*          stack_X_append_uninit(stack_X* self, isize n);

void            stack_X_pop(stack_X* self);                                     // destroy last element
i_key           stack_X_pull(stack_X* self);                                    // move out last element

// Requires either i_use_cmp, i_cmp or i_less defined:
void            stack_X_sort(stack_X* self);                                    // quicksort from sort.h
isize           stack_X_lower_bound(const stack_X* self, const i_keyraw raw);   // return c_NPOS if not found
isize           stack_X_binary_search(const stack_X* self, const i_keyraw raw); // return c_NPOS if not found

stack_X_iter    stack_X_begin(const stack_X* self);
stack_X_iter    stack_X_end(const stack_X* self);
void            stack_X_next(stack_X_iter* it);

bool            stack_X_eq(const stack_X* c1, const stack_X* c2); // require i_eq/i_cmp/i_less.
i_key           stack_X_value_clone(i_key value);
i_keyraw        stack_X_value_toraw(const vec_X_value* pval);
void            stack_X_value_drop(vec_X_value* pval);
```

## Types

| Type name          | Type definition                     | Used to represent...        |
|:-------------------|:------------------------------------|:----------------------------|
| `stack_X`          | `struct { stack_value *data; ... }` | The stack type             |
| `stack_X_value`    | `i_key`                             | The stack element type     |
| `stack_X_raw`      | `i_keyraw`                          | stack raw value type       |
| `stack_X_iter`     | `struct { stack_value *ref; }`      | stack iterator             |

## Example
```c++
#define i_type IStack, int
#include "stc/stack.h"

#include <stdio.h>

int main(void) {
    IStack stk = {0};

    for (int i=0; i < 100; ++i)
        IStack_push(&stk, i*i);

    for (int i=0; i < 90; ++i)
        IStack_pop(&stk);

    printf("top: %d\n", *IStack_top(&stk));

    IStack_drop(&stk);
}
```
Output:
```
top: 81
```
