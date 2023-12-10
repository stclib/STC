# STC [stack](../include/stc/stack.h): Stack
![Stack](pics/stack.jpg)

The **stack** is a container that gives the programmer the functionality of a stack - specifically, a LIFO (last-in, first-out) data structure. The stack pushes and pops the element from the back of the container, known as the top of the stack.

See the c++ class [std::stack](https://en.cppreference.com/w/cpp/container/stack) for a functional description.

## Header file and declaration

```c
#define i_TYPE <ct>,<kt> // shorthand to define i_type,i_key
#define i_type <t>       // container type name (default: stack_{i_key})
#define i_key <t>        // element type: REQUIRED. Defines stack_X_value
#define i_keydrop <f>    // destroy value func - defaults to empty destruct
#define i_keyclone <f>   // REQUIRED IF i_keydrop defined

#define i_keyraw <t>     // convertion "raw" type - defaults to i_key
#define i_keyfrom <f>    // convertion func i_keyraw => i_key
#define i_keyto <f>      // convertion func i_key* => i_keyraw

#define i_tag <s>        // alternative typename: stack_{i_tag}. i_tag defaults to i_key
#include "stc/stack.h"
```
`X` should be replaced by the value of `i_tag` in all of the following documentation.

## Methods

```c
stack_X             stack_X_init(void);
stack_X             stack_X_with_capacity(intptr_t cap);
stack_X             stack_X_with_size(intptr_t size, i_key fill);
stack_X             stack_X_clone(stack_X st);

void                stack_X_clear(stack_X* self);
bool                stack_X_reserve(stack_X* self, intptr_t n);
void                stack_X_shrink_to_fit(stack_X* self);
i_key*              stack_X_append_uninit(stack_X* self, intptr_t n);
void                stack_X_copy(stack_X* self, const stack_X* other);
void                stack_X_drop(stack_X* self);                       // destructor

intptr_t            stack_X_size(const stack_X* self);
intptr_t            stack_X_capacity(const stack_X* self);
bool                stack_X_empty(const stack_X* self);

i_key*              stack_X_top(const stack_X* self);
const i_key*        stack_X_at(const stack_X* self, intptr_t idx);
i_key*              stack_X_at_mut(stack_X* self, intptr_t idx);

i_key*              stack_X_push(stack_X* self, i_key value);
i_key*              stack_X_emplace(stack_X* self, i_keyraw raw);

void                stack_X_pop(stack_X* self);                        // destroy last element
stack_X_value       stack_X_pull(stack_X* self);                       // move out last element

stack_X_iter        stack_X_begin(const stack_X* self);
stack_X_iter        stack_X_end(const stack_X* self);
void                stack_X_next(stack_X_iter* it);

bool                stack_X_eq(const stack_X* c1, const stack_X* c2); // require i_eq/i_cmp/i_less.
i_key               stack_X_value_clone(i_key value);
i_keyraw            stack_X_value_toraw(const vec_X_value* pval);
void                stack_X_value_drop(vec_X_value* pval);
```

## Types

| Type name          | Type definition                     | Used to represent...        |
|:-------------------|:------------------------------------|:----------------------------|
| `stack_X`          | `struct { stack_value *data; ... }` | The stack type             |
| `stack_X_value`    | `i_key`                             | The stack element type     |
| `stack_X_raw`      | `i_keyraw`                          | stack raw value type       |
| `stack_X_iter`     | `struct { stack_value *ref; }`      | stack iterator             |

## Example
```c
#define i_TYPE IStack,int
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
