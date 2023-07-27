# STC [cstack](../include/stc/cstack.h): Stack
![Stack](pics/stack.jpg)

The **cstack** is a container that gives the programmer the functionality of a stack - specifically, a LIFO (last-in, first-out) data structure. The stack pushes and pops the element from the back of the container, known as the top of the stack.

See the c++ class [std::stack](https://en.cppreference.com/w/cpp/container/stack) for a functional description.

## Header file and declaration

```c
#define i_key <t>      // element type: REQUIRED. Note: i_val* may be specified instead of i_key*.
#define i_type <t>     // container type name
#define i_keydrop <f>  // destroy value func - defaults to empty destruct
#define i_keyclone <f> // REQUIRED IF i_keydrop defined

#define i_keyraw <t>   // convertion "raw" type - defaults to i_key
#define i_keyfrom <f>  // convertion func i_keyraw => i_key
#define i_keyto <f>    // convertion func i_key* => i_keyraw

#define i_tag <s>      // alternative typename: cstack_{i_tag}. i_tag defaults to i_key
#include <stc/cstack.h>
```
`X` should be replaced by the value of `i_tag` in all of the following documentation.

## Methods

```c
cstack_X            cstack_X_init(void);
cstack_X            cstack_X_with_capacity(intptr_t cap);
cstack_X            cstack_X_with_size(intptr_t size, i_key fill);
cstack_X            cstack_X_clone(cstack_X st);

void                cstack_X_clear(cstack_X* self);
bool                cstack_X_reserve(cstack_X* self, intptr_t n);
void                cstack_X_shrink_to_fit(cstack_X* self);
i_key*              cstack_X_append_uninit(cstack_X* self, intptr_t n);
void                cstack_X_copy(cstack_X* self, const cstack_X* other);
void                cstack_X_drop(cstack_X* self);                       // destructor

intptr_t            cstack_X_size(const cstack_X* self);
intptr_t            cstack_X_capacity(const cstack_X* self);
bool                cstack_X_empty(const cstack_X* self);

i_key*              cstack_X_top(const cstack_X* self);
const i_key*        cstack_X_at(const cstack_X* self, intptr_t idx);
i_key*              cstack_X_at_mut(cstack_X* self, intptr_t idx);

i_key*              cstack_X_push(cstack_X* self, i_key value);
i_key*              cstack_X_emplace(cstack_X* self, i_keyraw raw);

void                cstack_X_pop(cstack_X* self);                        // destroy last element
cstack_X_value      cstack_X_pull(cstack_X* self);                       // move out last element

cstack_X_iter       cstack_X_begin(const cstack_X* self);
cstack_X_iter       cstack_X_end(const cstack_X* self);
void                cstack_X_next(cstack_X_iter* it);

bool                cstack_X_eq(const cstack_X* c1, const cstack_X* c2); // require i_eq/i_cmp/i_less.
i_key               cstack_X_value_clone(i_key value);
i_keyraw            cstack_X_value_toraw(const cvec_X_value* pval);
void                cstack_X_value_drop(cvec_X_value* pval);
```

## Types

| Type name           | Type definition                      | Used to represent...        |
|:--------------------|:-------------------------------------|:----------------------------|
| `cstack_X`          | `struct { cstack_value *data; ... }` | The cstack type             |
| `cstack_X_value`    | `i_key`                              | The cstack element type     |
| `cstack_X_raw`      | `i_keyraw`                           | cstack raw value type       |
| `cstack_X_iter`     | `struct { cstack_value *ref; }`      | cstack iterator             |

## Example
```c
#define i_type IStack
#define i_key int
#include <stc/cstack.h>

#include <stdio.h>

int main(void) {
    IStack stk = IStack_init();

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
