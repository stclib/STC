# STC Container [carray](../stc/carray.h): Dynamic 1/2/3 Dimensional Array
![Array](pics/array.jpg)

This is 1D, 2D and 3D arrays, which are allocated from heap in one single contiguous block of memory.
*carray3* may have sub-array "views" of *carray2* and *carray1* etc.
See [boost::multi_array](https://www.boost.org/doc/libs/release/libs/multi_array) for similar c++ classes.

## Declaration

```c
#define using_carray(X, Value, valueDestroy=c_default_del, 
                               valueClone=c_default_clone)
```
The macro `using_carray()` can be instantiated with 2 or 4 arguments in the global scope.
Default values are given above for args not specified. `X` and `N` are type tags and
will affect the names of all cset types and methods. E.g. declaring `using_carray(my, int);`, `X` should
be replaced by `my` in all of the following documentation. The `N` character should be replaced by `1`, `2` or `3`.

## Header file

All carray definitions and prototypes may be included in your C source file by including a single header file.

```c
#include "stc/carray.h"
```
## Methods

```c
carray1X            carray1X_init(size_t xdim, Value val);
carray2X            carray2X_init(size_t ydim, size_t xdim, Value val);
carray3X            carray3X_init(size_t zdim, size_t ydim, size_t xdim, Value val);

carray1X            carray1X_from(Value* array, size_t xdim);
carray2X            carray2X_from(Value* array, size_t ydim, size_t xdim);
carray3X            carray3X_from(Value* array, size_t zdim, size_t ydim, size_t xdim);

carrayNX            carrayNX_clone(carrayNX arr);
void                carrayNX_del(carrayNX* self);

size_t              carrayNX_size(carrayNX arr);
size_t              carrayNX_xdim(carrayNX arr);
size_t              carrayNX_ydim(carrayNX arr); // not N=3
size_t              carrayNX_zdim(carrayNX arr); // only N=3

Value*              carray1X_at(carray1X *self, size_t x);
Value*              carray2X_at(carray2X *self, size_t y, size_t x);
Value*              carray3X_at(carray3X *self, size_t z, size_t y, size_t x);

carray1X            carray2X_at1(carray2X *self, size_t y);
carray2X            carray3X_at1(carray3X *self, size_t z);
carray1X            carray3X_at2(carray3X *self, size_t z, size_t y);

carrayNX_iter_t     carrayNX_begin(carrayNX* self);
carrayNX_iter_t     carrayNX_end(carrayNX* self);
void                carrayNX_next(carrayNX_iter_t* it);
carrayNX_value_t*   carrayNX_itval(carrayNX_iter_t it);
```
## Types

| Type name            | Type definition               | Used to represent...      |
|:---------------------|:------------------------------|:--------------------------|
| `carrayNX`           | `struct { ... }`              | The carray type           |
| `carrayNX_value_t`   | `Value`                       | The value type            |
| `carrayNX_iter_t`    | `struct { Value *ref; }`      | Iterator type             |

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
