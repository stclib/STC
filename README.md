![Standard Template Containers](docs/pics/containers.jpg)

STC - Standard Template Containers for C
========================================

Introduction
------------
A modern, templated, user-friendly, fast, fully type-safe, and customizable container library for C99,
with a uniform API across the containers, and is similar to the c++ standard library containers API.
For an introduction to templated containers, please read the blog by Ian Fisher on
[type-safe generic data structures in C](https://iafisher.com/blog/2020/06/type-safe-generics-in-c).

STC is a compact, header-only library with the all the major "standard" data containers, except for the
multimap/set variants. However, there is an example how to create a multimap in the examples folder.
- [***carray*** - **multi-dim dynamic array** type](docs/carray_api.md)
- [***cbits*** - **std::bitset** alike type](docs/cbits_api.md)
- [***cdeq*** - **std::deque** alike type](docs/cdeq_api.md)
- [***clist*** - **std::forward_list** alike type](docs/clist_api.md)
- [***cmap*** - **std::unordered_map** alike type](docs/cmap_api.md)
- [***cpque*** - **std::priority_queue** alike (adapter) type](docs/cpque_api.md)
- [***csptr*** - **std::shared_ptr** alike support](docs/csptr_api.md)
- [***cqueue*** - **std::queue** alike (adapter) type](docs/cqueue_api.md)
- [***cset*** - **std::unordered_set** alike type](docs/cset_api.md)
- [***csmap*** - **std::map** sorted map alike type](docs/csmap_api.md)
- [***csset*** - **std::set** sorted set alike type](docs/csset_api.md)
- [***cstack*** - **std::stack** alike (adapter) type](docs/cstack_api.md)
- [***cstr*** - **std::string** alike type](docs/cstr_api.md)
- [***csview*** - **std::string_view** alike type](docs/csview_api.md)
- [***cvec*** - **std::vector** alike type](docs/cvec_api.md)

Others:
- [***crandom*** - A novel extremely fast *PRNG* named **stc64**](docs/crandom_api.md)
- [***ccommon*** - Some handy macros and general definitions](docs/ccommon_api.md)

Highlights
----------
- **User friendly** - Just include the headers and you are good. The API and functionality is very close to c++ STL, and is fully listed in the docs. The ***using***-declaration instantiates the container type to use. You may pass *optional* arguments to it for customization of element- *comparison*, *destruction*, *cloning*, *conversion types*, and more.
- **Unparalleled performance** - The containers are about equal and often much faster than the c++ STL containers.
- **Fully memory managed** - All containers will destruct keys/values via destructor passed as macro parameters to the ***using***-declaration. Also, shared pointers are supported and can be stored in containers, see ***csptr***.
- **Fully type safe** - Because of templating, it avoids error-prone casting of container types and elements back and forth from the containers.
- **Uniform, easy-to-learn API** - Methods to ***construct***, ***initialize***, ***iterate*** and ***destruct*** have uniform and intuitive usage across the various containers.
- **Small footprint** - Small source code and generated executables. The executable from the example below using six different containers is *27 kb in size* compiled with TinyC.
- **Dual mode compilation** - By default it is a simple header-only library with inline and static methods only, but you can easily switch to create a traditional library with shared symbols, without changing existing source files. See the Installation section.
- **No callback functions** - All passed template argument functions/macros are directly called from the implementation, no slow callbacks which requires storage.
- **Compiles with C++ and C99** - C code can be compiled with C++.

Performance
-----------
![Benchmark](benchmarks/pics/benchmark.gif)
Benchmark notes:
- The barchart shows average test times over three platforms: Mingw64 10.30, Win-Clang 12, VC19. CPU: Ryzen 7 2700X CPU @4Ghz.
- Containers uses value types `uint64_t` and pairs of `uint64_t`for the maps.
- Black bars indicates performance variation between various platforms/compilers.
- Iterations are repeated 4 times over n elements.
- **find()**: not executed for *forward_list*, *deque*, and *vector* because these c++ containers does not have native *find()*.
- **deque**: *insert*: n/3 push_front(), n/3 push_back()+pop_front(), n/3 push_back().
- **map and unordered map**: *insert*: n/2 random numbers, n/2 sequential numbers. *erase*: n/2 keys in the map, n/2 random keys.

Usage
-----
The usage of the containers is similar to the c++ standard containers in STL, so it should be easy if you are familiar with them.
All containers are generic/templated, except for **cstr** and **cbits**. No casting is used, so containers are type-safe like
templates in c++. A basic usage example:
```c
#include <stc/cvec.h>

using_cvec(i, int);

int main(void) {
    cvec_i vec = cvec_i_init();
    cvec_i_push_back(&vec, 10);
    cvec_i_push_back(&vec, 20);
    cvec_i_push_back(&vec, 30);

    c_foreach (i, cvec_i, vec)
        printf(" %d", *i.ref);

    cvec_i_del(&vec);
}
```
With six different containers:
```c
#include <stc/cset.h>
#include <stc/cvec.h>
#include <stc/cdeq.h>
#include <stc/clist.h>
#include <stc/cqueue.h>
#include <stc/csmap.h>
#include <stdio.h>

struct Point { float x, y; };

int Point_compare(const struct Point* a, const struct Point* b) {
    if (a->x != b->x) return 1 - 2*(a->x < b->x);
    return (a->y > b->y) - (a->y < b->y);
}

// declare container types
using_cset(i, int);                         // unordered set
using_cvec(p, struct Point, Point_compare); // vector, struct as elements
using_cdeq(i, int);                         // deque
using_clist(i, int);                        // singly linked list
using_cqueue(i, cdeq_i);                    // queue, using deque as adapter
using_csmap(i, int, int);                   // sorted map

int main(void) {
    // shorthand for define + initialize containers with c_var():
    c_var (cset_i, set, {10, 20, 30});
    c_var (cvec_p, vec, { {10, 1}, {20, 2}, {30, 3} });
    c_var (cdeq_i, deq, {10, 20, 30});
    c_var (clist_i, lst, {10, 20, 30});
    c_var (cqueue_i, que, {10, 20, 30});
    c_var (csmap_i, map, { {20, 2}, {10, 1}, {30, 3} });

    // add one more element to each container
    cset_i_insert(&set, 40);
    cvec_p_push_back(&vec, (struct Point) {40, 4});
    cdeq_i_push_front(&deq, 5);
    clist_i_push_front(&lst, 5);
    cqueue_i_push(&que, 40);
    csmap_i_emplace(&map, 40, 4);

    // find an element in each container
    cset_i_iter_t i1 = cset_i_find(&set, 20);
    cvec_p_iter_t i2 = cvec_p_find(&vec, (struct Point) {20, 2});
    cdeq_i_iter_t i3 = cdeq_i_find(&deq, 20);
    clist_i_iter_t i4 = clist_i_find(&lst, 20);
    csmap_i_iter_t i5 = csmap_i_find(&map, 20);
    printf("\nFound: %d, (%g, %g), %d, %d, [%d: %d]\n", *i1.ref, i2.ref->x, i2.ref->y,
                                                        *i3.ref, *i4.ref,
                                                        i5.ref->first, i5.ref->second);
    // erase the elements found
    cset_i_erase_at(&set, i1);
    cvec_p_erase_at(&vec, i2);
    cdeq_i_erase_at(&deq, i3);
    clist_i_erase_at(&lst, i4);
    csmap_i_erase_at(&map, i5);

    printf("After erasing elements found:");
    printf("\n set:"); c_foreach (i, cset_i, set) printf(" %d", *i.ref);
    printf("\n vec:"); c_foreach (i, cvec_p, vec) printf(" (%g, %g)", i.ref->x, i.ref->y);
    printf("\n deq:"); c_foreach (i, cdeq_i, deq) printf(" %d", *i.ref);
    printf("\n lst:"); c_foreach (i, clist_i, lst) printf(" %d", *i.ref);
    printf("\n que:"); c_foreach (i, cqueue_i, que) printf(" %d", *i.ref);
    printf("\n map:"); c_foreach (i, csmap_i, map) printf(" [%d: %d]", i.ref->first, i.ref->second);

    // cleanup
    cset_i_del(&set);
    cvec_p_del(&vec);
    cdeq_i_del(&deq);
    clist_i_del(&lst);
    cqueue_i_del(&que);
    csmap_i_del(&map);
}
```
Output
```
Found: 20, (20, 2), 20, 20, [20: 2]
After erasing elements found:
 set: 10 30 40
 vec: (10, 1) (30, 3) (40, 4)
 deq: 5 10 30
 lst: 5 10 30
 que: 10 20 30 40
 map: [10: 1] [30: 3] [40: 4]
```

Installation
------------
Because it is headers-only, headers can simply be included in your program. The methods are static by default (some inlined).
You may add the *include* folder to the **CPATH** environment variable to let GCC, Clang, and TinyC locate the headers.

If containers are used across several translation units with common instantiated container types, it is recommended to
build as a "library" to minimize the executable size. To enable this mode, specify **-DSTC_HEADER** as a compiler option
in your build environment and place all the instantiations of containers used in a single C-source file, e.g.:
```c
// stc_libs.c
#define STC_IMPLEMENTATION
#include <stc/cstr.h>
#include <stc/cmap.h>
#include <stc/cvec.h>
#include <stc/clist.h>
#include "Point.h"

using_cmap(ii, int, int);
using_cset(ix, int64_t);
using_cvec(i, int);
using_clist(p, struct Point);
```

The *emplace* versus non-emplace container methods
--------------------------------------------------
STC, like c++ STL, has two sets of methods for adding elements to containers. One set begins
with **emplace**, e.g. **cvec_X_emplace_back()**. This is a convenient alternative to
**cvec_X_push_back()** when dealing non-trivial container elements, e.g. smart pointers or
elements using dynamic memory.

The **emplace** methods ***construct*** or ***clone*** their own copy of the element to be added.
In contrast, the non-emplace methods requires elements to be explicitly constructed or cloned
before adding them. For containers of integral or trivial element types, **emplace** and
corresponding non-emplace methods are identical, so the following does not apply for those.

| Move and insert element   | Construct element in-place   | Container                                   |
|:--------------------------|:-----------------------------|:--------------------------------------------|
| insert()                  | emplace()                    | cmap, cset, csmap, csset, cvec, cdeq, clist |
| insert_or_assign(), put() | emplace_or_assign()          | cmap, csmap                                 |
| push()                    | emplace()                    | cstack, cqueue, cpque                       |
| push_back()               | emplace_back()               | cvec, cdeq, clist                           |
| push_front()              | emplace_front()              | cdeq, clist                                 |

Strings are the most commonly used non-trivial data type. STC containers have proper pre-defined
**using**-declarations for cstr-elements, so they are fail-safe to use both with the **emplace**
and non-emplace methods:
```c
using_cvec_str(); // vector of string (cstr)
...
c_forvar (cvec_str vec = cvec_str_init(), cvec_str_del(&vec))   // defer vector destructor to end of block
c_forvar (cstr s = cstr_lit("a string literal"), cstr_del(&s))  // cstr_lit() for literals; no strlen() usage
{
    const char* hello = "Hello";
    cvec_str_push_back(&vec, cstr_from(hello);    // construct and add string from const char*
    cvec_str_push_back(&vec, cstr_clone(s));      // clone and add an existing cstr

    cvec_str_emplace_back(&vec, "Yay, literal");  // internally constructs cstr from string-literal
    cvec_str_emplace_back(&vec, cstr_clone(s));   // <-- COMPILE ERROR: expects const char*
    cvec_str_emplace_back(&vec, s.str);           // Ok: const char* input type.
}
```
This is made possible because the **using**-declarations may be given an optional
conversion/"rawvalue"-type as template parameter, along with a back and forth conversion
methods to the container value type. By default, *rawvalue has the same type as value*.

Rawvalues are also beneficial for **find()** and *map insertions*. The **emplace()** methods constructs
*cstr*-objects from the rawvalues, but only when required:
```c
cmap_str_emplace(&map, "Hello", "world");
// Two cstr-objects were constructed by emplace

cmap_str_emplace(&map, "Hello", "again");
// No cstr was constructed because "Hello" was already in the map.

cmap_str_emplace_or_assign(&map, "Hello", "there");
// Only cstr_from("there") constructed. "world" was destructed and replaced.

cmap_str_insert(&map, cstr_from("Hello"), cstr_from("you"));
// Two cstr's constructed outside call, but both destructed by insert
// because "Hello" existed. No mem-leak but less efficient.

it = cmap_str_find(&map, "Hello");
// No cstr constructed for lookup, although keys are cstr-type.
```
Apart from strings, maps and sets are normally used with trivial value types. However, the
last example on the **cmap** page demonstrates how to specify a map with non-trivial keys.

Erase methods
-------------
| Name                      | Description                  | Container                                   |
|:--------------------------|:-----------------------------|:--------------------------------------------|
| erase()                   | key based                    | csmap, csset, cmap, cset, cstr              |
| erase_at()                | iterator based               | csmap, csset, cmap, cset, cvec, cdeq, clist |
| erase_range()             | iterator based               | csmap, csset, cvec, cdeq, clist             |
| erase_n()                 | index based                  | cvec, cdeq, cstr                            |
| remove()                  | remove all matching values   | clist                                       |

Memory efficiency
-----------------
- **cstr**, **cvec**: Type size: one pointer. The size and capacity is stored as part of the heap allocation that also holds the vector elements.
- **clist**: Type size: one pointer. Each node allocates block storing value and next pointer.
- **cdeq**:  Type size: two pointers. Otherwise like *cvec*.
- **cmap**: Type size: 4 pointers. *cmap* uses one table of keys+value, and one table of precomputed hash-value/used bucket, which occupies only one byte per bucket. The closed hashing has a default max load factor of 85%, and hash table scales by 1.5x when reaching that.
- **csmap**: Type size: 1 pointer. *csmap* manages its own array of tree-nodes for allocation efficiency. Each node uses two 32-bit words by default for left/right child, and one byte for `level`. *csmap* can be configured to allow more than 2^32 elements, ie. 2^64, but it will double the overhead per node.
- **carray**: carray1, carray2 and carray3. Type size: One pointer plus one, two, or three size_t variables to store dimensions. Arrays are allocated as one contiguous block of heap memory.
- **csptr**: a shared-pointer uses two pointers, one for the data and one for the reference counter.
