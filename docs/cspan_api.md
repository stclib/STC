# STC [cspan](../include/stc/cspan.h): Multi-dimensional Array View
![Array](pics/array.jpg)

The **cspan** is templated non-owning multi-dimensional view of an array. 

See the c++ classes [std::span](https://en.cppreference.com/w/cpp/container/span) and 
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
Note that `cspan_make()`, `cmake_from*()`, `cspan_atN()`, `and cspan_subspanN()` require a (safe) cast to its span-type
on assignment, but not on initialization of a span variable. All functions are type-safe, and arguments are side-effect safe, except for SpanType arg. which must not have side-effects.
```c
SpanTypeN       cspan_make(ValueType* data,  size_t xdim, ...);         // make N-dimensional cspan
SpanType        cspan_from(STCContainer* cnt);                          // create a 1D cspan from a compatible STC container
SpanType        cspan_from_array(ValueType array[]);                    // create a 1D cspan from a C array

void            cspan_resize(SpanTypeN* self, size_t xdim, ...);        // change the extent of each dimension

size_t          cspan_size(const SpanTypeN* self);                      // return number of elements
unsigned        cspan_rank(const SpanTypeN* self);                      // return number of dimensions
size_t          cspan_index(const SpanTypeN* self, size_t x, ...);      // index of element
                
ValueType*      cspan_at(SpanTypeN* self, size_t x, ...);               // at(): num of args decides input SpanTypeN.
ValueType*      cspan_front(SpanTypeN* self);
ValueType*      cspan_back(SpanTypeN* self);

SpanType        cspan_subdim2(SpanType2* self, size_t x);               // return a cspan from a 2D SpanType.
SpanTypeN       cspan_subdim3(SpanType3* self, size_t x, ...);          // subdimN(): N decides input SpanType,
SpanTypeN       cspan_subdim4(SpanType4* self, size_t x, ...);          // and num of args decides returned SpanTypeN.

                // return a subspan of same rank:
SpanType        cspan_subspan(const SpanType* self, size_t offset, size_t count);
SpanType2       cspan_subspan2(const SpanType2 self, size_t offset, size_t count);
SpanType3       cspan_subspan3(const SpanType3 self, size_t offset, size_t count);
SpanType4       cspan_subspan4(const SpanType4 self, size_t offset, size_t count);

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

## Example
```c
#include <stdio.h>
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
        Span3 span3 = cspan_make(vec.data, xd, yd, zd);
        *cspan_at(&span3, 4, 3, 2) = 3.14159f;
        printf("index: %d", (int)cspan_index(&span3, 4, 3, 2));

        Span span1 = cspan_subdim3(&span3, 4, 3);
        printf("\niterate span1: ");
        c_FOREACH (i, Span, span1)
            printf("%g ", *i.ref);

        Span2 span2 = cspan_subdim3(&span3, 4);
        printf("\niterate span2: ");
        c_FOREACH (i, Span2, span2)
            printf("%g ", *i.ref);

        puts("\niterate span3 by dimensions:");
        c_FORRANGE (i, span3.dim[0]) {
            c_FORRANGE (j, span3.dim[1]) {
                c_FORRANGE (k, span3.dim[2])
                    printf(" %2g", *cspan_at(&span3, i, j, k));
                printf(" |");
            }
            puts("");
        }

        printf("%g\n", *cspan_at(&span3, 4, 3, 2));
        printf("%g\n", *cspan_at(&span2, 3, 2));
        printf("%g\n", *cspan_at(&span1, 2));
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
