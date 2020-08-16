STC - C99 STandard Container library
====================================

Introduction
------------

An elegant, fully typesafe, generic, customizable, user-friendly, consistent, and very fast standard container library for C99. This is a small headers only library with the most used container components, and a few algorithms:
- **stc/carray.h** - Dynamic generic **multi-dimensional array**, implemented as a single contiguous section of memory.
- **stc/cbitset.h** - Bitset similar to c++ std::bitset or boost::dynamic_bitset.
- **stc/clist.h** - A genric circular **singly linked list**. Can be used as a **queue** as it supports *push_back(), push_front(), and pop_front()*. It also contains various *splice* functions and *merge sort*.
- **stc/cmap.h** - A generic **unordered map** implemented as open hashing without tombstones. Highly customizable and fast.
- **stc/cset.h** - A generic **unordered set** implemented in tandem with *unordered map*
- **stc/cstr.h** - Compact and powerful **string** class.
- **stc/cvec.h** - Dynamic generic **vector** class, works well as a **stack**.
- **stc/cpqueue.h** - Priority queue adapter for **cvec.h**, as a **heap**.
- **stc/copt.h** - Implementation of a **getopt_long()**-like function, *copt_get()*, to parse command line arguments.
- **stc/crandom.h** - A few very efficent modern random number generators *pcg32* and my own *64-bit PRNG* inspired by *sfc64*.
- **stc/cdefs.h** - A common include file with some general definitions.

The usage of the containers is vert similar to the C++ standard containers, so it should be easy if you are familiar with them.

All containers mentioned above, except cstr_t and cbitset_t are generic and typesafe (similar to templates in C++). No casting is used. A simple example:
```
#include <stc/cvec.h>
declare_cvec(i, int);

int main(void) {
    cvec_i vec = cvec_init;
    cvec_i_push_back(&vec, 42);
    cvec_i_destroy(&vec);
}
```
Motivation
----------

The aim of this project was to create a small **Standard Container Library for the C99 language**. It should
- be easy to use, have intuitive naming and consistency across the library.
- be type safe. Have minimal usage of casting and void* pointers.
- be highly efficient. Both in speed and memory usage.
- be customizable without losing efficiency. E.g. inline replacable compare, hash, allocation functions per container type instantiation.
- have a fairly small code base, and easy to install, deploy and maintain.
- avoid bloat. It should not try to cover all thinkable functions, but limit itself to the most useful and commonly used.

Installation
------------

Because it is headers only, files can simply be included in your program. The functions will be inlined by default. If containers are extensively used accross many tranlation units with common instantiated container types, it is recommended to build as a library, to minimize executable size. To enable this mode, specify **-DSTC_HEADER** as compiler option, and put all the instantiations of the containers used in one C file, like this:
```
#define STC_IMPLEMENTATION
#include <stc/cstr.h>
#include <stc/cmap.h>
#include <stc/cvec.h>
#include <stc/clist.h>
#include "Vec3.h"

declare_cmap(ii, int, int);
declare_cset(ix, int64_t);
declare_cvec(i, int);
declare_clist(v3, Vec3);
...
```
Performance
-----------

The library is very efficent. Containers have templated intrusive elements. One of the most performance critical containers is the **cmap / cset**. Luckily, cmap is among the fastest C/C++ map implementations available, see **examples/benchmark.c**

Compiled with g++ v9.2.0 -O3 on windows, Ryzen 7 2700X CPU. Similar results with VC and clang.
**CMAP**=*cmap*, KMAP=*khash*, **UMAP**=*std::unordered_map*, BMAP=*ska::bytell_hash_map*, FMAP=*ska::flat_hash_map*, RMAP=*robin_hood::unordered_map*
```
Random keys are in range [0, 2^20):

Unordered maps: 50000000 repeats of insert a random key + (attemt to) remove another random key:
CMAP: time:  2.58 sec
KMAP: time: 11.80 sec
UMAP: time: 16.07 sec
BMAP: time:  3.54 sec
FMAP: time:  2.79 sec
RMAP: time:  2.70 sec

Unordered maps: Insert 50000000 sequenced keys, then remove all in same order:
CMAP: time: 5.28 sec
KMAP: time: 3.34 sec
UMAP: time: 4.91 sec
BMAP: time: 5.37 sec
FMAP: time: 4.51 sec
RMAP: time: 4.91 sec

Unordered maps: Insert 100000000 random keys, then remove all in same order:
CMAP: time:  2.66 sec
KMAP: time:  6.27 sec
UMAP: time: 15.30 sec
BMAP: time:  5.17 sec
FMAP: time:  3.37 sec
RMAP: time:  3.93 sec
```
Memory efficiency
-----------------

The containers are memory efficent, i.e. they occupy as little memory as practical possible.
- **cstr**, **cvec**: Type size: one pointer. The size and capacity is stored as part of the heap allocation that also holds the vector elements.
- **clist**: Type size: one pointer. Each node allocates block storing value and next pointer.
- **cmap**: Type size: 4 pointers. cmap uses one table of keys+value, and one table of precomputed hash-value/used bucket, which occupies only one byte per bucket. The open hashing has a default max load factor of 85%, and hash table scales by 1.5x when reaching that.
- **cset**: Same as cmap, but this uses a table of keys only, not (key, value) pairs.
- **carray**: carray1, carray2 and carray3. Type size: One pointer plus one, two, or three size_t variables to store dimensions. Arrays are allocated as one contiguous block of heap memory.

cmap, cset and cvec discussion
------------------------------

**cmap/cset** are the most complex of the containers (although, currently only ~380 lines of code). It uses open hashing, but does not rely on power-of-two size table, nor prime number lengths, and it does not have tombstone buckets. It is still among the fastest hash-tables, as shown above. The default max load-factor is 0.85, and it shrinks (and rehashes) when load-factor goes below 0.15, by default (can be set per hash container).

You can customize the destroy-, hash- and equals- function. **cmap/cset** also supports a few other arguments in the declare-statement that allows to define a convertion from a raw/literal type to the key-type specified. This is very useful when e.g. having cstr as key, as it enables the usage of string literals as key in *put() and find()* functions, instead of requering a constructed cstr. Without it, the code would become: 
```
declare_cmap(si, cstr_t, int); // don't do this.
...
cmap_si_put(&map, cstr_make("mykey"), 12);
```
This is a problem because cstr_t key may exist in the map, and it would need to destroy the current key and replace it with the new to avoid memory leak.  Lookup would also be problematic:
```
cstr lookup = cstr_make("mykey");
int x = cmap_si_find(&map, lookup)->value;
cstr_destroy(&lookup);
```
To avoid all this, use *declare_cmap_str(tag, keytype)* or *declare_cset_str()*:
```
declare_cmap_str(si, int);
...
cmap_si map = cmap_init;
cmap_si_put(&map, "mykey", 12);             // constructs a cstr_t key from the const char* internally.
int x = cmap_si_find(&map, "mykey")->value; // no allocation of string key happens here.
cmap_si_destroy(&map);
```
An alternative is to use *char* * as key type, but then you must manage allcoated memory of the hash char* keys yourself.
Note that this customization is also available for **cvec**, but only affects the *find()* function currently. See *declare_cvec_str()*.

You may want to look at **examples/advanced.c**, it demonstrates how to use a custom struct as a hash map key, using the optional parameters to declare_cmap().

Example usages
--------------
The examples folder contains further examples.

**cstr** string example.
```
#include <stc/cstr.h>

int main() {
    cstr_t s1 = cstr_make("one-nine-three-seven-five");
    printf("%s.\n", s1.str);

    cstr_insert(&s1, 3, "-two");
    printf("%s.\n", s1.str);

    cstr_erase(&s1, 7, 5); // -nine
    printf("%s.\n", s1.str);

    cstr_replace(&cs, cstr_find(cs, "seven", 0), 5, "four");
    printf("%s.\n", s1.str);
    
    // reassign:
    cstr_assign(&s1, "one two three four five six seven");
    cstr_append(&s1, " eight");
    printf("append: %s\n", s1.str);
    cstr_destroy(&s1);

    cstr_t s2 = cstr_from("Index %d: %f", 123, 4.56);
    cstr_destroy(&s2);
}
```
**cvec** of *int64_t*. 
```
#include <stc/cvec.h>
declare_cvec(ix, int64_t); // ix is just an example tag name, use anything without underscore.

int main() {
    cvec_ix bignums = cvec_init; // use cvec_ix_init() if initializing after declaration.
    cvec_ix_reserve(&bignums, 100);
    for (size_t i = 0; i<100; ++i)
        cvec_ix_push_back(&bignums, i * i * i);
    cvec_ix_pop_back(&bignums); // erase the last

    uint64_t value;
    for (size_t i = 0; i < cvec_size(bignums); ++i)
        value = bignums.data[i];
    cvec_ix_destroy(&bignums);
}
```
**cvec** of *cstr_t*.
```
#include <stc/cstr.h>
#include <stc/cvec.h>
declare_cvec_str();

int main() {
    cvec_str names = cvec_init;
    cvec_str_push_back(&names, cstr_make("Mary"));
    cvec_str_push_back(&names, cstr_make("Joe"));
    cstr_assign(&names.data[1], cstr_make("Jake")); // replace Joe

    printf("%s\n", names.data[1].str); // Access the string char*
    cvec_str_destroy(&names);
}
```
**cmap** of *int -> int*.
```
#include <stdio.h>
#include <stc/cmap.h>
declare_cmap(ii, int, int);

int main() {
    cmap_ii nums = cmap_init;
    cmap_ii_put(&nums, 8, 64); // put() works as c++ std::unordered_map<>::insert_or_replace()
    cmap_ii_insert(&nums, 11, 121); // only insert value if key does not exists - like std::unordered_map::insert().

    printf("%d\n", cmap_ii_find(nums, 8)->value);
    cmap_ii_destroy(&nums);
}
```
**cset** of *cstr*.
```
#include <stc/cstr.h>
#include <stc/cmap.h>
declare_cset_str(); // cstr set. See the discussion above.

int main() {
    cset_str words = cset_init;
    cset_str_put(&words, "Hello");
    cset_str_put(&words, "Groovy");
    cset_str_erase(&words, "Hello");

    // iterate the map:
    c_foreach (i, cset_str, words)
        printf("%s\n", i.item->key.str);
    cset_str_destroy(&words);
}
```
**cmap** of *cstr -> cstr*. Temporary cstr values are created by *cstr_make()*, and moved into the container
```
#include <stc/cstr.h>
#include <stc/cmap.h>
declare_cmap_str(ss, cstr, cstr_destroy); 

int main() {
    cmap_ss table = cmap_init;
    cmap_ss_put(&table, "Make", cstr_make("my"));
    cmap_ss_put(&table, "Sunny", cstr_make("day"));
    printf("Sunny: %s\n", cmap_ss_find(table, "Sunny")->value.str);
    cmap_ss_erase(&table, "Make");

    printf("size %d\n", cmap_size(table));
    cmap_ss_destroy(&table); // frees key and value cstrs, and hash table.
}
```
**clist** of *int64_t*. Similar to c++ *std::forward_list*, but can do both *push_front()* and *push_back()* as well as *pop_front()*.
```
#include <stdio.h>
#include <time.h>
#include <stc/clist.h>
#include <stc/crandom.h>
declare_clist(fx, double);

int main() {
    clist_fx list = clist_init;
    crandom_eng64_t eng = crandom_eng64_init(time(NULL));
    crandom_uniform_f64_t dist = crandom_uniform_f64_init(100.0, 1000.0);
    int k;
    
    for (int i = 0; i < 10000000; ++i)
        clist_fx_push_back(&list, crandom_uniform_f64(&eng, dist));
    k = 0; c_foreach (i, clist_fx, list)
        if (++k <= 100) printf("%8d: %10f\n", k, i.item->value); else break;

    clist_fx_sort(&list); // mergesort O(n*log n)
    puts("sorted");

    k = 0; c_foreach (i, clist_fx, list)
        if (++k <= 100) printf("%8d: %10f\n", k, i.item->value); else break;

    clist_fx_clear(&list);

    // generic c_push() function, works on most containers:
    c_push(&list, clist_fx, c_items(10, 20, 30, 40, 50));
    c_foreach (i, clist_fx, list) printf("%f ", i.item->value);
    puts("");

    clist_fx_destroy(&list);
}
```
**carray**. 1d, 2d and 3d arrays, allocated from heap in one memory block. *carray3* may have sub-array "views" of *carray2* and *carray1* etc., as shown in the following example:
```
#include <stdio.h>
#include <stc/carray.h>
declare_carray(f, float);

int main()
{
    carray3f a3 = carray3f_make(30, 20, 10, 0.f);
    carray3f_data(a3, 5, 4)[3] = 10.2f; // a3[5][4][3]
    carray2f a2 = carray3f_at(a3, 5);   // sub-array reference (no data copy).

    printf("%f\n", carray2f_value(a2, 4, 3));    // readonly lookup a2[4][3] (=10.2f)
    printf("%f\n", carray2f_data(a2, 4)[3]);     // same, but this is writable.
    printf("%f\n", carray2f_at(a2, 4).data[3]);  // same, via sub-array access.
    
    printf("%f\n", carray3f_value(a3, 5, 4, 3)); // same data location, via a3 array.
    printf("%f\n", carray3f_data(a3, 5, 4)[3]);
    printf("%f\n", carray3f_at2(a3, 5, 4).data[3]);
    
    carray2f_destroy(&a2); // does nothing, since it is a sub-array.
    carray3f_destroy(&a3); // also invalidates a2.
}
```
