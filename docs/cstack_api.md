# STC [cstack](../include/stc/cstack.h): Stack
![Stack](pics/stack.jpg)

The **cstack** is a container that gives the programmer the functionality of a stack - specifically, a LIFO (last-in, first-out) data structure. The stack pushes and pops the element from the back of the container, known as the top of the stack.

See the c++ class [std::stack](https://en.cppreference.com/w/cpp/container/stack) for a functional description.

## Header file and declaration

```c
#define i_val       // value: REQUIRED
#define i_cmp       // three-way compare two i_valraw* : REQUIRED IF i_valraw is a non-integral type
#define i_del       // destroy value func - defaults to empty destruct
#define i_valraw    // convertion "raw" type - defaults to i_val
#define i_valfrom   // convertion func i_valraw => i_val - defaults to plain copy
#define i_valto     // convertion func i_val* => i_valraw - defaults to plain copy
#define i_tag       // defaults to i_val name
#include <stc/cstack.h>
```
`X` should be replaced by the value of `i_tag` in all of the following documentation.

## Methods

```c
cstack_X            cstack_X_init(void);
cstack_X            cstack_X_clone(cstack_X st);

void                cstack_X_clear(cstack_X* self);
void                cstack_X_reserve(cstack_X* self, size_t n);
void                cstack_X_shrink_to_fit(cstack_X* self);
void                cstack_X_copy(cstack_X* self, cstack_X other);
void                cstack_X_del(cstack_X* self);       // destructor

size_t              cstack_X_size(cstack_X st);
bool                cstack_X_empty(cstack_X st);
cstack_X_value_t*   cstack_X_top(const cstack_X* self);

cstack_X_value_t*   cstack_X_push(cstack_X* self, i_val value);
cstack_X_value_t*   cstack_X_emplace(cstack_X* self, i_valraw raw);

void                cstack_X_pop(cstack_X* self);

cstack_X_iter_t     cstack_X_begin(const cstack_X* self);
cstack_X_iter_t     cstack_X_end(const cstack_X* self);
void                cstack_X_next(cstack_X_iter_t* it);

i_valraw            cstack_X_value_toraw(cvec_X_value_t* pval);
i_val               cstack_X_value_clone(i_val value);
```

## Types

| Type name             | Type definition                        | Used to represent...        |
|:----------------------|:---------------------------------------|:----------------------------|
| `cstack_X`            | `struct { cstack_value_t *data; ... }` | The cstack type             |
| `cstack_X_value_t`    | `i_val`                                | The cstack element type     |
| `cstack_X_rawvalue_t` | `i_valraw`                             | cstack raw value type       |
| `cstack_X_iter_t`     | `struct { cstack_value_t *ref; }`      | cstack iterator             |

## Example
```c
#define i_val int
#define i_tag i
#include <stc/cstack.h>

#include <stdio.h>

int main() {
    cstack_i S = cstack_i_init();

    for (int i=0; i < 100; ++i)
        cstack_i_push(&S, i*i);

    for (int i=0; i < 90; ++i)
        cstack_i_pop(&S);

    printf("top: %d\n", *cstack_i_top(&S));

    cstack_i_del(&S);
}
```
Output:
```
top: 81
```