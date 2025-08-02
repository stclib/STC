# STC [cspan](../include/stc/cspan.h): Multi-dimensional Array View
![Array](pics/array.jpg)

The **cspan** types are templated non-owning *single* and *multi-dimensional* views of an array.
It supports both row-major and column-major layout efficiently, in a addition to slicing
capabilities similar to [python's numpy arrays](https://numpy.org/doc/stable/user/basics.indexing.html).
Note that **cspan** stores indices as int32_t. Multi-dimensional spans can have up to INT32_MAX elements
in the RANK-1 inner dimensions in total, and INT32_MAX in the outer dimension. Currently limited to 8
dimensions due to ergonomics and optimization of one-dimensional spans. More dimensions can be added with
the *use_cspan_tuple(N)* macro.

See also C++
[std::span](https://en.cppreference.com/w/cpp/container/span) /
[std::mdspan](https://en.cppreference.com/w/cpp/container/mdspan) for similar functionality.

## Header file and declaration
**cspan** types are defined by the *use_cspan()* macro after the header is included.
This is different from other containers where template parameters are defined prior to
including each container. This works well mainly because cspan is a non-owning type.
```c++
#include <stc/cspan.h>
use_cspan(SpanType, ValueType);                      // Define a 1-d span with ValueType elements.
use_cspan(SpanTypeN, ValueType, RANK);               // Define multi-dimensional span with RANK.
                                                       // RANK is the number (constant) of dimensions
                                                       // Has no equality test support.
use_cspan_with_eq(SpanType, ValueType, eq);          // Define a 1-d span with equality function support
use_cspan_with_eq(SpanTypeN, ValueType, eq, RANK);   // Define span with equality function support

// Shorthands:
use_cspan2(S, ValueType);                            // Define span types S, S2 with ranks 1, 2.
use_cspan3(S, ValueType);                            // Define span types S, S2, S3 with ranks 1, 2, 3.

use_cspan2_with_eq(S, ValueType, eq);                // As above, but with equality function support
use_cspan3_with_eq(S, ValueType, eq);                // Use c_default_eq for primary type elements.
```
## Methods

All index arguments are side-effect safe, e.g. `*cspan_at(&ms3, i++, j++, k++)` is safe, however `*cspan_at(&spans[n++], i, j)`
is an error, i.e. the *span* argument itself is not side-effect safe. If the number of arguments does not match the span rank,
a compile error is issued. Runtime bounds checks are enabled by default (define `STC_NDEBUG` or `NDEBUG` to disable).
```c++
                // Create local cspan using stack memory
SpanType        c_make(<SpanType>, {v1, v2, ...});                  // create a local 1-d cspan from values (stack memory)
SpanType        cspan_make(<SpanType>, {v1, v2, ...});              // create a local 1-d cspan (on stack or global memory)
SpanType        cspan_zeros(<SpanType>, N);                         // create a local 1-d cspan. N must be a comp-time constant
SpanType        cspan_by_copy(const ValueType* ptr, N);             // create a local 1-d cspan. N must be a comp-time constant

                // Make cspan view into existing data
SpanType        cspan_from_n(ValueType* ptr, int32_t n);            // make a 1-d cspan view into a pointer + length
SpanType        cspan_from_array(ValueType array[]);                // make a 1-d cspan view into a C array
SpanType        cspan_from_vec(VecType* cnt);                       // make a 1-d cspan view into a vec or stack

                // ISpan2 m = {data, cspan_shape(3, 4), cspan_strides(4, 1)}; // => ISpan2 m = cspan_md(data, 3, 4);
int32_t[N]      cspan_shape(xd, ...)                                // specify dimensions for SpanTypeN constructor
cspan_tupleN    cspan_strides(xs, ...)                              // specify strides for SpanTypeN constructor

int             cspan_rank(const SpanTypeN* self);                  // num dimensions; compile-time constant
isize           cspan_size(const SpanTypeN* self);                  // return number of elements
isize           cspan_index(const SpanTypeN* self, int32_t i, j..); // offset index at i, j,...; range checked

ValueType*      cspan_at(const SpanTypeN* self, int32_t i, j..);    // get element; index range checked
ValueType*      cspan_front(const SpanTypeN* self);                 // first element pointer
ValueType*      cspan_back(const SpanTypeN* self);                  // last element pointer

                // Construct a multi-dim span
SpanTypeN       cspan_md(ValueType* data, int32_t dim1, dim2...);   // c_ROWMAJOR layout
SpanTypeN       cspan_md_layout(cspan_layout layout, ValueType* data, int32_t dim1, dim2...);

                // Transpose an md span in-place. Inverses layout and axes only.
void            cspan_transpose(SpanTypeN* self);
void            cspan_swap_axes(SpanTypeN* self, int ax1, int ax2);

                // Set all span elements to value.
void            cspan_set_all(<SpanTypeN>, self, value);

cspan_layout    cspan_get_layout(const SpanTypeN* self);
bool            cspan_is_rowmajor(const SpanTypeN* self);
bool            cspan_is_colmajor(const SpanTypeN* self);

                // Construct a 1d subspan. Faster, but equal to:
                // Span msub = cspan_slice(&ms, Span, {offset, offset+count});
SpanType1       cspan_subspan(const SpanType1* self, isize offset, int32_t count);

                // Construct a 1d subspan from a 2d span.
OutSpan1        cspan_submd2(const SpanType2* self, int32_t i);

                // Construct a lower rank submd. E.g. Span2 md2 = cspan_submd3(&md3, i);
                // is equal to: Span2 md2 = cspan_slice(&md3, Span2, {i}, {c_ALL}, {c_ALL});
OutSpan2        cspan_submd3(const SpanType3* self, int32_t i);
OutSpan1        cspan_submd3(const SpanType3* self, int32_t i, int32_t j);

                // Construct a 3d, 2d or 1d subspan from a 4d span.
OutSpan3        cspan_submd4(const SpanType4* self, int32_t i);
OutSpan2        cspan_submd4(const SpanType4* self, int32_t i, int32_t j);
OutSpan1        cspan_submd4(const SpanType4* self, int32_t i, int32_t j, int32_t k);

                // Multi-dim span slicing function.
                //       {i}: select i'th column. reduces output rank by one.
                //     {i,j}: from i to j-1.
                //{i,j,step}: step size (default step=1)
                // {i,c_END}: from i to last.
                //   {c_ALL}: full extent, like {0,c_END}.
OutSpanM        cspan_slice(const SpanTypeN* self, <OutSpanM>, {x0,x1,xs}, {y0,y1,ys}.., {N0,N1,Ns});

                // Print numpy style output.
                //  fmt      : printf format specifier.
                //  fp       : optional output file pointer, default stdout.
                //  brackets : optional brackets and comma. Example "{},". Default "[]".
                //  field    : optional args macro function, must match fmt args.
                //             e.g.: #define complexfield(x) creal(x), cimag(x)
                // Examples: cspan_print(Span2, "%.3f", Span2_transposed(sp2)));
                //           cspan_print(Span2, "%.3f", (Span2)cspan_submd3(&sp3, 1));
void            cspan_print(<SpanTypeN>, const char* fmt, SpanTypeN span, FILE* fp = stdout,
                            const char* brackets = "[]", field(x) = x);
                // Print matrix with complex numbers. num_decimals applies both to real and imag parts.
void            cspan_print_complex(<SpanTypeN>, int num_decimals, SpanTypeN span, FILE* fp);

// Member functions

SpanTypeN       SpanTypeN_transposed(SpanTypeN sp); // see also in-place cspan_transpose(&sp);
bool            SpanTypeN_equals(SpanTypeN spx, SpanTypeN spy);
bool            SpanTypeN_eq(const SpanTypeN* self, const SpanTypeN* other);

SpanTypeN_iter  SpanTypeN_begin(const SpanTypeN* self);
SpanTypeN_iter  SpanTypeN_end(const SpanTypeN* self);
void            SpanTypeN_next(SpanTypeN_iter* it);
```

## Types
| Type name         | Type definition / usage                             | Used to represent... |
|:------------------|:----------------------------------------------------|:---------------------|
| `cspan_istride`   | `int32_t`                                           | Stride / Index type  |
| SpanTypeN_value   | `ValueType`                                         | Element value type   |
| SpanTypeN         | `struct { ValueType *data; cspan_istride shape[N]; .. }`| SpanType with rank N |
| cspan_tupleN      | `struct { cspan_istride d[N]; }`                    | Strides for each rank |
| `cspan_layout`    | `enum { c_ROWMAJOR, c_COLMAJOR, c_STRIDED }`        | Multi-dim layout     |
| `c_ALL`           | `cspan_slice(&md, Mat, {1,3}, {c_ALL})`             | Full extent          |
| `c_END`           | `cspan_slice(&md, Mat, {1,c_END}, {2,c_END})`       | End of extent        |

## Example 1

[ [Run this code](https://godbolt.org/z/x1PYoarxE) ]
```c++
#include <stdio.h>
#define i_key int
#include <stc/vec.h>

#define i_key int
#include <stc/stack.h>
#include <stc/cspan.h>
use_cspan(intspan, int);

void printMe(intspan container) {
    printf("%d:", (int)cspan_size(&container));
    for (c_each(e, intspan, container))
        printf(" %d", *e.ref);
    puts("");
}

int main(void)
{
    printMe( c_make(intspan, {1, 2, 3, 4}) );

    int arr[] = {1, 2, 3, 4, 5};
    printMe( (intspan)cspan_from_array(arr) );

    vec_int vec = c_make(vec_int, {1, 2, 3, 4, 5, 6});
    printMe( (intspan)cspan_from_vec(&vec) );

    stack_int stk = c_make(stack_int, {1, 2, 3, 4, 5, 6, 7});
    printMe( (intspan)cspan_from_vec(&stk) );

    intspan spn = c_make(intspan, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12});
    printMe( (intspan)cspan_subspan(&spn, 2, 8) );

    // cleanup
    vec_int_drop(&vec);
    stack_int_drop(&stk);
}
```

## Example 2

Multi-dimension slicing (python):
```py
import numpy as np

if __name__ == '__main__':
    ms3 = np.array((1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24), int)

    ms3 = np.reshape(ms3, (2, 3, 4), order='C')
    ss3 = ms3[:, 0:3, 2:]
    a = ss3[1]
    b = np.transpose(a)

    print("ss3:")
    print(ss3)
    print("\nms3:")
    print(ms3[1])
    print("\na:")
    print(a)
    print("b:")
    print(b)

    print("\na flat:\n", [int(i) for i in a.flat])
    print("b flat:\n", [int(i) for i in b.flat])
'''
 ss3:
[[[ 3  4]
  [ 7  8]
  [11 12]]

 [[15 16]
  [19 20]
  [23 24]]]

ms3:
[[13 14 15 16]
 [17 18 19 20]
 [21 22 23 24]]

a:
[[15 16]
 [19 20]
 [23 24]]
b:
[[15 19 23]
 [16 20 24]]

a flat:
 [15, 16, 19, 20, 23, 24]
b flat:
 [15, 19, 23, 16, 20, 24]
'''
```
Multi-dimension slicing (STC cspan):

[ [Run this code](https://godbolt.org/z/cEPbsja98) ]
```c++
#include <stdio.h>
#include <stc/cspan.h>
use_cspan3(myspan, int); // define myspan, myspan2, myspan3.

int main(void) {
    int arr[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24};

    myspan3 ms3 = cspan_md(arr, 2, 3, 4); // row-major layout
    myspan3 ss3 = cspan_slice(&ms3, myspan3, {c_ALL}, {0,3}, {2,c_END});
    puts("ss3:");
    myspan2 a = cspan_submd3(&ss3, 1);
    myspan2 b = myspan2_transposed(a);

    cspan_print(myspan3, "%d", ss3);
    puts("\nms3[1]:");
    cspan_print(myspan2, "%d", ((myspan2)cspan_submd3(&ms3, 1)));

    puts("\na:");
    cspan_print(myspan2, "%d", a);

    puts("b:");
    cspan_print(myspan2, "%d", b);

    puts("\na flat:");
    for (c_each(i, myspan2, a))
        printf(" %d,", *i.ref);

    puts("\nb flat:");
    for (c_each(i, myspan2, b))
        printf(" %d,", *i.ref);
    puts("");
}
```

## Example 3
Slicing cspan without and with reducing the rank:

[ [Run this code](https://godbolt.org/z/PTh8ojenc) ]
```c++
#include <stdio.h>
#include <stc/cspan.h>

use_cspan3(Span, int); // Shorthand to define Span, Span2, and Span3

int main(void)
{
    Span span = c_make(Span, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
                              14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24});
    Span3 span3 = cspan_md(span.data, 2, 4, 3);

    // numpy style printout
    puts("span3:");
    cspan_print(Span3, "%d", span3);

    puts("\nspan3[:, 3:4, :]:");
    Span3 ss3 = cspan_slice(&span3, Span3, {c_ALL}, {3,4}, {c_ALL});
    cspan_print(Span3, "%d", ss3);

    puts("\nspan3[:, 3, :]:");
    Span2 ss2 = cspan_slice(&span3, Span2, {c_ALL}, {3}, {c_ALL});
    cspan_print(Span2, "%d", ss2);

    puts("\nspan3 swap axes to: [1, 2, 0]");
    Span3 swapped = span3;
    cspan_swap_axes(&swapped, 0, 1);
    cspan_swap_axes(&swapped, 1, 2);
    cspan_print(Span3, "%d", swapped);
}
```
