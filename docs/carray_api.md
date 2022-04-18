# STC [carr2, carr3](../include/stc/carray.h): Dynamic Multi-dimensional Arrays
![Array](pics/array.jpg)

The **carr2** and **carr3** are templated 2D and 3D dynamic arrays. They are allocated on the heap as a single
contiguous block of memory. The arrays can be indexed like regular constant size multi-dimensional arrays in C.

See the c++ class [boost::multi_array](https://www.boost.org/doc/libs/release/libs/multi_array) for similar functionality.

## Header file and declaration

```c
#define i_val       // value: REQUIRED
#define i_valdrop   // destroy value func - defaults to empty destruct
#define i_valfrom   // func Raw => i_val - defaults to plain copy
#define i_valto     // func i_val => Raw - defaults to plain copy
#define i_tag       // defaults to i_val

#include <stc/carr2.h> // or <stc/carr3.h>
```
`X` should be replaced by the value of `i_tag` in all of the following documentation.

## Methods

carr2_X:
```c
carr2_X             carr2_X_init(size_t xdim, size_t ydim);
carr2_X             carr2_X_with_values(size_t xdim, size_t ydim, i_val val);
carr2_X             carr2_X_with_storage(size_t xdim, size_t ydim, i_val* array);
carr2_X             carr2_X_clone(carr2_X arr);
void                carr2_X_copy(carr2_X* self, carr2_X other);
i_val*              carr2_X_release(carr2_X* self);       // release storage (not freed)
void                carr2_X_drop(carr2_X* self);

size_t              carr2_X_size(carr2_X arr);
i_val*              carr2_X_data(carr2_X* self);          // access storage data
const i_val*        carr2_X_at(const carr2_X* self, size_t x, size_t y);
size_t              carr2_X_idx(const carr2_X* self, size_t x, size_t y);

carr2_X_iter        carr2_X_begin(const carr2_X* self);
carr2_X_iter        carr2_X_end(const carr2_X* self);
void                carr2_X_next(carr2_X_iter* it);
```
carr3:
```c
carr3_X             carr3_X_init(size_t xdim, size_t ydim, size_t zdim);
carr3_X             carr3_X_with_values(size_t xdim, size_t ydim, size_t zdim, i_val val);
carr3_X             carr3_X_with_storage(size_t xdim, size_t ydim, size_t zdim, i_val* array);
carr3_X             carr3_X_clone(carr3_X arr);
void                carr3_X_copy(carr3_X* self, carr3_X other);
i_val*              carr3_X_release(carr3_X* self);                               // release storage (not freed)
void                carr3_X_drop(carr3_X* self);

size_t              carr3_X_size(carr3_X arr);
i_val*              carr3_X_data(carr3_X* self);                                  // storage data
const i_val*        carr3_X_at(const carr3_X* self, size_t x, size_t y, size_t z);
size_t              carr3_X_idx(const carr3_X* self, size_t x, size_t y, size_t z);

carr3_X_iter        carr3_X_begin(const carr3_X* self);
carr3_X_iter        carr3_X_end(const carr3_X* self);
void                carr3_X_next(carr3_X_iter* it);
```
## Types

| Type name         | Type definition                                      | Used to represent... |
|:------------------|:-----------------------------------------------------|:---------------------|
| `carr2_X`         | `struct { i_val **data; size_t xdim, ydim; }`        | The array 2D type    |
| `carr2_X_value`   | `i_val`                                              | The value type       |
| `carr2_X_iter`    | `struct { i_val *ref; }`                             | Iterator type        |
|                   |                                                      |                      |
| `carr3_X`         | `struct { i_val ***data; size_t xdim, ydim, zdim; }` | The array 3D type    |
| `carr3_X_value`   | `i_val`                                              | The value type       |
| `carr3_X_iter`    | `struct { i_val *ref; }`                             | Iterator type        |

The **carr3** elements can be accessed like `carr3_i arr = ...; int val = arr.data[x][y][z];`, or with `carr3_i_at(&arr, x, y, z)`.

## Example
```c
#include <stdio.h>

#define i_val uint32_t
#define i_tag i
#include <stc/carr2.h>

#define i_val float
#define i_tag f
#include <stc/carr3.h>

int main()
{
    // Ex1
    int xd = 30, yd = 20, zd = 10;
    // define arr3[30][20][10], initialized with zeros.
    c_autovar (carr3_f arr3 = carr3_f_with_values(xd, yd, zd, 0.0f), 
                              carr3_f_drop(&arr3)) {
        arr3.data[5][4][3] = 3.14f;

        float *arr1 = arr3.data[5][4];
        float **arr2 = arr3.data[5];

        printf("%f\n", arr1[3]); // 3.14
        printf("%f\n", arr2[4][3]); // 3.14
        printf("%f\n", arr3.data[5][4][3]); // 3.14
    }

    // Ex2
    int w = 256, h = 128;
    c_autovar (carr2_i image = carr2_i_init(w, h), carr2_i_drop(&image)) {
        int n = 0;
        c_foreach (i, carr2_i, image) {
            uint32_t t = n++ % 256;
            *i.ref = t | t << 8 | t << 16 | 255;
        }

        for (int y = 0; y < image.ydim; ++y)
            image.data[y][y] = 0xffffffff;
    }
}
```
Output:
```
3.140000
3.140000
3.140000
```
