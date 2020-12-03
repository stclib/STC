# Container carray: Dynamic 1/2/3 Dimensional Array

This is 1D, 2D and 3D arrays, which are allocated from heap in one single contiguous block of memory.
*carray3* may have sub-array "views" of *carray2* and *carray1* etc.

## Declaration

```c
#define using_carray(X, Value, valueDestroy=c_default_del)
```
The macro `using_carray()` can be instantiated with 2 or 3 arguments in the global scope.
Default values are given above for args not specified. `X` is a type tag name and
will affect the names of all cset types and methods. E.g. declaring `using_carray(my, int);`, `X` should
be replaced by `my` in all of the following documentation. The `#` character should be replaced by `1`, `2` or `3`.

## Types

| Type name            | Type definition               | Used to represent...      |
|:---------------------|:------------------------------|:--------------------------|
| `carray#X`           | `struct { ... }`              | The carray type           |
| `carray#X_value_t`   | `Value`                       | The value type            |
| `carray#X_iter_t`    | `struct { Value *val; }`      | Iterator type             |

## Constants and macros

| Name                 | Purpose                  |
|:---------------------|:-------------------------|
| `carray1_size(arr)`  |  carray1 size and dim    |
| `carray1_xdim(arr)`  |                          |
| `carray2_size(arr)`  |  carray2 size and dims   |
| `carray2_xdim(arr)`  |                          |
| `carray2_ydim(arr)`  |                          |
| `carray3_size(arr)`  |  carray3 size and dims   |
| `carray3_xdim(arr)`  |                          |
| `carray3_ydim(arr)`  |                          |
| `carray3_zdim(arr)`  |                          |

## Header file

All cset definitions and prototypes may be included in your C source file by including a single header file.

```c
#include "stc/carray.h"
```
## Methods

### Constructors
```c
carray1X            carray1X_init(size_t xdim, Value val);
carray2X            carray2X_init(size_t ydim, size_t xdim, Value val);
carray3X            carray3X_init(size_t zdim, size_t ydim, size_t xdim, Value val);
carray1X            carray1X_from(Value* array, size_t xdim);
carray2X            carray2X_from(Value* array, size_t ydim, size_t xdim);
carray3X            carray3X_from(Value* array, size_t zdim, size_t ydim, size_t xdim);
```
### Data access
```c
Value*              carray1X_at(carray1X *self, size_t x);
Value*              carray2X_at(carray2X *self, size_t y, size_t x);
Value*              carray3X_at(carray3X *self, size_t z, size_t y, size_t x);
carray1X            carray2X_at1(carray2X *self, size_t y);
carray1X            carray3X_at2(carray3X *self, size_t z, size_t y);
carray2X            carray3X_at1(carray3X *self, size_t z);
```
### Iterators
```c
carray#X_iter_t     carray#X_begin(carray#X* self);
carray#X_iter_t     carray#X_end(carray#X* self);
void                carray#X_next(carray#X_iter_t* it);
carray#X_value_t*   carray#X_itval(carray#X_iter_t it);
```

## Example
```c
#include <stdio.h>
#include "stc/carray.h"

using_carray(f, float);

int main()
{
    carray3f a3 = carray3f_init(30, 20, 10, 0.0f);  // define a3[30][20][10], init with 0.0f.
    *carray3f_at(&a3, 5, 4, 3) = 3.14f;             // a3[5][4][3] = 3.14

    carray1f a1 = carray3f_at2(&a3, 5, 4);          // sub-array a3[5][4] (no data copy).
    carray2f a2 = carray3f_at1(&a3, 5);             // sub-array a3[5]

    printf("%f\n", *carray1f_at(&a1, 3));           // a1[3] (3.14f)
    printf("%f\n", *carray2f_at(&a2, 4, 3));        // a2[4][3] (3.14f)
    printf("%f\n", *carray3f_at(&a3, 5, 4, 3));     // a3[5][4][3] (3.14f)
    // ...
    carray1f_del(&a1); // does nothing, since it is a sub-array.
    carray2f_del(&a2); // same.
    carray3f_del(&a3); // free array, and invalidates a1, a2.
}
```
Output:
```
3.140000
3.140000
3.140000
```
