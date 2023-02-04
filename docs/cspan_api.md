# STC [cspan](../include/stc/cspan.h): Multi-dimensional Array View
![Array](pics/array.jpg)

The **cspan** is templated non-owning multi-dimensional view of an array. See the c++ classes 
[std::span](https://en.cppreference.com/w/cpp/container/span) and 
[std::mdspan](https://en.cppreference.com/w/cpp/container/mdspan) for similar functionality.

## Header file and declaration

```c
#include <stc/cspan.h>
using_cspan(SpanType, ValueType);        // define a 1-d SpanType with ValueType elements.
using_cspan(SpanTypeN, ValueType, Rank); // define multi-dimensional span with Rank.
                                         // Rank is number of dimensions (max 5)
// Shorthands:
using_cspan2(S, ValueType);              // define span types S, S2 with ranks 1, 2.
using_cspan3(S, ValueType);              // define span types S, S2, S3 with ranks 1, 2, 3.
using_cspan4(S, ValueType);              // define span types S, S2, S3, S4 with ranks 1, 2, 3, 4.
```
## Methods
Note that `cspan_md()`, `cmake_from*()`, `cspan_atN()`, `and cspan_subspanN()` require a (safe) cast to its span-type
on assignment, but not on initialization of a span variable. All functions are type-safe, and arguments are side-effect safe, except for SpanType arg. which must not have side-effects.
```c
SpanTypeN       cspan_md(ValueType* data,  intptr_t xdim, ...);     // create a multi-dimensional cspan
SpanType        cspan_make(T SpanType, {v1, v2, ...});              // make a 1d-dimensional cspan from values
SpanType        cspan_from(STCContainer* cnt);                      // create a 1d cspan from a compatible STC container
SpanType        cspan_from_array(ValueType array[]);                // create a 1d cspan from a C array

intptr_t        cspan_size(const SpanTypeN* self);                  // return number of elements
unsigned        cspan_rank(const SpanTypeN* self);                  // return number of dimensions
intptr_t        cspan_index(const SpanTypeN* self, intptr_t x, ..); // index of element
                
ValueType*      cspan_at(const SpanTypeN* self, intptr_t x, ...);   // at(): num of args specifies rank of input span.
ValueType*      cspan_front(const SpanTypeN* self);
ValueType*      cspan_back(const SpanTypeN* self);

                // general index slicing to create a subspan.
                // {x} reduces rank. {x,c_END} slice to end. {c_ALL} take the full extent.
SpanTypeN       cspan_slice(T SpanTypeN, const SpanTypeM* parent, {x0,x1}, {y0,y1}, ...);
                
                // create a subspan of lower rank. Like e.g. cspan_slice(Span2, &ms4, {x}, {y}, {c_ALL}, {c_ALL});
SpanType        cspan_submd2(const SpanType2* self, intptr_t x);        // return a 1d subspan from a 2d span.
SpanTypeN       cspan_submd3(const SpanType3* self, intptr_t x, ...);   // return a 1d or 2d subspan from a 3d span.
SpanTypeN       cspan_submd4(const SpanType4* self, intptr_t x, ...);   // number of args determines rank of output span.

                // create a subspan of same rank. Like e.g. cspan_slice(Span3, &ms3, {off,off+count}, {c_ALL}, {c_ALL});
SpanType        cspan_subspan(const SpanType* self, intptr_t offset, intptr_t count);
SpanType2       cspan_subspan2(const SpanType2 self, intptr_t offset, intptr_t count);
SpanType3       cspan_subspan3(const SpanType3 self, intptr_t offset, intptr_t count);

SpanTypeN_iter  SpanType_begin(const SpanTypeN* self);
SpanTypeN_iter  SpanType_end(const SpanTypeN* self);
void            SpanType_next(SpanTypeN_iter* it);
```
## Types

| Type name         | Type definition                                | Used to represent... |
|:------------------|:-----------------------------------------------|:---------------------|
| SpanTypeN         | `struct { ValueType *data; uint32_t shape[N]; }` | SpanType with rank N |
| SpanTypeN`_value` | `ValueType`                                    | The ValueType        |
| `c_ALL`           | `0,-1`                                         | Full extent          |
| `c_END`           | `-1`                                           | End of extent        |

## Example 1

The *cspan_slice()* function is similar to pythons numpy multi-dimensional arrays slicing, e.g.:
```py
import numpy as np

if __name__ == '__main__':
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
... can be done in C with cspan:
```c
#include <c11/fmt.h>
#include <stc/cspan.h>
using_cspan3(myspan, int); // define myspan, myspan2, myspan3.

int main() {
    int arr[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24};

    myspan3 ms3 = cspan_md(arr, 2, 3, 4);
    myspan3 ss3 = cspan_slice(myspan3, &ms3, {c_ALL}, {1,3}, {2,c_END});
    myspan2 ss2 = cspan_submd3(&ss3, 1);

    c_FORRANGE (i, ss2.shape[0])
        c_FORRANGE (j, ss2.shape[1])
            fmt_print(" {}", *cspan_at(&ss2, i, j));
    puts("");

    c_FOREACH (i, myspan2, ss2)
        fmt_print(" {}", *i.ref);
}
```
... and (almost) in C++23:
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

    // std::mdspan can't be iterated as a flat container!
}
```
## Example 2
```c
#include <c11/fmt.h>
#include <stc/cspan.h>

using_cspan3(Span, int); // Shorthand to define Span, Span2, and Span3

int main()
{
    // c_make() can create any STC container/span from an initializer list:
    Span span = c_make(Span, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
                              14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24});
    // create a 3d cspan:
    Span3 span3 = cspan_md(span.data, 2, 4, 3);

    // reduce rank: (i.e. span3[1])
    Span2 span2 = cspan_submd3(&span3, 1);

    puts("\niterate span2 flat:");
    c_FOREACH (i, Span2, span2)
        fmt_print(" {}", *i.ref);
    puts("");

    // slice without reducing rank:
    Span3 ss3 = cspan_slice(Span3, &span3, {c_ALL}, {3,4}, {c_ALL});

    puts("\niterate ss3 by dimensions:");
    c_FORRANGE (i, ss3.shape[0]) {
        c_FORRANGE (j, ss3.shape[1]) {
            c_FORRANGE (k, ss3.shape[2])
                fmt_print(" {:2}", *cspan_at(&ss3, i, j, k));
            fmt_print(" |");
        }
        puts("");
    }
    // slice and reduce rank:
    Span2 ss2 = cspan_slice(Span2, &span3, {c_ALL}, {3}, {c_ALL});

    puts("\niterate ss2 by dimensions:");
    c_FORRANGE (i, ss2.shape[0]) {
        c_FORRANGE (j, ss2.shape[1]) {
                fmt_print(" {:2}", *cspan_at(&ss2, i, j));
            fmt_print(" |");
        }
        puts("");
    }

    puts("\niterate ss2 flat:");
    c_FOREACH (i, Span2, ss2)
        fmt_print(" {:2}", *i.ref);
    puts("");
}
```
Output:
```
iterate span2 flat:
 13 14 15 16 17 18 19 20 21 22 23 24

iterate ss3 by dimensions:
 10 11 12 |
 22 23 24 |

iterate ss2 by dimensions:
 10 | 11 | 12 |
 22 | 23 | 24 |

iterate ss2 flat:
 10 11 12 22 23 24
```
