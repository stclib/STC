# STC [cspan](../include/stc/cspan.h): Multi-dimensional Array View
![Array](pics/array.jpg)

The **cspan** is templated non-owning multi-dimensional view of an array. See the c++ classes 
[std::span](https://en.cppreference.com/w/cpp/container/span) and 
[std::mdspan](https://en.cppreference.com/w/cpp/container/mdspan) for similar functionality.

## Header file and declaration

```c
#include <stc/cspan.h>
using_cspan(SpanType, ValueType, Rank); // define SpanType with ValueType elements.
                                        // Rank is number of dimensions (max 4)
// Shorthands:
using_cspan2(S, ValueType);             // define span types S, S2 with ranks 1, 2.
using_cspan3(S, ValueType);             // define span types S, S2, S3 with ranks 1, 2, 3.
using_cspan4(S, ValueType);             // define span types S, S2, S3, S4 with ranks 1, 2, 3, 4.
```
## Methods
Note that `cspan_md()`, `cmake_from*()`, `cspan_atN()`, `and cspan_subspanN()` require a (safe) cast to its span-type
on assignment, but not on initialization of a span variable. All functions are type-safe, and arguments are side-effect safe, except for SpanType arg. which must not have side-effects.
```c
SpanTypeN       cspan_md(ValueType* data,  intptr_t xdim, ...);           // create a multi-dimensional cspan
SpanType        cspan_make(T SpanType, {v1, v2, ...});                  // make a 1d-dimensional cspan from values
SpanType        cspan_from(STCContainer* cnt);                          // create a 1d cspan from a compatible STC container
SpanType        cspan_from_array(ValueType array[]);                    // create a 1d cspan from a C array

intptr_t        cspan_size(const SpanTypeN* self);                      // return number of elements
unsigned        cspan_rank(const SpanTypeN* self);                      // return number of dimensions
intptr_t        cspan_index(const SpanTypeN* self, intptr_t x, ...);      // index of element
                
ValueType*      cspan_at(SpanTypeN* self, intptr_t x, ...);               // at(): num of args specifies rank of input span.
ValueType*      cspan_front(SpanTypeN* self);
ValueType*      cspan_back(SpanTypeN* self);

                // return a subspan of lower rank:
SpanType        cspan_submd2(SpanType2* self, intptr_t x);                // return a 1d subspan from a 2d span.
SpanTypeN       cspan_submd3(SpanType3* self, intptr_t x, ...);           // return a 1d or 2d subspan from a 3d span.
SpanTypeN       cspan_submd4(SpanType4* self, intptr_t x, ...);           // number of args determines rank of output span.

                // return a sliced span of same rank:
void            cspan_slice(SpanTypeN* self, {x0,x1}, {y0,y1},...);     // slice multidim span into a md subspan.

                // return a subspan of same rank. Like e.g. cspan_slice(&ms3, {offset, offset+count}, {0}, {0});
SpanType        cspan_subspan(const SpanType* self, intptr_t offset, intptr_t count);
SpanType2       cspan_subspan2(const SpanType2 self, intptr_t offset, intptr_t count);
SpanType3       cspan_subspan3(const SpanType3 self, intptr_t offset, intptr_t count);
SpanType4       cspan_subspan4(const SpanType4 self, intptr_t offset, intptr_t count);

SpanTypeN_iter  SpanType_begin(const SpanTypeN* self);
SpanTypeN_iter  SpanType_end(const SpanTypeN* self);
void            SpanType_next(SpanTypeN_iter* it);
```
## Types

| Type name         | Type definition                                | Used to represent... |
|:------------------|:-----------------------------------------------|:---------------------|
| SpanTypeN         | `struct { ValueType *data; uint32_t dim[N]; }` | SpanType with rank N |
| SpanTypeN`_value` | `ValueType`                                    | The ValueType        |
| SpanTypeN`_iter`  | `struct { ValueType *ref; ... }`               | Iterator type        |

## Example 1

The *cspan_slice()* function is similar to pythons numpy multi-dimensional arrays slicing, e.g.:
```py
import numpy as np
ms3 = np.array((1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24), int)

ms3 = np.reshape(ms3, (2, 3, 4))
ss3 = ms3[:, 1:3, 2:]
ss2 = ss3[1]

for i in range(ss2.shape[0]):
    for j in range(ss2.shape[1]):
        print(" {}".format(ss2[i, j]), end='')
print('')

for i in ss2.flat:
    print(" {}".format(i), end='')
# 19 20 23 24
# 19 20 23 24
```
... can be done in C with STC:
```c
#include <c11/fmt.h>
#include <stc/cspan.h>
using_cspan3(myspan, int); // define myspan, myspan2, myspan3.

int main() {
    int arr[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24};

    myspan3 ms3 = cspan_md(arr, 2, 3, 4), ss3 = ms3;
    cspan_slice(&ss3, {0}, {1,3}, {2,});
    myspan2 ss2 = cspan_submd3(&ss3, 1);

    c_FORRANGE (i, ss2.dim[0])
        c_FORRANGE (j, ss2.dim[1])
            fmt_print(" {}", *cspan_at(&ss2, i, j));
    puts("");

    c_FOREACH (i, myspan2, ss2)
        fmt_print(" {}", *i.ref);
}
```
... or (mostly) in C++23:
```c++
#include <print>
#include <mdspan>
#include <tuple>

int main() {
    int arr[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24};

    std::mdspan ms3(arr, 2, 3, 4);
    auto ss3 = std::submdspan(ms3, std::full_extent, std::tuple{1,3}, std::tuple{2,4});
    auto ss2 = std::submdspan(ss3, 1, std::full_extent, std::full_extent);

    for (std::size_t i = 0; i < ss2.extent(0); ++i)
        for (std::size_t j = 0; j < ss2.extent(1); ++j)
            std::print(" {}", ss2[i, j]);
    std::println();

    // mdspan can't printed as a flat array, afaik.
}
```
## Example 2
```c
#include <c11/fmt.h>
#include <stc/cspan.h>
#define i_val float
#include <stc/cstack.h>

using_cspan3(Span, float); // Shorthand to define span types Span, Span2, and Span3.

int main()
{
    int xd = 6, yd = 4, zd = 3;
    c_AUTO (cstack_float, vec)
    {
        c_FORRANGE (i, xd*yd*zd)
            cstack_float_push(&vec, i);

        // define "span3[xd][yd][zd]"
        Span3 span3 = cspan_md(vec.data, xd, yd, zd);
        *cspan_at(&span3, 4, 3, 2) = 3.14159f;
        fmt_print("index: {}", cspan_index(&span3, 4, 3, 2));

        Span span1 = cspan_submd3(&span3, 4, 3);
        printf("\niterate span1: ");
        c_FOREACH (i, Span, span1)
            fmt_print("{} ", *i.ref);

        Span2 span2 = cspan_submd3(&span3, 4);
        printf("\niterate span2: ");
        c_FOREACH (i, Span2, span2)
            fmt_print("{} ", *i.ref);

        puts("\niterate span3 by dimensions:");
        c_FORRANGE (i, span3.dim[0]) {
            c_FORRANGE (j, span3.dim[1]) {
                c_FORRANGE (k, span3.dim[2])
                    fmt_printf(" {:2}", *cspan_at(&span3, i, j, k));
                printf(" |");
            }
            puts("");
        }

        fmt_println("{}", *cspan_at(&span3, 4, 3, 2));
        fmt_println("{}", *cspan_at(&span2, 3, 2));
        fmt_println("{}", *cspan_at(&span1, 2));
    }
}
```
Output:
```
index: 59
iterate span1: 57 58 3.14159 
iterate span2: 48 49 50 51 52 53 54 55 56 57 58 3.14159 
iterate span3 by dimensions:
  0  1  2 |  3  4  5 |  6  7  8 |  9 10 11 |
 12 13 14 | 15 16 17 | 18 19 20 | 21 22 23 |
 24 25 26 | 27 28 29 | 30 31 32 | 33 34 35 |
 36 37 38 | 39 40 41 | 42 43 44 | 45 46 47 |
 48 49 50 | 51 52 53 | 54 55 56 | 57 58 3.14159 |
 60 61 62 | 63 64 65 | 66 67 68 | 69 70 71 |
3.14159
3.14159
3.14159
```
