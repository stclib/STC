# STC Container [cstack](../stc/cstack.h): Stack
![Stack](pics/stack.jpg)

This describes the API of the stack type **cstack**.
See [std::stack](https://en.cppreference.com/w/cpp/container/stack) for a similar c++ class.

## Declaration

```c
#define using_cstack(X, ctype)
```
The macro `using_cstack()` must be instantiated in the global scope. **cstack** uses normally
a **cvec** type as underlying implementation, given as `ctype`. `X` is a type tag name and will
affect the names of all cstack types and methods. E.g. declaring `using_cstack(my, cvec_my);`,
`X` should be replaced by `my` in all of the following documentation.

## Types

| Type name             | Type definition                        | Used to represent...        |
|:----------------------|:---------------------------------------|:----------------------------|
| `cstack_X`            | Depends on underlying container type   | The cstack type             |
| `cstack_X_value_t`    |                   "                    | The cstack element type     |
| `cstack_X_input_t`    |                   "                    | cstack input type           |
| `cstack_X_rawvalue_t` |                   "                    | cstack raw value type       |
| `cstack_X_iter_t`     |                   "                    | cstack iterator             |

## Header file

All cstack definitions and prototypes may be included in your C source file by including a single header file.

```c
#include "stc/cstack.h" /* includes default underlying implementation header cvec.h */
```

## Methods

```c
cstack_X                cstack_X_init(void);
cstack_X                cstack_X_clone(cstack_X st);
void                    cstack_X_del(cstack_X* self);

size_t                  cstack_X_size(cstack_X st);
bool                    cstack_X_empty(cstack_X st);
cstack_X_value_t*       cstack_X_top(cstack_X* self);

void                    cstack_X_push_n(cstack_X *self, const cstack_X_input_t arr[], size_t size);
void                    cstack_X_emplace(cstack_X* self, cstack_X_rawvalue_t raw);
void                    cstack_X_push(cstack_X* self, cstack_X_value_t value);
void                    cstack_X_pop(cstack_X* self);

cstack_X_iter_t         cstack_X_begin(cstack_X* self);
cstack_X_iter_t         cstack_X_end(cstack_X* self);
void                    cstack_X_next(cstack_X_iter_t* it);
cstack_X_value_t*       cstack_X_itval(cstack_X_iter_t it);
```

## Example
```c
#include <stdio.h>
#include "stc/cstack.h"

using_cvec(i, int);
using_cstack(i, cvec_i);

int main() {
    cstack_i stack = cstack_i_init();

    for (int i=0; i < 100; ++i)
        cstack_i_push(&stack, i*i);

    for (int i=0; i < 90; ++i)
        cstack_i_pop(&stack);

    printf("top: %d\n", *cstack_i_top(&stack));
}
```
Output:
```
top: 81
```