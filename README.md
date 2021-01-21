![Standard Template Containers](docs/pics/containers.jpg)

STC - Standard Template Containers
==================================

Introduction
------------

A modern, templated, user-friendly, fast, fully typesafe, and customizable container library for C99,
with a uniform API, which resembles the c++ standard template library (STL).

This is a compact, header-only library with the all the major "standard" data containers, except for the multi-map/set variants:
- [***carray*** - Templated **multi-dimensional array** type](docs/carray_api.md)
- [***cbits*** - A **std::bitset** / **boost::dynamic_bitset** alike type](docs/cbits_api.md)
- [***cdeq*** - Templated **std::deque** alike type](docs/cdeq_api.md)
- [***clist*** - Templated **std::forward_list** alike type](docs/clist_api.md)
- [***cmap*** - Templated **std::unordered_map** alike type](docs/cmap_api.md)
- [***cpque*** - Templated **std::priority_queue** alike adapter type](docs/cpque_api.md)
- [***cptr*** - Container pointers and **std::shared_ptr** alike support](docs/cptr_api.md)
- [***cqueue*** - Templated **std::queue** alike adapter type](docs/cqueue_api.md)
- [***cset*** - Templated **std::unordered_set** alike type](docs/cset_api.md)
- [***csmap*** - Templated sorted map **std::map** alike type](docs/csmap_api.md)
- [***csset*** - Templated sorted set **std::set** alike type](docs/csset_api.md)
- [***cstack*** - Templated **std::stack** alike adapter type](docs/cstack_api.md)
- [***cstr*** - A **std::string** alike type](docs/cstr_api.md)
- [***cvec*** - Templated **std::vector** alike type](docs/cvec_api.md)

Others:
- [***ccommon*** - General definitions and handy, safe-to-use macros](docs/ccommon_api.md)
- [***coption*** - Implements ***coption_get()***, similar to posix **getopt_long()**](docs/coption_api.md)
- [***crandom*** - A novel, extremely fast *PRNG* named **stc64**](docs/crandom_api.md)

The usage of the containers is similar to the c++ standard containers in STL, so it should be easy if you are familiar with them. 
All containers are generic/templated, except for **cstr** and **cbits**. No casting is used, so containers are typesafe like
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
And with multiple containers...
```c
#include <stc/cset.h>
#include <stc/cvec.h>
#include <stc/cdeq.h>
#include <stc/clist.h>
#include <stc/cqueue.h>
#include <stc/csmap.h>
#include <stdio.h>

struct Point { float x, y; };

// declare your container types
using_cset(i, int);                         // unordered set
using_cvec(p, struct Point, c_no_compare);  // vector, struct as elements
using_cdeq(i, int);                         // deque
using_clist(i, int);                        // singly linked list
using_cqueue(i, cdeq_i);                    // queue, using deque as adapter
using_csmap(i, int, int);                   // sorted map

int main(void) {
    // define and initialize
    c_init (cset_i, set, {10, 20, 30});
    c_init (cvec_p, vec, {{10, 1}, {20, 2}, {30, 3}});
    c_init (cdeq_i, deq, {10, 20, 30});
    c_init (clist_i, lst, {10, 20, 30});
    c_init (cqueue_i, que, {10, 20, 30});
    c_init (csmap_i, map, {{20, 2}, {10, 1}, {30, 3}});

    // add one more element
    cset_i_insert(&set, 40);
    cvec_p_push_back(&vec, (struct Point) {40, 4});
    cdeq_i_push_front(&deq, 5);
    clist_i_push_front(&lst, 5);
    cqueue_i_push(&que, 40);
    csmap_i_emplace(&map, 40, 4);

    // print them
    c_foreach (i, cset_i, set) printf(" %d", *i.ref); puts("");
    c_foreach (i, cvec_p, vec) printf(" (%f, %f)", i.ref->x, i.ref->y); puts("");
    c_foreach (i, cdeq_i, deq) printf(" %d", *i.ref); puts("");
    c_foreach (i, clist_i, lst) printf(" %d", *i.ref); puts("");
    c_foreach (i, cqueue_i, que) printf(" %d", *i.ref); puts("");
    c_foreach (i, csmap_i, map) printf(" {%d: %d}", i.ref->first, i.ref->second);

    // cleanup
    cset_i_del(&set);
    cvec_p_del(&vec);
    cdeq_i_del(&deq);
    clist_i_del(&lst);
    cqueue_i_del(&que);
    csmap_i_del(&map);
}
```
Outputs
```
 10 20 30 40
 (10.000000, 1.000000) (20.000000, 2.000000) (30.000000, 3.000000) (40.000000, 4.000000)
 5 10 20 30
 5 10 20 30
 10 20 30 40
 {10: 1} {20: 2} {30: 3} {40: 4}
```

Highlights
----------
- **User Friendly** - Easy to use, as can be seen from the examples above. The ***using_***-declaration instantiates the container type to use. You may pass *optional* arguments for customization of value- *comparison*, *destruction*, *cloning*, *convertion types*, and more. Methods have in most cases similar named corresponding methods in STL.
- **High Performance** - The containers are written with efficiency, low memory usage, and small code size in mind. Most containers perform similarly to the c++ STL containers, however **cmap** and **cset** are around 5 ***-five-*** times faster than the STL equivalents, *std::unordered_map* and *std::unordered_set*. See below. Also **cdeq** are in some cases significantly faster than *std::deque*, however implementations vary between different c++ compilers.
- **Type Safe** - No more casting that obscure bugs in your code. The compiler will let you know when you're passing wrong container or element types to your methods, and no more error prone casting of elements back from your containers.
- **Uniform API** - Methods to *construct*, *initialize*, *iterate* and *destruct* are intuitive and consistent across the various containers in the library. Makes it easier to learn how to use the library.
- **Small Footprint** - Generated code is surprisingly small, partly due to the small library code base. The example above with six different containers resulted in an executable of ***18 kb in size*** without dependencies, using the TinyC compiler! It compiles and links instantaniously. Compare this with a corresponding c++ program using STL.
- **Dual Mode Compilation** - Can be used a simple header-only library with static methods (default), or as a traditional library by defining STC_HEADER in your project. See below for instructions.
- **Simple Installation** - It is headers only by default.

Installation
------------

Because it is headers only, files can simply be included in your program. The methods will be static by default (some inlined). You may add the project folder to CPATH environment variable, to let gcc, clang, or tinyc locate the headers.

If containers are extensively used accross several translation units with common instantiated container types, it is recommended to build as a "library", to minimize executable size. To enable this mode, specify **-DSTC_HEADER** as compiler option, and put all the instantiations of containers used in one single C-file, e.g.:
```c
#define STC_IMPLEMENTATION
#include <stc/cstr.h>
#include <stc/cmap.h>
#include <stc/cvec.h>
#include <stc/clist.h>
#include "Point.h"

using_cmap(ii, int, int);
using_cset(ix, int64_t);
using_cvec(i, int);
using_clist(pt, struct Point);
...
```
Performance
-----------

All containers have templated intrusive elements. The unordered map and set are among the most performance critical containers. **cmap** and **cset** are among the very fastest unordered map implementations available, also considering highly optimized c++ implementations. Below are some benchmarks for this.

Compiled with clang -O3 -x c++, v10.0 on windows, Ryzen 7 2700X CPU. Similar results with VC and g++.

- **CMAP** = stc/*cmap*
- KMAP = klib/*khash*
- UMAP = *std::unordered_map*
- SMAP = *spp::sparse_hash_map*
- BMAP = *ska::bytell_hash_map*
- FMAP = *ska::flat_hash_map*
- RMAP = *robin_hood::unordered_map*
- HMAP = *tsl::hopscotch_map*
```
Random keys are in range [0, 2^24), seed = 1600720716:

Unordered maps: 30000000 repeats of Insert random key + try to remove a random key:
CMAP: time:  4.35, sum: 450000015000000, size: 8153497, erased 10922466
KMAP: time:  3.37, sum: 450000015000000, size: 8153497, erased 10922466
UMAP: time: 11.19, sum: 450000015000000, size: 8153497, erased 10922466
SMAP: time: 14.21, sum: 450000015000000, size: 8153497, erased 10922466
BMAP: time:  5.00, sum: 450000015000000, size: 8153497, erased 10922466
FMAP: time:  4.68, sum: 450000015000000, size: 8153497, erased 10922466
RMAP: time:  3.16, sum: 450000015000000, size: 8153497, erased 10922466
HMAP: time:  4.49, sum: 450000015000000, size: 8153497, erased 10922466

Unordered maps: Insert 30000000 index keys, then remove them in same order:
CMAP: time:  3.27, erased 30000000
KMAP: time:  2.86, erased 30000000
UMAP: time: 15.80, erased 30000000
SMAP: time: 22.60, erased 30000000
BMAP: time:  7.93, erased 30000000
FMAP: time:  5.91, erased 30000000
RMAP: time:  3.22, erased 30000000
HMAP: time:  5.91, erased 30000000

Unordered maps: Insert 30000000 random keys, then remove them in same order:
CMAP: time:  3.16, erased 13971002
KMAP: time:  4.05, erased 13971002
UMAP: time: 10.77, erased 13971002
SMAP: time: 13.77, erased 13971002
BMAP: time:  5.27, erased 13971002
FMAP: time:  4.41, erased 13971002
RMAP: time:  3.21, erased 13971002
HMAP: time:  4.50, erased 13971002

Unordered maps: Iterate 30000000 random keys:
CMAP: time:  0.23, size: 13971002, sum 1344701724191145
UMAP: time:  3.32, size: 13971002, sum 1344701724191145
SMAP: time:  0.33, size: 13971002, sum 1344701724191145
BMAP: time:  0.60, size: 13971002, sum 1344701724191145
FMAP: time:  0.56, size: 13971002, sum 1344701724191145
HMAP: time:  0.51, size: 13971002, sum 1344701724191145
```
From these tests *cmap*, *robin_hood* and *khash* are almost equally fast. std::unordered_map is bad. With random numbers in 0 - 2^20 range, khash performs slightly worse:
```
Unordered maps: 30000000 repeats of Insert random key + try to remove a random key:
CMAP: time:  1.93, sum: 450000015000000, size: 524809, erased 14738434
KMAP: time:  7.28, sum: 450000015000000, size: 524809, erased 14738434

Unordered maps: Insert 30000000 random keys, then remove them in same order:
CMAP: time:  1.16, erased 1048576
KMAP: time:  2.62, erased 1048576
```
Memory efficiency
-----------------

The containers are memory efficent, i.e. they occupy as little memory as practical possible.
- **cstr**, **cvec**: Type size: one pointer. The size and capacity is stored as part of the heap allocation that also holds the vector elements.
- **clist**: Type size: one pointer. Each node allocates block storing value and next pointer.
- **cmap**: Type size: 4 pointers. cmap uses one table of keys+value, and one table of precomputed hash-value/used bucket, which occupies only one byte per bucket. The open hashing has a default max load factor of 85%, and hash table scales by 1.5x when reaching that.
- **cset**: Same as cmap, but this uses a table of keys only, not (key, value) pairs.
- **carray**: carray1, carray2 and carray3. Type size: One pointer plus one, two, or three size_t variables to store dimensions. Arrays are allocated as one contiguous block of heap memory.

More on **cmap**
----------------

**cmap** uses open hashing, and default max load-factor is 0.85.

You can customize the destroy-, hash-, equals- functions, but also define a convertion from a raw/literal type to the key-type specified. This is very useful when e.g. having cstr as key, and therefore a few using-macros are pre-defined
for **cmap** with **cstr** keys and/or values:

- *using_cmap_strkey(tag, valuetype)*
- *using_cmap_strval(tag, keytype)*
- *using_cmap_str()* // cstr -> cstr
- *using_cset_str()* // cstr set

To customize your own cmap type to work like these, you may want to look at examples. One shows how to use a custom struct as a hash map key, by using the optional parameters of *using_cmap()*.
