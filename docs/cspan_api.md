# STC [cspan](../include/stc/cspan.h): Multi-dimensional Array View
![Array](pics/array.jpg)

The **cspan** types are templated non-owning *single* and *multi-dimensional* views of an array.
It supports both row-major and column-major layout efficiently, in a addition to slicing
capabilities similar to [python's numpy arrays](https://numpy.org/doc/stable/user/basics.indexing.html).
Note that each dimension is currently limited to int32_t sizes and 8 dimensions (can be extended).

See also C++
[std::span](https://en.cppreference.com/w/cpp/container/span) /
[std::mdspan](https://en.cppreference.com/w/cpp/container/mdspan) for similar functionality.

## Header file and declaration
**cspan** types are defined by the *using_cspan()* macro after the header is included.
This is different from other containers where template parameters are defined prior to
including each container. This works well mainly because cspan is non-owning.
```c
#include "stc/cspan.h"
using_cspan(SpanType, ValueType);        // define a 1-d SpanType with ValueType elements.
using_cspan(SpanTypeN, ValueType, RANK); // define multi-dimensional span with RANK.
                                         // RANK is the literal number of dimensions
// Shorthands:
using_cspan2(S, ValueType);              // define span types S, S2 with ranks 1, 2.
using_cspan3(S, ValueType);              // define span types S, S2, S3 with ranks 1, 2, 3.
using_cspan4(S, ValueType);              // define span types S, S2, S3, S4 with ranks 1, 2, 3, 4.
```
## Methods

All functions are type-safe. NOTE: the span argument itself is generally **not** side-effect safe -
it may be expanded multiple times. However, all index arguments are safe, e.g.
`cspan_at(&ms3, i++, j++, k++)` is safe, but `cspan_at(&spans[n++], i, j)` is an error! If the number
of arguments does not match the span rank, a compile error is issued. Runtime bounds checks are enabled
by default (define `STC_NDEBUG` or `NDEBUG` to disable).
```c
SpanType        cspan_init(<TYPE> SpanType, {v1, v2, ...});         // make a 1-d cspan from value list
SpanType        cspan_from_n(ValueType* ptr, intptr_t n);           // make a 1-d cspan from a pointer and length
SpanType        cspan_from_array(ValueType array[]);                // make a 1-d cspan from a C array
SpanType        cspan_from(STCContainer* cnt);                      // make a 1-d cspan from a vec or stack

int             cspan_rank(const SpanTypeN* self);                  // num dimensions; compile-time constant
intptr_t        cspan_size(const SpanTypeN* self);                  // return number of elements
intptr_t        cspan_index(const SpanTypeN* self, intptr_t i, j..); // offset index at i, j,...

ValueType*      cspan_at(const SpanTypeN* self, intptr_t i, j..);  // num args is compile-time checked
ValueType*      cspan_front(const SpanTypeN* self);
ValueType*      cspan_back(const SpanTypeN* self);

                // print numpy style output.
                //  span     : any dimension. Note that span is passed by value.
                //  fmt      : printf format specifier.
                //  fp       : optional output file pointer, default stdout.
                //  brackets : optional brackets and comma. Example "{},". Default "[]".
                //  itemfn   : optional macro function
                //    Example: #define complexnum(e) e.real, e.imag
                //    Default: itemfn(e) e
                //    Note that fmt must match the arguments passed via itemfn().
                // Usage ex: cspan_print(Span2, ((Span2)cspan_transposed2(&sp2)), "%.3f");
                //           cspan_print(Span2, cspan_slice(Span2, &sp3, {c_ALL}, {3}, {c_ALL}), "%.3f");
void            cspan_print(<TYPE> SpanTypeN, SpanTypeN span, const char* fmt,
                            FILE* fp = stdout, const char* brackets = "[]", itemfn = c_EXPAND);

SpanTypeN_iter  SpanType_begin(const SpanTypeN* self);
SpanTypeN_iter  SpanType_end(const SpanTypeN* self);
void            SpanType_next(SpanTypeN_iter* it);

                // construct a multi-dim span
SpanTypeN       cspan_md(ValueType* data, intptr_t d1, d2...); // row-major layout
SpanTypeN       cspan_md_layout(cspan_layout layout, ValueType* data, intptr_t d1, d2...);

                // transpose an md span. Inverses layout and axes only.
void            cspan_swap_axes(SpanTypeN* self, int ax1, int ax2);
void            cspan_transpose(SpanTypeN* self);
SpanType2       cspan_transposed2(const SpanType2* self);       // constructor
cspan_layout    cspan_get_layout(const SpanTypeN* self);
bool            cspan_is_rowmajor(const SpanTypeN* self);
bool            cspan_is_colmajor(const SpanTypeN* self);

                // construct a 1d subspan. Like cspan_slice(Span, &ms, {offset, offset+count});
SpanType1       cspan_subspan(const SpanType1* self, intptr_t offset, intptr_t count);

                // construct submd span of lower rank. Like e.g. cspan_slice(Span2, &ms4, {i}, {j}, {c_ALL}, {c_ALL});
OutSpan1        cspan_submd2(const SpanType2* self, intptr_t i);     // construct a 1d subspan from a 2d span.
OutSpanM        cspan_submd3(const SpanType3* self, intptr_t i,...); // construct a 2d or 1d subspan from a 3d span.
OutSpanM        cspan_submd4(const SpanType4* self, intptr_t i,...); // construct a 3d, 2d or 1d subspan from a 4d span.

                // multi-dim span slicing function.
                //       {i}: select i'th column. reduce output rank.
                //     {i,j}: from i to j-1.
                //   {i,j,s}: every s column only (default s=1)
                // {i,c_END}: from i to last.
                //   {c_ALL}: full extent, like {0,c_END}.
OutSpanM        cspan_slice(<TYPE> OutSpanM, const SpanTypeN* self, {x0,x1,xs}, {y0,y1,ys}.., {N0,N1,Ns});
```
## Types
| Type name         | Type definition / usage                             | Used to represent... |
|:------------------|:----------------------------------------------------|:---------------------|
| STC_CSPAN_INDEX_TYPE | Defines `cextent_t`, default `int32_t`           | Index type           |
| SpanTypeN_value   | `ValueType`                                         | The ValueType        |
| SpanTypeN         | `struct { ValueType *data; cextent_t shape[N]; .. }`| SpanType with rank N |
| `cspan_tupleN`    | `struct { cextent_t d[N]; }`                        | Strides for each rank |
| `cspan_layout`    | `enum { c_ROWMAJOR, c_COLMAJOR }`                   | Multi-dim layout     |
| `c_ALL`           | `cspan_slice(&md, {1,3}, {c_ALL})`                  | Full extent          |
| `c_END`           | `cspan_slice(&md, {1,c_END}, {2,c_END})`            | End of extent        |

## Example 1

[ [Run this code](https://godbolt.org/z/qrdcjPaqK) ]
```c
#include <stdio.h>
#define i_key int
#include "stc/vec.h"

#define i_key int
#include "stc/stack.h"

#include "stc/cspan.h"
using_cspan(intspan, int);

void printMe(intspan container) {
    printf("%d:", (int)cspan_size(&container));
    c_foreach (e, intspan, container)
        printf(" %d", *e.ref);
    puts("");
}

int main(void)
{
    printMe( c_init(intspan, {1, 2, 3, 4}) );

    int arr[] = {1, 2, 3, 4, 5};
    printMe( (intspan)cspan_from_array(arr) );

    vec_int vec = c_init(vec_int, {1, 2, 3, 4, 5, 6});
    printMe( (intspan)cspan_from(&vec) );

    stack_int stk = c_init(stack_int, {1, 2, 3, 4, 5, 6, 7});
    printMe( (intspan)cspan_from(&stk) );

    intspan spn = c_init(intspan, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12});
    printMe( (intspan)cspan_subspan(&spn, 2, 8) );

    // cleanup
    vec_int_drop(&vec);
    stack_int_drop(&stk);
}
```

## Example 2

Multi-dimension slicing (first in python):
```py
import numpy as np

def print_myspan2(s2):
    for i in range(s2.shape[0]):
        for j in range(s2.shape[1]):
            print(" {}".format(s2[i, j]), end='')
        print('')
    print('')

if __name__ == '__main__':
    ms3 = np.array((1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24), int)

    ms3 = np.reshape(ms3, (2, 3, 4), order='C')
    ss3 = ms3[:, 0:3, 2:]
    a = ss3[1]
    b = np.transpose(a)

    print_myspan2(ms3[1])
    print_myspan2(a)
    print_myspan2(b)

    for i in a.flat: print(" {}".format(i), end='')
    print('')
    for i in b.flat: print(" {}".format(i), end='')
'''
 13 14 15 16
 17 18 19 20
 21 22 23 24

 15 16
 19 20
 23 24

 15 19 23
 16 20 24

 15 16 19 20 23 24
 15 19 23 16 20 24
'''
```
... in C with STC cspan:

[ [Run this code](https://godbolt.org/z/q5rT89Eze) ]
```c
#include <stdio.h>
#include "stc/cspan.h"

using_cspan3(myspan, int); // define myspan, myspan2, myspan3.

void print_myspan2(myspan2 ms) {
    for (int i=0; i < ms.shape[0]; ++i) {
        for (int j=0; j < ms.shape[1]; ++j)
            printf(" %2d", *cspan_at(&ms, i, j));
        puts("");
    }
    puts("");
}

int main(void) {
    int arr[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24};

    myspan3 ms3 = cspan_md(arr, 2, 3, 4); // row-major layout
    myspan3 ss3 = cspan_slice(myspan3, &ms3, {c_ALL}, {0,3}, {2,c_END});

    myspan2 a = cspan_submd3(&ss3, 1);
    myspan2 b = cspan_transposed2(&a);

    cspan_print(myspan3, ss3); // C11. Add "%d" argument for C99 comp.
    puts("");
    print_myspan2((myspan2)cspan_submd3(&ms3, 1));
    print_myspan2(a);
    print_myspan2(b);

    c_foreach (i, myspan2, a) printf(" %d", *i.ref);
    puts("");
    c_foreach (i, myspan2, b) printf(" %d", *i.ref);
    puts("");
}
```

## Example 3
Slicing cspan without and with reducing the rank:

[ [Run this code](https://godbolt.org/z/EdGzxeM4b) ]
```c
#include <stdio.h>
#include "stc/cspan.h"

using_cspan3(Span, int); // Shorthand to define Span, Span2, and Span3

int main(void)
{
    Span span = c_init(Span, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
                              14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24});
    Span3 span3 = cspan_md(span.data, 2, 4, 3);

    // numpy style printout
    puts("span3:");
    cspan_print(Span3, span3, "%d");

    puts("Slice without reducing rank:");
    Span3 ss3 = cspan_slice(Span3, &span3, {c_ALL}, {3,4}, {c_ALL});
    cspan_print(Span3, ss3);

    puts("Slice with reducing rank:");
    Span2 ss2 = cspan_slice(Span2, &span3, {c_ALL}, {3}, {c_ALL});
    cspan_print(Span2, ss2);

    puts("Swapped:");
    Span3 swapped = span3;
    cspan_swap_axes(&swapped, 0, 1);
    cspan_swap_axes(&swapped, 1, 2);
    cspan_print(Span3, swapped);
}
```
