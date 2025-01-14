![STC](docs/pics/containers.jpg)
---

# STC - Smart Template Containers

## Version 5.0.1
STC is a comprehensive, modern, typesafe and fast templated general purpose container and algorithms
library for C99. It aims to make C-programming even more fun, more productive and safer.

<details>
<summary><b>Version 5.0 NEWS</b></summary>

- New main build system with Meson. Simple Makefile provided as well.
- New **sum type** (tagged union), included via `algorithm.h`
- New single/multi-dimensional generic **span** type, with numpy-like slicing.
- Coroutines now support *structured concurrency* and *symmetric coroutines*.
- Coroutines now support *error handling* and *error recovery*.
- Template parameter `i_type` lets you define `i_type`, `i_key`, and `i_val` all in one line (comma separated).
- Template parameters `i_keyclass` and `i_valclass` to specify types with `_drop()` and `_clone()` functions defined.
- Template parameters `i_keypro` and `i_valpro` to specify `cstr`, `box` and `arc` types (users may also define pro-types).
- **hmap** now uses *Robin Hood hashing* (very fast on clang compiler).
- Several new algorithms added, e.g. `c_filter` (ranges-like).
- A lot of improvements and bug fixes.

See also [version history](#version-history) for breaking changes in V5.0.
</details>
<details>
<summary><b>Reasons why you want to you use STC</b></summary>
C is still among the most popular programming languages, despite the fact that it was created
as early as in 1972. That is a manifestation of how well the language was designed for its time,
and still is. However, times are changing, and C is starting to lag behind many of the new upcoming
system languages like Zig, Odin and Rust with regard to features in the standard library, but also
when it comes to safety and vulnerabilities. STC aims to bridge some of that gap, to let us have
common modern features and added safety, while we still can enjoy writing C.

#### A. Missing features in the C standard library, which STC provides
* A wide set of high performance, generic/templated typesafe container types, including smart pointers and bitsets.
* String type with utf8 support and short string optimization (sso), plus two string-view types.
* Typesafe and ergonomic **sum type** implementation, aka. tagged union or variant.
* A **coroutine** implementation with excellent ergonomics, error recovery and cleanup support.
* Fast, modern **regular expressions** with full utf8 and a subset of unicode character classes support.
* Ranges algorithms like *iota* and filter views like *take, skip, take-while, skip-while, map*.
* Generic algorithms, iterators and loop abstactions. Blazing fast *sort, binary search* and *lower bound*.
* Single/multi-dimensional generic **span view** with arbitrary array dimensions (numpy array-like slicing).

#### B. Improved safety by using STC
* Abstractions for raw loops, ranged iteration over containers, and generic ranges algorithms. All this
reduces the chance of creating bugs, as user code with raw loops and ad-hoc implementation of
common algorithms and containers is minimized/eliminated.
* STC is inherently **type safe**. Essentially, there are no opaque pointers or casting away of type information.
Only where neccesary, generic code will use some macros to do compile-time type-checking before types are casted.
Examples are `c_static_assert`, `c_const_cast`, `c_safe_cast` and macros for safe integer type casting.
* Containers and algorithms all use **signed integers** for indices and sizes, and it encourange to use
signed integers for quantities in general (unsigned integers have valid usages as bitsets and in bit operations).
This could remove a wide range of bugs related to mixed unsigned-signed calculations and comparisons, which
intuitively gives the wrong answer in many cases.
* Tagged unions in C are common, but normally unsafely implemented. Traditionally, it leaves the inactive payload
data readily accesible to user code, and there is no general way to ensure that the payload is assigned along with
the tag, or that they match. STC **sum type** is a typesafe version of tagged unions which eliminates all those
safety concerns.
</details>

Containers
----------
- [***arc*** - (atomic) reference counted shared pointer`](docs/arc_api.md)
- [***box*** - heap allocated unique pointer`](docs/box_api.md)
- [***cbits*** - dynamic bitset](docs/cbits_api.md)
- [***list*** - forward linked list](docs/list_api.md)
- [***stack*** - stack type](docs/stack_api.md)
- [***vec*** - vector type](docs/vec_api.md)
- [***deque*** - double-ended queue](docs/deque_api.md)
- [***queue*** - queue type](docs/queue_api.md)
- [***pqueue*** - priority queue](docs/pqueue_api.md)
- [***hmap*** - hashmap (unordered)](docs/hmap_api.md)
- [***hset*** - hashset (unordered)](docs/hset_api.md)
- [***smap*** - sorted binary tree map](docs/smap_api.md)
- [***sset*** - sorted binary tree set](docs/sset_api.md)
- [***cstr*** - string type (short string optimized)](docs/cstr_api.md)
- [***csview*** - string view (non-zero terminated)](docs/csview_api.md)
- [***zsview*** - zero-terminated string view](docs/zsview_api.md)
- [***cspan*** - single and multidimensional span (view)](docs/cspan_api.md)

Algorithms
----------
- [***Coroutines*** - ergonomic portable coroutines](docs/coroutine_api.md)
- [***Sum types*** - a.k.a. variants or tagged unions](docs/algorithm_api.md#sum-types)
- [***Generic algorithms***](docs/algorithm_api.md#stc-algorithms)
- [***Regular expressions*** - Rob Pike's Plan 9 regexp modernized!](docs/cregex_api.md)
- [***Random numbers*** - a very fast *PRNG* based on *SFC64*](docs/random_api.md)
- [***Command line argument parser*** - similar to *getopt()*](docs/coption_api.md)

## Contents

<details>
<summary>Highlights</summary>

## Highlights

- **Minimal boilerplate code** - Specify only the required template parameters, and leave the rest as defaults.
- **Fully type safe** - Because of templating, it avoids error-prone casting of container types and elements back and forth from the containers.
- **High performance** - Unordered maps and sets, queues and deques are significantly faster than the C++ STL containers, the remaining are similar or close to STL in speed (See graph below).
- **Fully memory managed** - Containers destructs keys/values via default or user supplied drop function. They may be cloned if element types are clonable. Smart pointers (shared and unique) works seamlessly when stored in containers. See [***arc***](docs/arc_api.md) and [***box***](docs/box_api.md).
- **Uniform, easy-to-learn API** - For the generic containers and algorithms, simply include the headers. The API and functionality resembles c++ STL or Rust and is fully listed in the docs. Uniform usage across the various containers.
- **No signed/unsigned mixing** - Unsigned sizes and indices mixed with signed for comparison and calculation is asking for trouble. STC only uses signed numbers in the API for this reason.
- **Small footprint** - Small source code and generated executables.
- **Dual mode compilation** - By default it is a header-only library with inline and static methods, but you can easily switch to create a shared library without changing existing source files. Non-generic types, like (utf8) strings are compiled with external linking. one See the [installation section](#installation).
- **No callback functions** - All passed template argument functions/macros are directly called from the implementation, no slow callbacks which requires storage.
- **Compiles with C++ and C99** - C code can be compiled with C++ (container element types must be POD).
- **Pre-declaration** - Templated containers may be [pre-declared](#pre-declarations) without including the full API/implementation.
- **Extendable containers** - STC provides a mechanism to wrap containers inside a struct with [custom data per instance](#per-container-instance-customization).

</details>
<details>
<summary>Installation</summary>

## Installation

STC uses meson build system. Make sure to have meson and ninja installed, e.g. as a python pip package from a bash shell:
```bash
pip install meson ninja
export LIBRARY_PATH=$LIBRARY_PATH:~/.local/lib
export CPATH=$CPATH:~/.local/include
export CC=gcc
```
To create a build folder and to set the install folder to e.g. ~/.local:
```bash
meson setup --buildtype debug build --prefix ~/.local
cd build
ninja
ninja install
```
STC is mixed *"headers-only"* / traditional library, i.e the templated container headers (and the *sort*/*lower_bound*
algorithms) can simply be included - they have no library dependencies. By default, all templated functions are
static (many inlined). This is often optimal for both performance and compiled binary size. However, for frequently
used container type instances (more than 2-3 TUs), consider creating a separate header file for them, e.g.:
```c++
// intvec.h
#ifndef INTVEC_H_
#define INTVEC_H_
#define i_header // header definitions only
#define i_type intvec, int
#include "stc/vec.h"
#endif
```
So anyone may use the shared vec-type. Implement the shared functions in one C file (if several containers are shared,
you may define STC_IMPLEMENT on top of the file once instead):
```c++
// shared.c
#define i_implement // implement the shared intvec.
#include "intvec.h"
```
The non-templated types  **cstr**, **csview**, **cregex**, **cspan** and **random**, are built as a library (libstc),
and is using the ***meson*** build system. However, the most common functions in **csview** and **random** are inlined.
The bitset **cbits**, the zero-terminated string view **zsview** and **algorthm** are all fully inlined and need no
linking with the stc-library.
</details>
<details>
<summary>Usage</summary>

## Usage
STC containers have similar functionality to the C++ STL standard containers. All containers except for a few,
like **cstr** and **cbits** are generic/templated. No type casting is used, so containers are type-safe like
templated types in C++. To specify template parameters with STC, you define them as macros prior to
including the container, e.g.
```c++
#define i_type Floats, float // Container type (name, element type)
#include "stc/vec.h"         // "instantiate" the desired container type
#include <stdio.h>

int main(void)
{
    Floats nums = {0};
    Floats_push(&nums, 30.f);
    Floats_push(&nums, 10.f);
    Floats_push(&nums, 20.f);

    for (int i = 0; i < Floats_size(&nums); ++i)
        printf(" %g", nums.data[i]);

    for (c_each(i, Floats, nums))     // Alternative and recommended way to iterate.
        printf(" %g", *i.ref);      // i.ref is a pointer to the current element.

    Floats_drop(&nums); // cleanup memory
}
```
Switching to a different container type, e.g. a sorted set (sset):
<!-- https://raw.githubusercontent.com/stclib/stcsingle/main/ -->
[ [Run this code](https://godbolt.org/z/6KhzdMafd) ]
```c++
#define i_type Floats, float
#include "stc/sset.h" // Use a sorted set instead
#include <stdio.h>

int main(void)
{
    Floats nums = {0};
    Floats_push(&nums, 30.f);
    Floats_push(&nums, 10.f);
    Floats_push(&nums, 20.f);

    // print the numbers (sorted)
    for (c_each(i, Floats, nums))
        printf(" %g", *i.ref);

    Floats_drop(&nums);
}
```
For associative containers and priority queues (hmap, hset, smap, sset, pqueue), comparison/lookup functions
are assumed to be defined. I.e. if they are not specified with template parameters, it assumes default
comparison operators works. To enable search/sort for the remaining containers (stack, vec, queue, deque),
define `i_cmp` or `i_eq` and/or `i_less` for the element type. If the element type is an integral type,
just define `i_use_cmp` (will use  `==` and `<` operators for comparisons).

If an element destructor `i_keydrop` is defined, `i_keyclone` function is required.
*Alternatively `#define i_opt c_no_clone` to disable container cloning.*

Let's make a vector of vectors, which can be cloned. All of its element vectors will be destroyed when destroying the Vec2D.

[ [Run this code](https://godbolt.org/z/36PWz51sn) ]
```c++
#include <stdio.h>
#include "stc/algorithm.h"

#define i_type Vec, float
#define i_use_cmp        // enable default ==, < and hash operations
#include "stc/vec.h"

#define i_type Vec2D
#define i_keyclass Vec   // Use i_keyclass when key type has "members" _clone() and _drop().
#define i_use_eq         // vec does not have _cmp(), but it has _eq()
#include "stc/vec.h"

int main(void)
{
    Vec* v;
    Vec2D vec_a = {0};                  // All containers in STC can be initialized with {0}.
    v = Vec2D_push(&vec_a, Vec_init()); // push() returns a pointer to the new element in vec.
    Vec_push(v, 10.f);
    Vec_push(v, 20.f);

    v = Vec2D_push(&vec_a, Vec_init());
    Vec_push(v, 30.f);
    Vec_push(v, 40.f);

    Vec2D vec_b = c_make(Vec2D, {
        c_make(Vec, {10.f, 20.f}),
        c_make(Vec, {30.f, 40.f}),
    });
    printf("vec_a == vec_b is %s.\n", Vec2D_eq(&vec_a, &vec_b) ? "true":"false");

    Vec2D clone = Vec2D_clone(vec_a);   // Make a deep-copy of vec

    for (c_each(i, Vec2D, clone))         // Loop through the cloned vector
        for (c_each(j, Vec, *i.ref))
            printf(" %g", *j.ref);

    c_drop(Vec2D, &vec_a, &vec_b, &clone);  // Free all 9 vectors.
}
```
This example uses four different container types:

[ [Run this code](https://godbolt.org/z/3h7WaxoGb) ]
<!--{%raw%}-->
```c++
#include <stdio.h>

#define i_type hset_int, int
#include "stc/hset.h"   // unordered/hash set (assume i_key is basic type, uses `==` operator)

struct Point { float x, y; };
// Define cvec_pnt and enable linear search by defining i_eq
#define i_type vec_pnt, struct Point
#define i_eq(a, b) (a->x == b->x && a->y == b->y)
#include "stc/vec.h"    // vector of struct Point

#define i_type list_int, int
#define i_use_cmp       // enable sort/search. Use native `<` and `==` operators
#include "stc/list.h"   // singly linked list

#define i_type smap_int, int, int
#include "stc/smap.h"  // sorted map int => int

int main(void)
{
    // Define four empty containers
    hset_int set = {0};
    vec_pnt vec = {0};
    list_int lst = {0};
    smap_int map = {0};
    c_defer( // Drop the containers at scope exit
        hset_int_drop(&set),
        vec_pnt_drop(&vec),
        list_int_drop(&lst),
        smap_int_drop(&map)
    ){
        enum{N = 5};
        int nums[N] = {10, 20, 30, 40, 50};
        struct Point pts[N] = {{10, 1}, {20, 2}, {30, 3}, {40, 4}, {50, 5}};
        int pairs[N][2] = {{20, 2}, {10, 1}, {30, 3}, {40, 4}, {50, 5}};

        // Add some elements to each container
        for (int i = 0; i < N; ++i) {
            hset_int_insert(&set, nums[i]);
            vec_pnt_push(&vec, pts[i]);
            list_int_push_back(&lst, nums[i]);
            smap_int_insert(&map, pairs[i][0], pairs[i][1]);
        }

        // Find an element in each container
        hset_int_iter i1 = hset_int_find(&set, 20);
        vec_pnt_iter i2 = vec_pnt_find(&vec, (struct Point){20, 2});
        list_int_iter i3 = list_int_find(&lst, 20);
        smap_int_iter i4 = smap_int_find(&map, 20);

        printf("\nFound: %d, (%g, %g), %d, [%d: %d]\n",
                *i1.ref, i2.ref->x, i2.ref->y, *i3.ref,
                i4.ref->first, i4.ref->second);

        // Erase all the elements found
        hset_int_erase_at(&set, i1);
        vec_pnt_erase_at(&vec, i2);
        list_int_erase_at(&lst, i3);
        smap_int_erase_at(&map, i4);

        printf("After erasing the elements found:");
        printf("\n set:");
        for (c_each(i, hset_int, set))
            printf(" %d", *i.ref);

        printf("\n vec:");
        for (c_each(i, vec_pnt, vec))
            printf(" (%g, %g)", i.ref->x, i.ref->y);

        printf("\n lst:");
        for (c_each(i, list_int, lst))
            printf(" %d", *i.ref);

        printf("\n map:");
        for (c_each(i, smap_int, map))
            printf(" [%d: %d]", i.ref->first, i.ref->second);
    }
}
```
<!--{%endraw%}-->
</details>
<details>
<summary>Performance</summary>

## Performance

STC is a fast and memory efficient library, and code compiles fast:

![Benchmark](docs/pics/Figure_1.png)

Benchmark notes:
- The barchart shows average test times over three compilers: **Mingw64 13.1.0, Win-Clang 16.0.5, VC-19-36**. CPU: **Ryzen 7 5700X**.
- Containers uses value types `uint64_t` and pairs of `uint64_t` for the maps.
- Black bars indicates performance variation between various platforms/compilers.
- Iterations and access are repeated 4 times over n elements.
- access: no entryfor *forward_list*, *deque*, and *vector* because these c++ containers does not have native *find()*.
- **deque**: *insert*: n/3 push_front(), n/3 push_back()+pop_front(), n/3 push_back().
- **map and unordered map**: *insert*: n/2 random numbers, n/2 sequential numbers. *erase*: n/2 keys in the map, n/2 random keys.
</details>
<details>
<summary>Some unique features of STC</summary>

## Some unique features of STC

1. ***Centralized analysis of template parameters***. The analyser assigns values to all
non-specified template parameters using meta-programming. You may specify a set of "standard"
template parameters for each container, but as a minimum *only one is required*: `i_type` or
`i_key` (+ `i_val` for maps). In this case, STC assumes that the elements are of basic types.
For non-trivial types, additional template parameters must be given.
2. ***Alternative lookup and insert type***. Specify an alternative type to use for
lookup in containers. E.g., containers with STC string elements (**cstr**) uses `const char*`
as lookup type. Therefore it is not needed to construct (or destroy) a `cstr` in order
to lookup a **cstr** object. Also, one may pass a c-string literal to one of the
***emplace***-functions to implicitly insert a cstr object, i.e. `vec_cstr_emplace(&vec, "Hello")`
as an alternative to `vec_cstr_push(&vec, cstr_from("Hello"))`.
3. ***Standardized container iterators***. All containers can be iterated in the same manner, and all use the
same element access syntax. The following works for single-element type containers, e.g a linked list:
```c++
#define i_type MyInts, int
#include "stc/list.h"
...
MyInts ints = c_make(MyInts, {3, 5, 9, 7, 2});
for (c_each(it, MyInts, ints)) *it.ref += 42;
```
</details>
<details>
<summary>Naming rules</summary>

## Naming rules

- Naming conventions
    - Non-templated container names are prefixed by `c`, e.g. `cstr`, `cbits`, `cregex`.
    - Public STC macros and "keywords" are prefixed by `c_`, e.g. `c_each`, `c_make`.
    - Template parameter macros are prefixed by `i_`, e.g. `i_key`, `i_type`.
    - All owning containers can be initialized with `{0}` (also `cstr`), i.e. no heap allocation initially.

- Common types for any container type Cont:
    - Cont
    - Cont_value
    - Cont_raw
    - Cont_iter

- Functions available for most all containers:
    - Cont_init()
    - Cont_with_n(rawvals[], n)
    - Cont_reserve(Cont*, capacity)
    - Cont_clone(Cont)
    - Cont_drop(Cont*)
    - Cont_size(Cont*)
    - Cont_is_empty(Cont*)
    - Cont_push(Cont*, value)
    - Cont_put_n(Cont*, rawvals[], n)
    - Cont_erase_at(Cont*, Cont_iter)
    - Cont_front(Cont*)
    - Cont_back(Cont*)
    - Cont_begin(Cont*)
    - Cont_end(Cont*)
    - Cont_next(Cont_iter*)
    - Cont_advance(Cont_iter, n)
</details>
<details>
<summary>Defining template parameters</summary>

## Defining template parameters

The container template parameters are specified with a `#define i_xxxx` statement. Only `i_key` is
strictly required. Each templated type instantiation requires an `#include` statement, even if the
same container base type was included earlier. Possible template parameters are:

### Basic template parameters
- `i_type` *ContType*, *KeyType*[, *ValType*] is a shorthand for defining ***i_type***, ***i_key*** (and ***i_val***)
 individually, as described next.
- `i_type` *ContType* - Custom container type name.
- `i_key` *KeyType* - Element type. **[required]**.
- `i_val` *MappedType* - Element type. **[required for]** hmap and smap containers.
- `i_cmp` *Func* - Three-way comparison of two *i_keyraw*\*
- `i_less` *Func* - Comparison of two *i_keyraw*\* - an alternative to specifying ***i_cmp***.
- `i_eq` *Func* - Equality comparison of two *i_keyraw*\* - defaults to *!i_cmp*. Companion with *i_hash*.
- `i_hash` *Func* - Hash function taking *i_keyraw*\* - defaults to *c_default_hash*. **[required for]**
***hmap/hset*** with non-POD *i_keyraw* elements.

#### Key (element lookup type):
- `i_keydrop` *Func* - Destroy key - defaults to empty destructor.
- `i_keyclone` *Func* - **[required if]** *i_keydrop* is defined (exception for **arc**, as it shares).
- Advanced, convertion between an alternative input type:
    - `i_keyraw` *Type* - Lookup and emplace "raw" type, defaults to *i_key*.
    - `i_keyfrom` *Func* - Convertion func from *i_keyraw* to *i_key*.
    - `i_keytoraw` *Func*  - Convertion func to *i_keyraw* from *i_key*. **[required if]** *i_keyraw* is defined

#### Val (mapped value type for maps):
- These are analogues to the Key parameters, i.e. `i_valdrop`, `i_valclone`, `i_valraw`, etc.

---

### Meta template parameters
The following meta-template parameters can be specified instead of ***i_key***, ***i_val***, and ***i_keyraw***.
These parameters make types into "classes" in the sense that they bind associated function names to the basic
template parameters described above. This reduces boiler-plate code and simplifies the management
of non-trivial container elements. Note that many basic template parameters will be defined when defining the
following parameters, but the user may override those when needed. E.g. by defining the template parameters
directly as macro functions or with macros that refer to the C function names.

#### Key meta parameters:
- `i_rawclass` *RawType* - Defines ***i_keyraw*** and binds *RawType_cmp()*, *RawType_eq()*, *RawType_hash()*
    to ***i_cmp***, ***i_eq***, and ***i_hash***.
    - If neither ***i_key*** nor ***i_keyclass*** are defined, ***i_key*** is defined as *RawType*.
    - Useful for containers of views (like csview).
- `i_keyclass` *KeyType* - Defines ***i_key*** and binds standard named functions *KeyType_clone()* and
*KeyType_drop()* to ***i_keyclone*** / ***i_keydrop***. If `i_keyraw` is also specified, *KeyType_from()*
and *KeyType_toraw()* are bound to ***i_keyfrom*** / ***i_keytoraw***.
    - Use with container of containers, or in general when the element type has *_clone()* and *_drop()* "member" functions.
- `i_keypro` *KeyType* - Use with "pro"-element types, i.e. library types like **cstr**, **box** and **arc**.
It combines all the ***i_keyclass*** and ***i_rawclass*** properties. Defining ***i_keypro*** is like defining
    - ***i_rawclass*** *KeyType*_***raw***.
    - ***i_keyclass*** *KeyType*
    - I.e. `i_key`, `i_keyclone`, `i_keydrop`, `i_keyraw`, `i_keyfrom`, `i_keytoraw`, `i_cmp`, `i_eq`, `i_hash` will all be defined/bound.

#### Val (mapped) meta parameters:
- `i_valclass` *MappedType* - Analogous to the ***i_keyclass*** parameter.
- `i_valpro` *MappedType* - Comparison functions are not relevant for the mapped type, so this defines
    - ***i_valraw*** *MappedType*_***raw***
    - ***i_valclass*** *MappedType*
    - I.e. `i_val`, `i_valclone`, `i_valdrop`, `i_valraw`, `i_valfrom`, `i_valtoraw` will all be defined/bound.

Option flags:
- `i_opt` *Flags* - Boolean properties: may combine *c_no_clone*, *c_no_atomic*, *c_declared*, *c_static*,
*c_header* with the `|` separator.

**Notes**:
- Define `i_no_clone` or `i_opt c_no_clone | c_... | ...` to disable *clone* functionality.
- If ***i_keyraw*** and ***i_keyfrom*** are defined, the *emplace*-functions are enabled. The *_cmp()*, *_less()*,
*_eq()*, and *_hash()* functions takes pointers to parameter type ***i_keyraw***.
- Specify `i_has_cmp` instead of the comparison parameters to enable searching / sorting for integral
***i_keyraw*** types, or when comparison functions are implicitly bound via meta-template parameters.


</details>
<details>
<summary>Specifying comparison parameters</summary>

## Specifying comparison parameters

The table below shows the template parameters which *must* be defined to support element search/lookup and sort for various container type instantiations.

For the containers marked ***optional***, the features are disabled if the template parameter(s) are not defined. Note that the ***(integral type)*** columns also applies to "special" key-types, specified with `i_keyclass` (so not only for true integral types like `int` or `float`).

| Container         | search (integral type) | sort (integral type) |\|| search (struct elem) | sort (struct elem) | optional |
|:------------------|:---------------------|:---------------------|:-|:-----------------|:-------------------|:---------|
| vec, deque, list  | `i_use_cmp`          | `i_use_cmp`          || `i_eq`             | `i_cmp` / `i_less` | yes      |
| stack             | n/a                  | `i_use_cmp`          || n/a                | `i_cmp` / `i_less` | yes      |
| box, arc          | `i_use_cmp`          | `i_use_cmp`          || `i_eq` + `i_hash`  | `i_cmp` / `i_less` | yes      |
| hmap, hset        |                      | n/a                  || `i_eq` + `i_hash`  | n/a                | no       |
| smap, sset        |                      |                      || `i_cmp` / `i_less` | `i_cmp` / `i_less` | no       |
| pqueue            | n/a                  |                      || n/a                | `i_cmp` / `i_less` | no       |
| queue             | n/a                  | n/a                  || n/a                | n/a                | n/a      |

</details>
<details>
<summary>The <b>emplace</b> methods</summary>

## The *emplace* methods

STC, like c++ STL, has two sets of methods for adding elements to containers. One set begins
with **emplace**, e.g. *vec_X_emplace_back()*. This is an ergonimic alternative to
*vec_X_push_back()* when dealing non-trivial container elements, e.g. strings, shared pointers or
other elements using dynamic memory or shared resources.

The **emplace** methods ***constructs*** / ***clones*** the given element when they are added
to the container. In contrast, the *non-emplace* methods ***moves*** the element into the
container.

**Note**: For containers with integral/trivial element types, or when neither `i_keyraw/i_valraw` is defined,
the **emplace** functions are ***not*** available (or needed), as it can easier lead to mistakes.

| non-emplace: Move          | emplace: Embedded copy         | Container                   |
|:---------------------------|:-------------------------------|:----------------------------|
| insert(), push()           | emplace()                      | hmap, smap, hset, sset      |
| insert_or_assign()         | emplace_or_assign()            | hmap, smap                  |
| push()                     | emplace()                      | queue, pqueue, stack        |
| push_back(), push()        | emplace_back()                 | deque, list, vec            |
| push_front()               | emplace_front()                | deque, list                 |

Strings are the most commonly used non-trivial data type. STC containers have proper pre-defined
definitions for cstr container elements, so they are fail-safe to use both with the **emplace**
and non-emplace methods:
```c++
#include "stc/cstr.h"

#define i_keypro cstr  // use i_keypro for "pro" types like cstr, arc, box
#include "stc/vec.h"   // vector of string (cstr)
...
vec_cstr vec = {0};
cstr s = cstr_lit("a string literal");
const char* hello = "Hello";

vec_cstr_push(&vec, cstr_from(hello);    // make a cstr from const char* and move it onto vec
vec_cstr_push(&vec, cstr_clone(s));      // make a cstr clone and move it onto vec

vec_cstr_emplace(&vec, "Yay, literal");  // internally make a cstr from const char*
vec_cstr_emplace(&vec, cstr_clone(s));   // <-- COMPILE ERROR: expects const char*
vec_cstr_emplace(&vec, cstr_str(&s));    // Ok: const char* input type.

cstr_drop(&s)
vec_cstr_drop(&vec);
```
This is made possible because the type configuration may be given an optional
conversion/"rawvalue"-type as template parameter, along with a back and forth conversion
methods to the container value type.

Rawvalues are primarily beneficial for **lookup** and **map insertions**, however the
**emplace** methods constructs `cstr`-objects from the rawvalues, but only when required:
```c++
hmap_cstr_emplace(&map, "Hello", "world");
// Two cstr-objects were constructed by emplace

hmap_cstr_emplace(&map, "Hello", "again");
// No cstr was constructed because "Hello" was already in the map.

hmap_cstr_emplace_or_assign(&map, "Hello", "there");
// Only cstr_lit("there") constructed. "world" was destructed and replaced.

hmap_cstr_insert(&map, cstr_lit("Hello"), cstr_lit("you"));
// Two cstr's constructed outside call, but both destructed by insert
// because "Hello" existed. No mem-leak but less efficient.

it = hmap_cstr_find(&map, "Hello");
// No cstr constructed for lookup, although keys are cstr-type.
```
Apart from strings, maps and sets are normally used with trivial value types. However, the
last example on the **hmap** page demonstrates how to specify a map with non-trivial keys.
</details>
<details>
<summary>The <b>erase</b> methods</summary>

## The *erase* methods

| Name                      | Description                  | Container                                |
|:--------------------------|:-----------------------------|:-----------------------------------------|
| erase()                   | key based                    | smap, sset, hmap, hset, cstr             |
| erase_at()                | iterator based               | smap, sset, hmap, hset, vec, deque, list |
| erase_range()             | iterator based               | smap, sset, vec, deque, list             |
| erase_n()                 | index based                  | vec, deque, cstr                         |
| remove()                  | remove all matching values   | list                                     |
</details>
<details>
<summary>User-defined container type name</summary>

## User-defined container type name

Define `i_type` and/or `i_key`:
```c++
// #define i_type MyVec, int // shorthand
#define i_type MyVec
#define i_key int
#include "stc/vec.h"

MyVec vec = {0};
MyVec_push(&vec, 42);
...
```
</details>
<details>
<summary>Pre-declarations</summary>

## Pre-declarations
Pre-declare templated container in header file. The container can then e.g. be a "private"
member of a struct defined in a header file.

```c++
// Dataset.h
#ifndef Dataset_H_
#define Dataset_H_
#include "stc/types.h"   // include various container data structure templates

// declare PointVec as a vec. Also struct Point may be incomplete/undeclared.
declare_vec(PointVec, struct Point);

typedef struct Dataset {
    PointVec vertices;
    PointVec colors;
} Dataset;

void Dataset_drop(Dataset* self);
...
#endif
```

Define and use the "private" container in the c-file:
```c++
// Dataset.c
#include "Dataset.h"
#include "Point.h"      // struct Point must be defined here.

#define i_type PointVec, struct Point
#define i_declared      // must flag that the container was pre-declared.
#include "stc/vec.h"    // Implements PointVec with static linking by default
...
```
</details>
<details>
<summary>Per container-instance customization</summary>

## Per container-instance customization
Sometimes it is useful to extend a container type to store extra data, e.g. a comparison
or allocator function pointer or a context which the function pointers can use. Most
libraries solve this by adding an opaque pointer (void*) or function pointer(s) into
the data structure for the user to manage. Because most containers are templated,
an extra template parameter, `i_aux` may be defined to extend the container with
typesafe custom attributes.

The example below shows how to customize containers to work with PostgreSQL memory management.
It adds a MemoryContext to each container by defining the `i_aux` template parameter.
Note that `pgs_realloc` and `pgs_free` is also passed the
allocated size of the given pointer, unlike standard `realloc` and `free`.

`i_aux` is accessible for customizing the following containers using template parameters:
- `i_malloc`, `i_calloc`, `i_realloc`, `i_free`: **all containers**
- `i_cmp`: **smap** and **sset**
- `i_hash`, `i_eq`: **hmap** and **hset**
- `i_eq`: **vec**, **deque**, **list**
- `i_less`: **pqueue**

```c++
// stcpgs.h
#define pgs_malloc(sz) MemoryContextAlloc(self->aux.memctx, sz)
#define pgs_calloc(n, sz) MemoryContextAllocZero(self->aux.memctx, (n)*(sz))
#define pgs_realloc(p, old_sz, sz) (p ? repalloc(p, sz) : pgs_malloc(sz))
#define pgs_free(p, sz) (p ? pfree(p) : (void)0) // pfree/repalloc does not accept NULL.

#define i_aux MemoryContext memctx;
#define i_allocator pgs
#define i_no_clone
```
Usage is straight forward:
```c++
#define i_type IMap, int, int
#include "stcpgs.h"
#include "stc/smap.h"

void maptest()
{
    IMap map = {.aux={CurrentMemoryContext}};
    for (c_range(i, 1, 16))
        IMap_insert(&map, i*i, i); // uses pgs_malloc

    for (c_each(i, IMap, map))
        printf("%d:%d ", i.ref->first, i.ref->second);

    IMap_drop(&map);
}
```
</details>
<details>
<summary>Memory efficiency</summary>

## Memory efficiency

STC is generally very memory efficient. Memory usage for the different containers:
- **cstr**, **vec**, **stack**, **pqueue**: 1 pointer, 2 isize + memory for elements.
- **csview**, 1 pointer, 1 isize. Does not own data!
- **cspan**, 1 pointer and 2 \* dimension \* int32_t. Does not own data!
- **list**: Type size: 1 pointer. Each node allocates a struct to store its value and a next pointer.
- **deque**, **queue**:  Type size: 2 pointers, 2 isize. Otherwise like *vec*.
- **hmap/hset**: Type size: 2 pointers, 2 int32_t (default). *hmap* uses one table of keys+value, and one table of precomputed hash-value/used bucket, which occupies only one byte per bucket. The closed hashing has a default max load factor of 85%, and hash table scales by 1.5x when reaching that.
- **smap/sset**: Type size: 1 pointer. *smap* manages its own ***array of tree-nodes*** for allocation efficiency. Each node uses two 32-bit ints for child nodes, and one byte for `level`, but has ***no parent node***.
- **arc**: Type size: 1 pointer, 1 long for the reference counter + memory for the shared element.
- **box**: Type size: 1 pointer + memory for the pointed-to element.
</details>

<details>
<summary>Version history</summary>

## Version history

## Version 5.0 changes
- This is a major new version, with serveral breaking changes compared to 4.3
    - Some API changes in `cregex`.
    - Some API changes in `cstr` and `csview`.
    - Renamed czsview type to `zsview`, some API changes.
    - Renamed all member Container_empty() functions to `Container_is_empty()`.
    - Changed API in `random` numbers.
    - c_init renamed to `c_make`
    - c_forlist renamed to `c_foritems`
    - c_forpair *replaced by* `c_each_kv` (changed API).
    - Renamed all functions stc_\<xxxx\>() to `c_<xxxx>()` in common.h.
    - c_SVFMT(sv) renamed tp `c_svfmt(sv)`
    - c_SVARG(sv) renamed tp `c_svarg(sv)`
    - Renamed coroutine cco_yield() to "keyword" `cco_yield`.
    - Swapped 2nd and 3rd argument in `c_fortoken()` to make it consistent with all other `c_for*()`, i.e, input object is third last.
    - New header `vec.h` renamed from cvec.h
    - New header `deque.h` renamed from cdeq.h
    - New header `list.h` renamed from clist.h
    - New header `stack.h` renamed from cstack.h
    - New header `queue.h` renamed from cqueue.h
    - New header `pqueue.h` renamed from cpque.h
    - New header `hmap.h` renamed from cmap.h
    - New header `hset.h` renamed from cset.h
    - New header `smap.h` renamed from csmap.h
    - New header `sset.h` renamed from csset.h
    - New header `zsview.h` renamed from czview.h
    - New header `random.h` renamed from crand.h
    - New header `types.h` renamed from forward.h

## Version 4.3
- Breaking changes:
    - **cstr** and **csview** now uses *shared linking* by default. Implement by either defining `i_implement` or `i_static` before including.
    - Renamed "stc/calgo.h" => `"stc/algorithm.h"`
    - Moved "stc/algo/coroutine.h" => `"stc/coroutine.h"`
        - Much improved with some new API and added features.
    - Removed deprecated "stc/crandom.h". Use `"stc/random.h"` with the new API.
        - Reverted names _unif and _norm back to `_uniform` and `_normal`.
    - Removed default comparison for **list**, **vec** and **deque**:
        - Define `i_use_cmp` to enable comparison for built-in i_key types (<, ==).
        - Use of `i_keyclass` still expects comparison functions to be defined.
    - Renamed input enum flags for ***cregex***-functions.
- **cspan**: Added **column-major** order (fortran) multidimensional spans and transposed views (changed representation of strides).
- All new faster and smaller **queue** and **deque** implementations, using a circular buffer.
- Renamed i_extern => `i_import` (i_extern deprecated).
    - Define `i_import` before `#include "stc/cstr.h"` will also define full utf8 case conversions.
    - Define `i_import` before `#include "stc/cregex.h"` will also define cstr + utf8 tables.
- Renamed c_make() => ***c_make()*** macro for initializing containers with element lists. c_make deprecated.
- Removed deprecated uppercase flow-control macro names.
- Other smaller additions, bug fixes and improved documentation.

## Version 4.2
- New home! And online single headers for https://godbolt.org
    - Library: https://github.com/stclib/STC
    - Headers, e.g. https://raw.githubusercontent.com/stclib/stcsingle/main/stc/vec.h
- Much improved documentation
- Added Coroutines + documentation
- Added new random.h API & header. Old crandom.h is deprecated.
- Added `c_const_cast()` typesafe macro.
- Removed RAII macros usage from examples
- Renamed c_flt_count(i) => `c_flt_counter(i)`
- Renamed c_flt_last(i) => `c_flt_getcount(i)`
- Renamed c_ARRAYLEN() => c_arraylen()
- Removed deprecated c_ARGSV(). Use c_svarg()
- Removed c_PAIR

## Version 4.1.1
Major changes:
- A new exciting [**cspan**](docs/cspan_api.md) single/multi-dimensional array view (with numpy-like slicing).
- Signed sizes and indices for all containers. See C++ Core Guidelines by Stroustrup/Sutter: [ES.100](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#es100-dont-mix-signed-and-unsigned-arithmetic), [ES.102](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#es102-use-signed-types-for-arithmetic), [ES.106](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#es106-dont-try-to-avoid-negative-values-by-using-unsigned), and [ES.107](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#es107-dont-use-unsigned-for-subscripts-prefer-gslindex).
- Customizable allocator [per templated container type](https://github.com/tylov/STC/discussions/44#discussioncomment-4891925).
- Updates on **cregex** with several [new unicode character classes](docs/cregex_api.md#regex-cheatsheet).
- Algorithms:
    - [crange](docs/algorithm_api.md#crange) - similar to [boost::irange](https://www.boost.org/doc/libs/release/libs/range/doc/html/range/reference/ranges/irange.html) integer range generator.
    - [c_forfilter](docs/algorithm_api.md#c_forfilter) - ranges-like view filtering.
    - [quicksort](include/stc/sort.h) - fast quicksort with custom inline comparison.
- Renamed `c_ARGSV()` => `c_svarg()`: **csview** print arg. Note `c_sv()` is shorthand for *csview_from()*.
- Support for [uppercase flow-control](include/stc/priv/altnames.h) macro names in common.h.
- Some API changes in **cregex** and **cstr**.
- Create single header container versions with python script.


## API changes summary V4.0
- Added **cregex** with documentation - powerful regular expressions.
- Added: `c_forfilter`: container iteration with "piped" filtering using && operator. 4 built-in filters.
- Added: **crange**: number generator type, which can be iterated (e.g. with *c_forfilter*).
- Added back **coption** - command line argument parsing.
- New + renamed loop iteration/scope macros:
    - `c_foritems`: macro replacing *c_forarray* and *c_apply*. Iterate a compound literal list.
- Updated **cstr**, now always takes self as pointer, like all containers except csview.
- Updated **vec**, **deque**, changed `*_range*` function names.

</details>

---
