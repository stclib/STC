STC - C99 Standard Containers library
=====================================

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
- **stc/cstack.h** - **stack** adapter, currently works with *cvec*.
- **stc/cqueue.h** - **queue** adapter, currently works with *clist*.
- **stc/cpqueue.h** - **priority queue** adapter. Works with *cvec*.
- **stc/coption.h** - Implementation of a **getopt_long()**-like function, *coption_get()*, to parse command line arguments.
- **stc/crandom.h** - A few very efficent modern random number generators *pcg32* and my own *64-bit PRNG* inspired by *sfc64*. Both uniform and normal distributions.
- **stc/cdefs.h** - A common include file with a few general definitions.

The usage of the containers is vert similar to the C++ standard containers, so it should be easy if you are familiar with them.

All containers mentioned above, except cstr_t and cbitset_t are generic and typesafe (similar to templates in C++). No casting is used. A simple example:
```
#include <stc/cvec.h>
using_cvec(i, int);

int main(void) {
    cvec_i vec = cvec_ini;
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

using_cmap(ii, int, int);
using_cset(ix, int64_t);
using_cvec(i, int);
using_clist(v3, Vec3);
...
```
Performance
-----------

The library is highly efficent. All containers have templated intrusive elements. One of the most performance critical containers is the **cmap / cset**. Luckily, cmap is among the fastest C/C++ hash map implementations available, see **examples/benchmark.c**

Compiled with g++ v9.2.0 -O3 on windows, Ryzen 7 2700X CPU. Similar results with VC and clang.

- **CMAP**=stc/*cmap*
- KMAP=klib/*khash*
- UMAP=*std::unordered_map*
- BMAP=*ska::bytell_hash_map*
- FMAP=*ska::flat_hash_map*
- RMAP=*robin_hood::unordered_map*
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

cmap discussion
---------------

**cmap / cset** support managing memory of the elements, i.e., you may customize the destroy-, hash- and equals- function, in addition to convertion to from some literal input types. E.g. it enables the usage of string literals as key in functions, instead of requering a constructed cstr as input.
```
using_cmap_strkey(si, int); // cstr_t => int map. Uses a special instatiatiation for cstr that allows you to input string literals as key.
...
cmap_si_emplace(&map, "mykey", 12); // construct and assign cstr("mykey") inside function, only if not already exist.
```
As string are common both as key and value, these are predefined:
- *using_cmap_strkey(tag, valuetype)* // *cstr_t* => *valuetype*
- *using_cmap_strval(tag, keytype)* // *keytype* => *cstr_t*
- *using_cmap_str()* // cstr_t => *cstr_t*, tag is *str*.
- *using_cset_str()* // cstr_t **cset**
```
using_cmap_strkey(si, int);  // cstr_t => int map. Uses a special instatiatiation for cstr that allows you to input string literals as key.
...
cmap_si map = cmap_ini;
cmap_si_emplace(&map, "mykey", 12); // construct and assign a cstr_t inside the function, but only if not already exist.
int x = cmap_si_find(&map, "mykey")->second; // Also accepts const char* string input
cmap_si_destroy(&map);
```
To customize your own cmap type to work like cmap_str, you may want to look at **examples/advanced.c**. It demonstrates how to use a custom struct as a hash map key, using the optional parameters to using_cmap().

Example usages
--------------
The examples folder contains further examples.

**cstr** string example.
```
#include <stc/cstr.h>

int main() {
    cstr_t s1 = cstr("one-nine-three-seven-five");
    printf("%s.\n", s1.str);

    cstr_insert(&s1, 3, "-two");
    printf("%s.\n", s1.str);

    cstr_erase(&s1, 7, 5); // -nine
    printf("%s.\n", s1.str);

    cstr_replace(&cs, cstr_find(&cs, "seven"), 5, "four");
    printf("%s.\n", s1.str);
    
    // reassign:
    cstr_assign(&s1, "one two three four five six seven");
    cstr_append(&s1, " eight");
    printf("append: %s\n", s1.str);

    cstr_t full_path = cstr_from("%s/%s.%s", "directory", "filename", "ext");
    printf("%s\n", full_path.str);
    
    cstr_mdestroy(&s1, &full_path);
}
```
**cvec** of *int64_t*. 
```
#include <stc/cvec.h>

using_cvec(ix, int64_t); // ix is just an example type tag name.

int main() {
    cvec_ix bignums = cvec_ini; // use cvec_ix_init() if initializing after declaration.
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

using_cvec_str();

int main() {
    cvec_str names = cvec_ini;
    cvec_str_emplace_back(&names, "Mary");
    cvec_str_emplace_back(&names, "Joe");
    cstr_assign(&names.data[1], "Jake"); // replace "Joe".
    
    // Use puch_back() for arguments of type cstr_t: is moved to the map.
    cvec_str_push_back(&names, cstr_from("%d elements so far", cvec_size(names))); 

    printf("%s\n", names.data[1].str); // Access the string char*
    c_foreach (i, cvec_str, names)
        printf("get %s\n", i.get->str);
    cvec_str_destroy(&names);
}
```
**cmap** of *int -> int*.
```
#include <stdio.h>
#include <stc/cmap.h>

using_cmap(ii, int, int);

int main() {
    cmap_ii nums = cmap_ini;
    cmap_ii_put(&nums, 8, 64); // similar to insert_or_assign()
    cmap_ii_emplace(&nums, 11, 121); 

    printf("%d\n", cmap_ii_find(nums, 8)->value);
    cmap_ii_destroy(&nums);
}
```
**cset** of *cstr_t* type.
```
#include <stc/cstr.h>
#include <stc/cmap.h>

using_cset_str(); 

int main() {
    cset_str words = cset_ini;
    cset_str_insert(&words, "Hello");
    cset_str_insert(&words, "Sad");
    cset_str_insert(&words, "World");    

    cset_str_erase(&words, "Sad");

    // iterate the set:
    c_foreach (i, cset_str, words)
        printf("%s\n", i.get->str);
    cset_str_destroy(&words);
}
```
**cmap** of *cstr_t -> cstr_t*. Both cstr keys and values are created internally via *cstr()* from const char* inputs.
```
#include <stc/cstr.h>
#include <stc/cmap.h>

using_cmap_str(); // cstr_t => cstr_t

int main() {
    cmap_str table = cmap_ini;
    cmap_str_put(&table, "Make", "my");
    cmap_str_put(&table, "Rainy", "day");
    cmap_str_insert_or_assign(&table, "Sunny", "afternoon"); // same as put()
    printf("Sunny: %s\n", cmap_str_find(table, "Sunny")->second.str);
    cmap_str_erase(&table, "Rainy");

    printf("size = %zu\n", cmap_size(table));
    c_foreach (i, cmap_str, table)
        printf("%s: %s\n", i.get->first.str, i.get->second.str);
    cmap_str_destroy(&table); // frees key and value cstrs, and hash table.
}
```
**clist** of *int64_t*. Similar to c++ *std::forward_list*, but can do both *push_front()* and *push_back()* as well as *pop_front()*.
```
#include <stdio.h>
#include <time.h>
#include <stc/clist.h>
#include <stc/crandom.h>

using_clist(fx, double);

int main() {
    // Random engine, with uniform distribution from 100 to 1000:
    crand_eng64_t eng = crand_eng64_init(time(NULL));
    crand_uniform_f64_t dist = crand_uniform_f64_init(100.0, 1000.0);
    
    int k;
    // Fill singly linked list with random real numbers
    clist_fx list = clist_ini;
    for (int i = 0; i < 10000000; ++i)
        clist_fx_push_back(&list, crand_uniform_f64(&eng, &dist));
    
    k = 0;
    c_foreach (i, clist_fx, list)
        if (++k <= 10) printf("%8d: %10f\n", k, *i.get); else break;

    clist_fx_sort(&list); // mergesort O(n*log n)
    puts("sorted");

    k = 0;
    c_foreach (i, clist_fx, list)
        if (++k <= 10) printf("%8d: %10f\n", k, *i.get); else break;

    clist_fx_clear(&list);

    // generic push a list of items, works on most containers:
    c_push_items(&list, clist_fx, {10, 20, 30, 40, 50});

    c_foreach (i, clist_fx, list)
        printf("%f ", *i.get);
    puts("");

    clist_fx_destroy(&list);
}
```
**carray**. 1d, 2d and 3d arrays, allocated from heap in one memory block. *carray3* may have sub-array "views" of *carray2* and *carray1* etc., as shown in the following example:
```
#include <stdio.h>
#include <stc/carray.h>

using_carray(f, float);

int main()
{
    carray3f a3 = carray3f_make(30, 20, 10, 0.0f);  // define a3[30][20][10], init with 0.0f.
    *carray3f_at(a3, 5, 4, 3) = 3.14f;         // a3[5][4][3] = 3.14

    carray1f a1 = carray3f_at2(a3, 5, 4);      // sub-array a3[5][4] (no data copy).    
    carray2f a2 = carray3f_at1(a3, 5);         // sub-array a3[5]
    
    printf("%f\n", *carray2f_at(a1, 3));       // a1[3] (3.14f)
    printf("%f\n", *carray2f_at(a2, 4, 3));    // a2[4][3] (3.14f)
    printf("%f\n", *carray3f_at(a3, 5, 4, 3)); // a3[5][4][3] (3.14f)
    // ...
    carray2f_destroy(&a1); // does nothing, since it is a sub-array.
    carray2f_destroy(&a2); // same.
    carray3f_destroy(&a3); // free array, and invalidates a1, a2.
}
```
