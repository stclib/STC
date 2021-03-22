# STC [carray](../stc/carray.h): Dynamic Multi-dimensional Array
![Array](pics/array.jpg)

The **carray** containers provides templates for multidimensional arrays. It supports 2- and
3-dimensional arrays, which are allocated from the heap as a single contiguous block of memory.
The carrays can be used almost like regular constant size multi-dimensional arrays in C, and has
the same property of storing data in one block of memory, which can be passed to any method.

See the c++ class [boost::multi_array](https://www.boost.org/doc/libs/release/libs/multi_array) for similar functionality.

## Declaration

```c
using_carray2(X, Value);
using_carray2(X, Value, valueDel, valueClone);
using_carray3(X, Value);
using_carray3(X, Value, valueDel, valueClone);
```
The macro `using_carray2()` must be instantiated in the global scope. `X` and `N` are type tags and
will affect the names of all cset types and methods. E.g. declaring `using_carray3(i, int);`, `X` should
be replaced by `i` in all of the following documentation.

## Header file

All carray definitions and prototypes are available by including a single header file.

```c
#include <stc/carray.h>
```
## Methods

```c
carray2X            carray2X_init(size_t xdim, size_t ydim, Value val);
carray2X            carray2X_from(Value* array, size_t xdim, size_t ydim);
carray2X            carray2X_clone(carray2X arr);
void                carray2X_del(carray2X* self);
void                carray2X_del_internals(carray2X* self); // destroy pointers only

size_t              carray2X_size(carray2X arr);
Value*              carray2X_data(carray2X* self);          // contiguous memory

carray2X_iter_t     carray2X_begin(const carray2X* self);
carray2X_iter_t     carray2X_end(const carray2X* self);
void                carray2X_next(carray2X_iter_t* it);
```
```c
carray3X            carray3X_init(size_t xdim, size_t ydim, size_t zdim, Value val);
carray3X            carray3X_from(Value* array, size_t xdim, size_t ydim, size_t zdim);
carray3X            carray3X_clone(carray3X arr);
void                carray3X_del(carray3X* self);
void                carray3X_del_internals(carray3X* self); // destroy pointers only

size_t              carray3X_size(carray3X arr);
Value*              carray3X_data(carray3X* self);          // contiguous memory

carray3X_iter_t     carray3X_begin(const carray3X* self);
carray3X_iter_t     carray3X_end(const carray3X* self);
void                carray3X_next(carray3X_iter_t* it);
```
## Types

| Type name            | Type definition                                  | Used to represent...      |
|:---------------------|:-------------------------------------------------|:--------------------------|
| `carray2X`           | `struct { Value **at; size_t xdim,ydim; }`       | The carray2 type           |
| `carray2X_value_t`   | `Value`                                          | The value type            |
| `carray2X_iter_t`    | `struct { Value *ref; }`                         | Iterator type             |
| `carray3X`           | `struct { Value ***at; size_t xdim,ydim,zdim; }` | The carray3 type           |
| `carray3X_value_t`   | `Value`                                          | The value type            |
| `carray3X_iter_t`    | `struct { Value *ref; }`                         | Iterator type             |

## Example
```c
#include "stc/carray.h"
#include <stdio.h>

using_carray3(f, float);
using_carray2(i, uint32_t);

int main()
{
    // Ex1
    int xd = 30, yd = 20, zd = 10;
    carray3f a3 = carray3f_init(xd, yd, zd, 0.0f);  // define a3[30][20][10], init with 0.0f.
    a3.at[5][4][3] = 3.14f;

    float *a1 = a3.at[5][4];
    float **a2 = a3.at[5];

    printf("%f\n", a1[3]); // 3.14
    printf("%f\n", a2[4][3]); // 3.14
    printf("%f\n", a3.at[5][4][3]); // 3.14
    carray3f_del(&a3); // free array

    // Ex2
    int w = 256, h = 128;
    carray2i image = carray2i_from(c_new(uint32_t, w*h), w, h); // no value init
    int n = 0;
    c_foreach (i, carray2i, image) {
        uint32_t t = n++ % 256;
        *i.ref = t | t << 8 | t << 16 | 255;
    }

    for (int y=0; y<image.ydim; ++y)
        image.at[y][y] = 0xffffffff;
    carray2i_del(&image);
}
```
Output:
```
3.140000
3.140000
3.140000
```
