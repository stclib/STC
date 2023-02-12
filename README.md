![STC](docs/pics/containers.jpg)

STC - Smart Template Containers for C
=====================================

News: Version 4.1 Released (Feb 2023)
------------------------------------------------
I am happy to finally announce a new release! Major changes:
- A new exciting [**cspan**](docs/cspan_api.md) single/multi-dimensional array view (with numpy-like slicing).
- Signed sizes and indices for all containers. See C++ Core Guidelines by Stroustrup/Sutter: [ES.100](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#es100-dont-mix-signed-and-unsigned-arithmetic), [ES.102](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#es102-use-signed-types-for-arithmetic), [ES.106](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#es106-dont-try-to-avoid-negative-values-by-using-unsigned), and [ES.107](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#es107-dont-use-unsigned-for-subscripts-prefer-gslindex).
- Customizable allocator [per templated container type](https://github.com/tylov/STC/discussions/44#discussioncomment-4891925).
- Updates on **cregex** with several [new unicode character classes](docs/cregex_api.md#regex-cheatsheet).
- Algorithms:
    - [crange](docs/ccommon_api.md#crange) - similar to [boost::irange](https://www.boost.org/doc/libs/release/libs/range/doc/html/range/reference/ranges/irange.html) integer range generator.
    - [c_forfilter](docs/ccommon_api.md#c_forfilter) - ranges-like filtering.
    - [csort](misc/benchmarks/various/csort_bench.c) - fast quicksort with custom inline comparison.
- Support for [uppercase flow-control](include/stc/priv/altnames.h) macro names in ccommon.h.
- Create single header container versions with python script.
- Some API changes in cregex and cstr.
- [Previous changes for version 4](#version-4).

Introduction
------------
STC is a *modern*, *ergonomic*, *type-safe*, *very fast* and *compact* container library for C99.
The API has similarities with c++ STL, but is more uniform across the containers and takes inspiration from Rust
and Python as well. It is an advantage to know how these containers work in other languages, like Java, C# or C++,
but it's not required.

This library allows you to manage both trivial to very complex data in a wide variety of containers
without the need for boilerplate code. You may specify element-cloning, -comparison, -destruction and
more on complex container hierarchies without resorting to cumbersome function pointers with type casting.
Usage with trivial data types is simple to use compared to most generic container libraries for C because
of its type safety with an intuitive and consistent API.

The library is mature and well tested, so you may use it in projects. However, minor breaking API changes may
still happen. The main development of this project is finished, but I will handle PRs with bugs and improvements
in the future, and do minor modifications.

Containers
----------
- [***carc*** - **std::shared_ptr** alike type](docs/carc_api.md)
- [***cbox*** - **std::unique_ptr** alike type](docs/cbox_api.md)
- [***cbits*** - **std::bitset** alike type](docs/cbits_api.md)
- [***clist*** - **std::forward_list** alike type](docs/clist_api.md)
- [***cqueue*** - **std::queue** alike type](docs/cqueue_api.md)
- [***cpque*** - **std::priority_queue** alike type](docs/cpque_api.md)
- [***cmap*** - **std::unordered_map** alike type](docs/cmap_api.md)
- [***cset*** - **std::unordered_set** alike type](docs/cset_api.md)
- [***csmap*** - **std::map** sorted map alike type](docs/csmap_api.md)
- [***csset*** - **std::set** sorted set alike type](docs/csset_api.md)
- [***cstack*** - **std::stack** alike type](docs/cstack_api.md)
- [***cstr*** - **std::string** alike type](docs/cstr_api.md)
- [***csview*** - **std::string_view** alike type](docs/csview_api.md)
- [***cspan*** - **std::span/std::mdspan** alike type](docs/cspan_api.md)
- [***cdeq*** - **std::deque** alike type](docs/cdeq_api.md)
- [***cvec*** - **std::vector** alike type](docs/cvec_api.md)

Others
------
- [***ccommon*** - Generic safe macros and algorithms](docs/ccommon_api.md)
- [***cregex*** - Regular expressions (extended from Rob Pike's regexp9)](docs/cregex_api.md)
- [***crandom*** - A novel very fast *PRNG* named **stc64**](docs/crandom_api.md)
- [***coption*** - getopt() alike command line args parser](docs/coption_api.md)

Highlights
----------
- **No boilerplate** - With STC, no boilerplate code is needed to setup containers either with simple built-in types or containers with complex element types, which requires cleanup. Only specify what is needed, e.g. compare-, clone-, drop- functions, and leave the rest as defaults.
- **Fully type safe** - Because of templating, it avoids error-prone casting of container types and elements back and forth from the containers.
- **User friendly** - Just include the headers and you are good. The API and functionality is very close to c++ STL, and is fully listed in the docs. 
- **Templates** - Use `#define i_{arg}` to specify container template arguments. There are templates for element-*type*, -*comparison*, -*destruction*, -*cloning*, -*conversion types*, and more.
- **Unparalleled performance** - Some containers are much faster than the c++ STL containers, the rest are about equal in speed.
- **Fully memory managed** - All containers will destruct keys/values via destructor defined as macro parameters before including the container header. Also, smart pointers are supported and can be stored in containers, see ***carc*** and ***cbox***.
- **Uniform, easy-to-learn API** - Methods to ***construct***, ***initialize***, ***iterate*** and ***destruct*** have uniform and intuitive usage across the various containers.
- **No signed/unsigned mixing** - Unsigned sizes and indices mixed with signed in comparisons and calculations is asking for trouble. STC uses only signed numbers in the API.
- **Small footprint** - Small source code and generated executables. The executable from the example below using ***six different*** container types is only ***19 Kb in size*** compiled with gcc -O3 -s on Linux.
- **Dual mode compilation** - By default it is a simple header-only library with inline and static methods only, but you can easily switch to create a traditional library with shared symbols, without changing existing source files. See the Installation section.
- **No callback functions** - All passed template argument functions/macros are directly called from the implementation, no slow callbacks which requires storage.
- **Compiles with C++ and C99** - C code can be compiled with C++ (container element types must be POD).
- **Forward declaration** - Templated containers may be forward declared without including the full API/implementation. See documentation below.

Performance
-----------
![Benchmark](misc/benchmarks/pics/benchmark.gif)

Benchmark notes:
- The barchart shows average test times over three platforms: Mingw64 10.30, Win-Clang 12, VC19. CPU: Ryzen 7 2700X CPU @4Ghz.
- Containers uses value types `uint64_t` and pairs of `uint64_t` for the maps.
- Black bars indicates performance variation between various platforms/compilers.
- Iterations are repeated 4 times over n elements.
- **find()**: not executed for *forward_list*, *deque*, and *vector* because these c++ containers does not have native *find()*.
- **deque**: *insert*: n/3 push_front(), n/3 push_back()+pop_front(), n/3 push_back().
- **map and unordered map**: *insert*: n/2 random numbers, n/2 sequential numbers. *erase*: n/2 keys in the map, n/2 random keys.

STC conventions
---------------
- Container names are prefixed by `c`, e.g. `cvec`, `cstr`.
- Public STC macros are prefixed by `c_`, e.g. `c_foreach`, `c_make`.
- Template parameter macros are prefixed by `i_`, e.g. `i_val`, `i_type`.
- All containers can be initialized with `{0}`, i.e. no heap allocation used by default init.
- Common types for a container type Con:
    - Con
    - Con_value
    - Con_raw
    - Con_iter
    - Con_ssize
- Common function names for a container type Con:
    - Con_init()
    - Con_reserve(&con, capacity)
    - Con_drop(&con)
    - Con_empty(&con)
    - Con_size(&con)
    - Con_clone(con)
    - Con_push(&con, value)
    - Con_emplace(&con, rawval)
    - Con_put_n(&con, rawval[], n)
    - Con_erase_at(&con, iter)
    - Con_front(&con)
    - Con_back(&con)
    - Con_begin(&con)
    - Con_end(&con)
    - Con_next(&iter)
    - Con_advance(iter, n)

Some standout features of STC
-----------------------------
1. ***Centralized analysis of template arguments***. Assigns good defaults to non-specified templates.
You may specify a number of "standard" template arguments for each container, but as minimum only one is
required (two for maps). In the latter case, STC assumes the elements are basic types. For more complex types,
additional template arguments should be defined.
2. ***General "heterogeneous lookup"-like feature***. Allows specification of an alternative type to use
for lookup in containers. E.g. for containers with string type (**cstr**) elements, `const char*` is used
as lookup type. It will then use the input `const char*` directly when comparing with the string data in the
container. This avoids the construction of a new `cstr` (which possible allocates memory) for the lookup. 
Finally, destruction of the lookup key (i.e. string literal) after usage is not needed (or allowed), which 
is convenient in C. A great ergonomic feature is that the alternative lookup type can also be used for adding
entries into containers through using the *emplace*-functions. E.g. `MyCStrVec_emplace_back(&vec, "Hello")`.
3. ***Standardized container iterators***. All container can be iterated the same way, and uses the
same element access syntax. E.g.:
    - `c_foreach (it, IntContainer, container) printf(" %d", *it.ref);` will work for
every type of container defined as `IntContainer` with `int` elements. Also the form:
    - `c_foreach (it, IntContainer, it1, it2)`
may be used to iterate from `it1` up to `it2`.

Usage
-----
The usage of the containers is similar to the c++ standard containers in STL, so it should be easy if you
are familiar with them. All containers are generic/templated, except for **cstr** and **cbits**.
No casting is used, so containers are type-safe like templates in c++. A basic usage example:
```c
#define i_type FVec    // Container type name; if not defined, it would be cvec_float
#define i_val float    // Container element type
#include <stc/cvec.h>

int main(void)
{
    FVec vec = FVec_init();
    FVec_push(&vec, 10.f);
    FVec_push(&vec, 20.f);
    FVec_push(&vec, 30.f);

    for (intptr_t i = 0; i < FVec_size(vec); ++i)
        printf(" %g", vec.data[i]);

    FVec_drop(&vec); // cleanup memory
}
```
Below is an alternative way to write this with STC. It uses the generic flow control macros `c_auto` and `c_foreach`, and the function macro *c_make()*. This simplifies the code and makes it less prone to errors:
```c
int main()
{
    c_auto (FVec, vec)                          // RAII: define, init() and drop() combined.
    {
        vec = c_make(FVec, {10.f, 20.f, 30.f}); // Initialize with a list of floats.

        c_foreach (i, FVec, vec)                // Iterate elements of the container.
            printf(" %g", *i.ref);              // i.ref is a pointer to the current element.
    }
    // vec is "dropped" at end of c_auto scope
}
```
For user defined struct elements, `i_cmp` compare function should be defined, as the default `<` and `==`
only works for integral types. *Alternatively, `#define i_opt c_no_cmp` to disable sorting and searching*.

Similarily, if an element destructor `i_valdrop` is defined, `i_valclone` function is required.
*Alternatively `#define i_opt c_no_clone` to disable container cloning.*

In order to include two **cvec**'s with different element types, include <stc/cvec.h> twice:
```c
#define i_val struct One
#define i_opt c_no_cmp
#define i_tag one
#include <stc/cvec.h>

#define i_val struct Two
#define i_opt c_no_cmp
#define i_tag two
#include <stc/cvec.h>
...
cvec_one v1 = cvec_one_init();
cvec_two v2 = cvec_two_init();
```

An example using six different container types:
```c
#include <stdio.h>
#include <stc/ccommon.h>

struct Point { float x, y; };
int Point_cmp(const struct Point* a, const struct Point* b);

#define i_key int
#include <stc/cset.h>  // cset_int: unordered set

#define i_val struct Point
#define i_cmp Point_cmp
#define i_tag pnt
#include <stc/cvec.h>  // cvec_pnt: vector of struct Point

#define i_val int
#include <stc/cdeq.h>  // cdeq_int: deque of int

#define i_val int
#include <stc/clist.h> // clist_int: singly linked list

#define i_val int
#include <stc/cstack.h>

#define i_key int
#define i_val int
#include <stc/csmap.h> // csmap_int: sorted map int => int

int Point_cmp(const struct Point* a, const struct Point* b) {
    int cmp = c_default_cmp(&a->x, &b->x);
    return cmp ? cmp : c_default_cmp(&a->y, &b->y);
}

int main(void)
{
    /* Define six containers with automatic call of init and drop (destruction after scope exit) */
    c_auto (cset_int, set)
    c_auto (cvec_pnt, vec)
    c_auto (cdeq_int, deq)
    c_auto (clist_int, lst)
    c_auto (cstack_int, stk)
    c_auto (csmap_int, map)
    {
        int nums[4] = {10, 20, 30, 40};
        struct Point pts[4] = { {10, 1}, {20, 2}, {30, 3}, {40, 4} };
        int pairs[4][2] = { {20, 2}, {10, 1}, {30, 3}, {40, 4} };
        
        /* Add some elements to each container */
        for (int i = 0; i < 4; ++i) {
            cset_int_insert(&set, nums[i]);
            cvec_pnt_push(&vec, pts[i]);
            cdeq_int_push_back(&deq, nums[i]);
            clist_int_push_back(&lst, nums[i]);
            cstack_int_push(&stk, nums[i]);
            csmap_int_insert(&map, pairs[i][0], pairs[i][1]);
        }

        /* Find an element in each container (except cstack) */
        cset_int_iter i1 = cset_int_find(&set, 20);
        cvec_pnt_iter i2 = cvec_pnt_find(&vec, (struct Point){20, 2});
        cdeq_int_iter i3 = cdeq_int_find(&deq, 20);
        clist_int_iter i4 = clist_int_find(&lst, 20);
        csmap_int_iter i5 = csmap_int_find(&map, 20);

        printf("\nFound: %d, (%g, %g), %d, %d, [%d: %d]\n",
                *i1.ref, i2.ref->x, i2.ref->y, *i3.ref,
                *i4.ref, i5.ref->first, i5.ref->second);
        
        /* Erase the elements found */
        cset_int_erase_at(&set, i1);
        cvec_pnt_erase_at(&vec, i2);
        cdeq_int_erase_at(&deq, i3);
        clist_int_erase_at(&lst, i4);
        csmap_int_erase_at(&map, i5);

        printf("After erasing the elements found:");
        printf("\n set:");
        c_foreach (i, cset_int, set)
            printf(" %d", *i.ref);
        
        printf("\n vec:");
        c_foreach (i, cvec_pnt, vec)
            printf(" (%g, %g)", i.ref->x, i.ref->y);
        
        printf("\n deq:");
        c_foreach (i, cdeq_int, deq)
            printf(" %d", *i.ref);
        
        printf("\n lst:");
        c_foreach (i, clist_int, lst)
            printf(" %d", *i.ref);
        
        printf("\n stk:");
        c_foreach (i, cstack_int, stk)
            printf(" %d", *i.ref);
        
        printf("\n map:");
        c_foreach (i, csmap_int, map)
            printf(" [%d: %d]", i.ref->first, i.ref->second);
    }
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
 stk: 10 20 30 40
 map: [10: 1] [30: 3] [40: 4]
```

Installation
------------
Because it is headers-only, headers can simply be included in your program. By default, functions are static
(some inlined). You may add the *include* folder to the **CPATH** environment variable to
let GCC, Clang, and TinyC locate the headers.

If containers are used across several translation units with common instantiated container types, it is 
recommended to build as a "library" with external linking to minimize executable size. To enable this,
specify `-DSTC_HEADER` as compiler option in your build environment. Next, place all the instantiations
of the containers used inside a single C-source file as in the example below, and `#define STC_IMPLEMENT` at top. 
You may also cherry-pick shared linking mode on individual containers by `#define i_header` and
`#define i_implement`, or force static symbols by `#define i_static` before container includes.

As a special case, there may be non-templated functions in templated containers that should be implemented only
once and if needed. Currently, define `i_extern` before including **clist** for its sorting function, and before 
**cregex** or **utf8** to implement them (global `STC_EXTERN` can alternatively be defined).

It is possible to generate single headers by executing the python script `src/singleheader.py header-file > single`.

Conveniently, `src\libstc.c` implements non-templated functions as shared symbols for **cstr**, **csview**,
**cbits** and **crandom**. When building in shared mode (-DSTC_HEADER), you may include this file in your project,
or define your own as descibed above.
```c
// stc_libs.c
#define STC_IMPLEMENT

#include <stc/cstr.h>
#include "Point.h"

#define i_key int
#define i_val int
#define i_tag ii
#include <stc/cmap.h>  // cmap_ii: int => int

#define i_key int64_t
#define i_tag ix
#include <stc/cset.h>  // cset_ix

#define i_val int
#include <stc/cvec.h>  // cvec_int

#define i_val Point
#define i_tag pnt
#include <stc/clist.h> // clist_pnt
```

Specifying template parameters
------------------------------
Each templated type requires one `#include`, even if it's the same container base type, as described earlier.
The template parameters are given by a `#define i_xxxx` statement, where *xxxx* is the parameter name.
The list of template parameters:

- `i_key`     - Element key type for map/set only. **[required]**.
- `i_val`     - Element value type. **[required for]** cmap/csmap, it is the mapped value type.
- `i_cmp`     - Three-way comparison of two *i_keyraw*\* or *i_valraw*\* - **[required for]** non-integral *i_keyraw* elements unless *i_opt* is defined with *c_no_cmp*.
- `i_hash`    - Hash function taking *i_keyraw*\* - defaults to *c_default_hash*. **[required for]** ***cmap/cset*** with non-POD *i_keyraw* elements.
- `i_eq`      - Equality comparison of two *i_keyraw*\* - defaults to *!i_cmp*. Companion with *i_hash*.

Properties:
- `i_tag`     - Container type name tag. Defaults to *i_key* name.
- `i_type`    - Full container type name. Alternative to *i_tag*.
- `i_opt`     - Boolean properties: may combine *c_no_cmp*, *c_no_clone*, *c_no_atomic*, *c_is_forward*, *c_static*, *c_header* with the *|* separator.

Key:
- `i_keydrop` - Destroy map/set key func - defaults to empty destructor.
- `i_keyclone` - **[required if]** *i_keydrop* is defined (exception for **carc**, as it shares).
- `i_keyraw`  - Convertion "raw" type - defaults to *i_key*.
- `i_keyfrom` - Convertion func *i_key* <- *i_keyraw*. 
- `i_keyto`   - Convertion func *i_key*\* -> *i_keyraw*. **[required if]** *i_keyraw* is defined

Val:
- `i_valdrop` - Destroy mapped or value func - defaults to empty destruct.
- `i_valclone` - **[required if]** *i_valdrop* is defined. 
- `i_valraw`  - Convertion "raw" type - defaults to *i_val*.
- `i_valfrom` - Convertion func *i_val* <- *i_valraw*.
- `i_valto`   - Convertion func *i_val*\* -> *i_valraw*.

Specials (meta-template parameters):
- `i_keyclass TYPE` - Auto-binds to standard named functions: *TYPE_clone()*, *TYPE_drop()*, *TYPE_cmp()*, *TYPE_eq()*, *TYPE_hash()*. If `i_keyraw` is defined, function *TYPE_toraw()* is bound to `i_keyto`, and *TYPE_from()* binds to `i_keyfrom`. Only functions required by the container type needs to be defined. E.g.:
    - *TYPE_hash()* and *TYPE_eq()* are only required by **cmap**, **cset** and smart pointers.
    - *TYPE_cmp()* is not used by **cstack** and **cmap/cset**, or if *#define i_opt c_no_cmp* is specified.
    - *TYPE_clone()* is not used by if *#define i_opt c_no_clone* is specified.
- `i_key_str` - Defines *i_keyclass = cstr*, *i_tag = str*, and *i_keyraw = const char*\*. Defines type convertion 
*i_keyfrom/i_keyto*, and *i_cmp*, *i_eq*, *i_hash*, *i_keydrop* functions using *const char*\* as input.
- `i_key_ssv` - Defines *i_keyclass = cstr*, *i_tag = ssv*, and *i_keyraw = csview*. Defines type convertion 
*i_keyfrom/i_keyto* and *i_cmp*, *i_eq*, *i_hash*, *i_keydrop* functions using *csview* as input.
- `i_keyboxed TYPE` - Use when TYPE is a smart pointer **carc** or **cbox**. Defines *i_keyclass = TYPE*, and *i_keyraw = TYPE\**. NB: Do not use when defining carc/cbox types themselves.
- `i_valclass`, `i_val_str`, `i_val_ssv`, `i_valboxed` - Similar rules as for ***key***.

**Notes**:
- Instead of defining `i_cmp`, you may define *i_opt c_no_cmp* to disable *searching and sorting* functions.
- Instead of defining `i_*clone`, you may define *i_opt c_no_clone* to disable *clone* functionality.
- For `i_keyclass`, if *i_keyraw RAWTYPE* is defined along with it, *i_keyfrom* may also be defined to enable the *emplace*-functions. NB: the signature for ***cmp***, ***eq***, and ***hash*** uses *RAWTYPE* as input.

The *emplace* versus non-emplace container methods
--------------------------------------------------
STC, like c++ STL, has two sets of methods for adding elements to containers. One set begins
with **emplace**, e.g. *cvec_X_emplace_back()*. This is an ergonimic alternative to
*cvec_X_push_back()* when dealing non-trivial container elements, e.g. strings, shared pointers or
other elements using dynamic memory or shared resources.

The **emplace** methods ***constructs*** / ***clones*** the given element when they are added
to the container. In contrast, the *non-emplace* methods ***moves*** the element into the
container. 

**Note**: For containers with integral/trivial element types, or when neither `i_keyraw/i_valraw` is defined,
the **emplace** functions are ***not*** available (or needed), as it can easier lead to mistakes.

| non-emplace: Move          | emplace: Embedded copy         | Container                                   |
|:---------------------------|:-------------------------------|:--------------------------------------------|
| insert(), push()           | emplace()                      | cmap, csmap, cset, csset                    |
| insert_or_assign()         | emplace_or_assign()            | cmap, csmap                                 |
| push()                     | emplace()                      | cqueue, cpque, cstack                       |
| push_back(), push()        | emplace_back()                 | cdeq, clist, cvec                           |
| push_front()               | emplace_front()                | cdeq, clist                                 |

Strings are the most commonly used non-trivial data type. STC containers have proper pre-defined
definitions for cstr container elements, so they are fail-safe to use both with the **emplace**
and non-emplace methods:
```c
#define i_implement     // define in ONE file to implement longer functions in cstr
#include <stc/cstr.h>

#define i_val_str       // special macro to enable container of cstr
#include <stc/cvec.h>   // vector of string (cstr)
...
c_auto (cvec_str, vec)  // declare and call cvec_str_init() and defer cvec_str_drop(&vec)
c_with (cstr s = cstr_lit("a string literal"), cstr_drop(&s))
{
    const char* hello = "Hello";
    cvec_str_push_back(&vec, cstr_from(hello);    // construct and add string from const char*
    cvec_str_push_back(&vec, cstr_clone(s));      // clone and append a cstr

    cvec_str_emplace_back(&vec, "Yay, literal");  // internally constructs cstr from const char*
    cvec_str_emplace_back(&vec, cstr_clone(s));   // <-- COMPILE ERROR: expects const char*
    cvec_str_emplace_back(&vec, cstr_str(&s));    // Ok: const char* input type.
}
```
This is made possible because the type configuration may be given an optional
conversion/"rawvalue"-type as template parameter, along with a back and forth conversion
methods to the container value type.

Rawvalues are primarily beneficial for **lookup** and **map insertions**, however the
**emplace** methods constructs `cstr`-objects from the rawvalues, but only when required:
```c
cmap_str_emplace(&map, "Hello", "world");
// Two cstr-objects were constructed by emplace

cmap_str_emplace(&map, "Hello", "again");
// No cstr was constructed because "Hello" was already in the map.

cmap_str_emplace_or_assign(&map, "Hello", "there");
// Only cstr_lit("there") constructed. "world" was destructed and replaced.

cmap_str_insert(&map, cstr_lit("Hello"), cstr_lit("you"));
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

Forward declaring containers
----------------------------
It is possible to forward declare containers. This is useful when a container is part of a struct, 
but still not expose or include the full implementation / API of the container.
```c
// Header file
#include <stc/forward.h> // only include data structures
forward_cstack(cstack_pnt, struct Point); // declare cstack_pnt (and cstack_pnt_value, cstack_pnt_iter);
                                          // struct Point may be an incomplete type.
typedef struct Dataset {
    cstack_pnt vertices;
    cstack_pnt colors;
} Dataset;

...
// Implementation
#define c_opt c_is_forward                  // flag that the container was forward declared.
#define i_val struct Point
#define i_tag pnt
#include <stc/cstack.h>
```

User-defined container type name
--------------------------------
Define `i_type` instead of `i_tag`:
```c
#define i_type MyVec
#define i_val int
#include <stc/cvec.h>

myvec vec = MyVec_init();
MyVec_push_back(&vec, 1);
...
```

Memory efficiency
-----------------
STC is generally very memory efficient. Type sizes:
- **cstr**, **cvec**, **cstack**, **cpque**: 1 pointer, 2 intptr_t + memory for elements.
- **csview**, 1 pointer, 1 intptr_t. Does not own data!
- **cspan**, 1 pointer and 2 \* dimension \* int32_t. Does not own data!
- **clist**: Type size: 1 pointer. Each node allocates a struct to store its value and a next pointer.
- **cdeq**, **cqueue**:  Type size: 2 pointers, 2 intptr_t. Otherwise like *cvec*.
- **cmap/cset**: Type size: 2 pointers, 2 int32_t (default). *cmap* uses one table of keys+value, and one table of precomputed hash-value/used bucket, which occupies only one byte per bucket. The closed hashing has a default max load factor of 85%, and hash table scales by 1.5x when reaching that.
- **csmap/csset**: Type size: 1 pointer. *csmap* manages its own ***array of tree-nodes*** for allocation efficiency. Each node uses two 32-bit ints for child nodes, and one byte for `level`, but has ***no parent node***.
- **carc**: Type size: 1 pointer, 1 long for the reference counter + memory for the shared element.
- **cbox**: Type size: 1 pointer + memory for the pointed-to element.

# Version 4

## API changes summary V4.0
- Added **cregex** with documentation - powerful regular expressions.
- Added: `c_forfilter`: container iteration with "piped" filtering using && operator. 4 built-in filters.
- Added: `c_forwhile`: *c_foreach* container iteration with extra predicate.
- Added: **crange**: number generator type, which can be iterated (e.g. with *c_forfilter*).
- Added back **coption** - command line argument parsing.
- New + renamed loop iteration/scope macros:
    - `c_forlist`: macro replacing `c_forarray` and `c_apply`. Iterate a compound literal list.
    - `c_forrange`: macro replacing `c_forrange`. Iterate a `long long` type number sequence.
    - `c_with`: macro renamed from `c_autovar`. Like Python's **with** statement.
    - `c_scope`: macro renamed from `c_autoscope`.
    - `c_defer`: macro renamed from `c_autodefer`. Resembles Go's and Zig's **defer**.
- Updated **cstr**, now always takes self as pointer, like all containers except csview.
- Updated **cvec**, **cdeq**, changed `*_range*` function names.

## Changes version 3.8
- Overhauled some **cstr** and **csview** API:
    - Changed cstr_replace*() => `cstr_replace_at*(self, pos, len, repl)`: Replace at specific position.
    - Changed `cstr_replace_all() cstr_replace*(self, search, repl, count)`: Replace count occurences.
    - Renamed `cstr_find_from()` => `cstr_find_at()`
    - Renamed `cstr_*_u8()` => `cstr_u8_*()`
    - Renamed `csview_*_u8()` => `csview_u8_*()`
    - Added cstr_u8_slice() and csview_u8_slice().
    - Removed `csview_from_s()`: Use `cstr_sv(s)` instead.
    - Added back file coption.h
    - Simplified **cbits** usage: all inlined.
    - Updated docs.

## Changes version 3.7
- NB! Changed self argument from value to const pointer on containers (does not apply to **cstr**):
    - `CNT_size(const CNT *self)`
    - `CNT_capacity(const CNT *self)`
    - `CNT_empty(const CNT *self)`
- Now both **cstack** and **cbits** can be used with template `i_capacity` parameter: `#define i_capacity <NUM>`. They then use fixed sized arrays, and no heap allocated memory.
- Renamed *cstr_rename_n()* => *cstr_rename_with_n()* as it could be confused with replacing n instances instead of n bytes.
- Fixed bug in `csmap.h`: begin() on empty map was not fully initialized.

## Changes version 3.6
- Swapped to new **cstr** (*short string optimized*, aka SSO). Note that `cstr_str(&s)` must be used, `s.str` is no longer usable.
- Removed *redundant* size argument to `i_hash` template parameter and `c_default_hash`. Please update your code.
- Added general `i_keyclone/i_valclone` template parameter: containers of smart pointers (**carc**, **cbox**) now correctly cloned.
- Allows for `i_key*` template parameters instead of `i_val*` for all containers, not only for **cset** and **csset**.
- Optimized *c_default_hash()*. Therefore *c_hash32()* and *c_hash64()* are removed (same speed).
- Added *.._push()* and *.._emplace()* function to all containers to allow for more generic coding.
- Renamed global PRNGs *stc64_random()* and *stc64_srandom()* to *crandom()* and *csrandom()*.
- Added some examples and benchmarks for SSO and heterogenous lookup comparison with c++20 (string_bench_*.cpp).

## Brief summary of changes from version 2.x to 3.0
- Renamed: all ***_del*** to `_drop` (like destructors in Rust).
- Renamed: all ***_compare*** to `_cmp`
- Renamed: ***i_equ*** to `i_eq`, and ***_equalto*** to `_eq`.
- Renamed: ***i_cnt*** to `i_type` for defining the complete container type name.
- Renamed: type **csptr** to [**carc**](docs/carc_api.md) (atomic reference counted) smart pointer.
- Renamed: ***i_key_csptr*** / ***i_val_csptr*** to `i_keyboxed` / `i_valboxed` for specifying **carc** and **cbox** values in containers.
- Renamed: *csptr_X_make()* to `carc_X_from()`.
- Renamed: *cstr_new()* to `cstr_lit(literal)`, and *cstr_assign_fmt()* to `cstr_printf()`.
- Renamed: *c_default_fromraw()* to `c_default_from()`.
- Changed: the [**c_apply**](docs/ccommon_api.md) macros API.
- Replaced: *csview_first_token()* and *csview_next_token()* with one function: `csview_token()`.
- Added: **checkauto** tool for checking that c-source files uses `c_auto*` macros correctly.
- Added: general `i_keyclass` / `i_valclass` template parameters which auto-binds template functions.
- Added: `i_opt` template parameter: compile-time options: `c_no_cmp`, `c_no_clone`, `c_no_atomic`, `c_is_forward`; may be combined with `|`
- Added: [**cbox**](docs/cbox_api.md) type: smart pointer, similar to [Rust Box](https://doc.rust-lang.org/rust-by-example/std/box.html) and [std::unique_ptr](https://en.cppreference.com/w/cpp/memory/unique_ptr).
- Added: [**c_forpair**](docs/ccommon_api.md) macro: for-loop with "structured binding"
