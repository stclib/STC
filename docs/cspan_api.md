# STC [cspan](../include/stc/cspan.h): Multi-dimensional Array View
![Array](pics/array.jpg)

The **cspan** types are templated non-owning *single* and *multi-dimensional* views of an array.
It supports both row-major and column-major layout efficiently, in a addition to slicing
capabilities similar to [python's numpy arrays](https://numpy.org/doc/stable/user/basics.indexing.html).
Note that **cspan** stores indices as int32_t. Multi-dimensional spans can have up to INT32_MAX elements
in the RANK-1 inner dimensions in total, and INT32_MAX in the outer dimension. Currently limited to 8
dimensions due to ergonomics and optimization of one-dimensional spans. More dimensions can be added with
the *using_cspan_tuple(N)* macro.

See also C++
[std::span](https://en.cppreference.com/w/cpp/container/span) /
[std::mdspan](https://en.cppreference.com/w/cpp/container/mdspan) for similar functionality.

## Header file and declaration
**cspan** types are defined by the *using_cspan()* macro after the header is included.
This is different from other containers where template parameters are defined prior to
including each container. This works well mainly because cspan is a non-owning type.
```c++
#include "stc/cspan.h"
using_cspan(SpanType, ValueType);                       // Define a 1-d span with ValueType elements.
using_cspan(SpanTypeN, ValueType, RANK);                // Define multi-dimensional span with RANK.
                                                        // RANK is the number (constant) of dimensions
                                                        // Has no equality test support.
using_cspan_with_eq(SpanType, ValueType, eq);           // Define a 1-d span with equality function support
using_cspan_with_eq(SpanTypeN, ValueType, eq, RANK);    // Define span with equality function support

// Shorthands:
using_cspan2(S, ValueType);                            // Define span types S, S2 with ranks 1, 2.
using_cspan3(S, ValueType);                            // Define span types S, S2, S3 with ranks 1, 2, 3.

using_cspan2_with_eq(S, ValueType, eq);                // As above, but with equality function support
using_cspan3_with_eq(S, ValueType, eq);                // Use c_default_eq for primary type elements.
```
## Methods

All index arguments are side-effect safe, e.g. `*cspan_at(&ms3, i++, j++, k++)` is safe, however `*cspan_at(&spans[n++], i, j)`
is an error, i.e. the *span* argument itself is not side-effect safe. If the number of arguments does not match the span rank,
a compile error is issued. Runtime bounds checks are enabled by default (define `STC_NDEBUG` or `NDEBUG` to disable).
```c++
SpanType        c_make(<TYPE> SpanType, {v1, v2, ...});             // make a 1-d cspan from value list
SpanType        cspan_make(<TYPE> SpanType, {v1, v2, ...});         // make a static 1-d cspan from value list
SpanType        cspan_with_n(ValueType* ptr, int32 n);              // make a 1-d cspan from a pointer and length
SpanType        cspan_from_array(ValueType array[]);                // make a 1-d cspan from a C array
SpanType        cspan_from(STCContainer* cnt);                      // make a 1-d cspan from a vec or stack

int             cspan_rank(const SpanTypeN* self);                  // num dimensions; compile-time constant
isize           cspan_size(const SpanTypeN* self);                  // return number of elements
isize           cspan_index(const SpanTypeN* self, int32 i, j..);   // offset index at i, j,...

ValueType*      cspan_at(const SpanTypeN* self, int32 i, j..);      // num args is compile-time checked
ValueType*      cspan_front(const SpanTypeN* self);
ValueType*      cspan_back(const SpanTypeN* self);

                // Construct a multi-dim span
SpanTypeN       cspan_md(ValueType* data, int32 dim1, dim2...); // row-major layout
SpanTypeN       cspan_md_layout(cspan_layout layout, ValueType* data, int32 dim1, dim2...);

                // Transpose an md span in-place. Inverses layout and axes only.
void            cspan_transpose(SpanTypeN* self);
void            cspan_swap_axes(SpanTypeN* self, int ax1, int ax2);

cspan_layout    cspan_get_layout(const SpanTypeN* self);
bool            cspan_is_rowmajor(const SpanTypeN* self);
bool            cspan_is_colmajor(const SpanTypeN* self);

                // Construct a 1d subspan. Like cspan_slice(Span, &ms, {offset, offset+count});
SpanType1       cspan_subspan(const SpanType1* self, isize offset, int32 count);

                // Construct submd span of lower rank. Like e.g. cspan_slice(Span2, &ms4, {i}, {j}, {c_ALL}, {c_ALL});
OutSpan1        cspan_submd2(const SpanType2* self, int32 i);

                // Construct a 2d or 1d subspan from a 3d span.
OutSpan2        cspan_submd3(const SpanType3* self, int32 i);
OutSpan1        cspan_submd3(const SpanType3* self, int32 i, int32 j);

                // Construct a 3d, 2d or 1d subspan from a 4d span.
OutSpan3        cspan_submd4(const SpanType4* self, int32 i);
OutSpan2        cspan_submd4(const SpanType4* self, int32 i, int32 j);
OutSpan1        cspan_submd4(const SpanType4* self, int32 i, int32 j, int32 k);

                // Multi-dim span slicing function.
                //       {i}: select i'th column. reduces output rank by one.
                //     {i,j}: from i to j-1.
                //{i,j,step}: every step column (default step=1)
                // {i,c_END}: from i to last.
                //   {c_ALL}: full extent, like {0,c_END}.
OutSpanM        cspan_slice(<TYPE> OutSpanM, const SpanTypeN* self, {x0,x1,xs}, {y0,y1,ys}.., {N0,N1,Ns});

                // Print numpy style output.
                //  fmt      : printf format specifier.
                //  fp       : optional output file pointer, default stdout.
                //  brackets : optional brackets and comma. Example "{},". Default "[]".
                //  field    : optional args macro function, must match fmt args.
                //             e.g.: #define complexfield(x) creal(x), cimag(x)
                // Examples: cspan_print(Span2, Span2_transpose(sp2)), "%.3f");
                //           cspan_print(Span2, cspan_submd(Span2, &sp3, 1), "%.3f");
void            cspan_print(<TYPE> SpanTypeN, SpanTypeN span, const char* fmt, FILE* fp = stdout,
                            const char* brackets = "[]", field(x) = x);
                // Print matrix with complex numbers. num_decimals applies both to real and imag parts.
void            cspan_print_complex(<TYPE> SpanTypeN, SpanTypeN span, int num_decimals, FILE* fp);

// Member functions

SpanTypeN       SpanTypeN_transpose(SpanTypeN sp);
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
| `cspan_layout`    | `enum { c_ROWMAJOR, c_COLMAJOR }`                   | Multi-dim layout     |
| `c_ALL`           | `cspan_slice(&md, {1,3}, {c_ALL})`                  | Full extent          |
| `c_END`           | `cspan_slice(&md, {1,c_END}, {2,c_END})`            | End of extent        |

## Example 1

[ [Run this code](https://godbolt.org/z/sdc45vrsh) ]
```c++
#include <stdio.h>
#define i_key int
#include "stc/vec.h"

#define i_key int
#include "stc/stack.h"
#include "stc/cspan.h"
using_cspan(intspan, int);

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
    printMe( (intspan)cspan_from(&vec) );

    stack_int stk = c_make(stack_int, {1, 2, 3, 4, 5, 6, 7});
    printMe( (intspan)cspan_from(&stk) );

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

[ [Run this code](https://godbolt.org/z/7ca8PqMEY) ]
```c++
#include <stdio.h>
#include "stc/cspan.h"
using_cspan3(myspan, int); // define myspan, myspan2, myspan3.

int main(void) {
    int arr[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24};

    myspan3 ms3 = cspan_md(arr, 2, 3, 4); // row-major layout
    myspan3 ss3 = cspan_slice(myspan3, &ms3, {c_ALL}, {0,3}, {2,c_END});
    puts("ss3:");
    myspan2 a = cspan_submd3(&ss3, 1);
    myspan2 b = myspan2_transpose(a);

    cspan_print(myspan3, ss3, "%d");
    puts("\nms3[1]:");
    cspan_print(myspan2, ((myspan2)cspan_submd3(&ms3, 1)), "%d");

    puts("\na:");
    cspan_print(myspan2, a, "%d");

    puts("b:");
    cspan_print(myspan2, b, "%d");

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

[ [Run this code](https://godbolt.org/z/YKx8K1hsn) ]
```c++
#include <stdio.h>
#include "stc/cspan.h"

using_cspan3(Span, int); // Shorthand to define Span, Span2, and Span3

int main(void)
{
    Span span = c_make(Span, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
                              14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24});
    Span3 span3 = cspan_md(span.data, 2, 4, 3);

    // numpy style printout
    puts("span3:");
    cspan_print(Span3, span3, "%d");

    puts("\nspan3[:, 3:4, :]:");
    Span3 ss3 = cspan_slice(Span3, &span3, {c_ALL}, {3,4}, {c_ALL});
    cspan_print(Span3, ss3, "%d");

    puts("\nspan3[:, 3, :]:");
    Span2 ss2 = cspan_slice(Span2, &span3, {c_ALL}, {3}, {c_ALL});
    cspan_print(Span2, ss2, "%d");

    puts("\nspan3 swap axes to: [1, 2, 0]");
    Span3 swapped = span3;
    cspan_swap_axes(&swapped, 0, 1);
    cspan_swap_axes(&swapped, 1, 2);
    cspan_print(Span3, swapped, "%d");
}
```
