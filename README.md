STC - C99 Standard Container Library
====================================

Introduction
------------

An elegant, typesafe, generic, customizable, user-friendly, consistent, and very fast standard container library for C99. This is a small headers only library with the most used container components, and a few algorithms:
- **stc/carray.h** - Dynamic generic **multi-dimensional array**, implemented as a single contiguous section of memory.
- **stc/cbitset.h** - Bitset similar to c++ std::bitset or boost::dynamic_bitset.
- **stc/clist.h** - A genric circular singly **linked list**, can be used as a **queue** - supports *pushBack, pushFront, and popFront* - **stc/cmap.h** - A generic **unordered map** implemented as open hashing without tombstones. Highly customizable and fast.
- **stc/cset.h** - A generic **unordered set** implemented in tandem with *unordered map*
- **stc/cstr.h** - Compact and powerful **string** class.
- **stc/cvec.h** - Dynamic generic **vector** class, works well as a **stack**.
- **stc/cvecpq.h** - Priority queue adapter for **cvec.h**, implemented as a **heap**.
in *O*(1). Also contains various *splice* functions and (merge) *sort*.
- **stc/coption.h** - Implementation of *getopt_long*-"like" function, *coption_get*, to parse command line arguments.
- **stc/crandom.h** - A few very efficent modern random number generators *pcg32* and *sfc64*. It also implements the crypto-strong *siphash* algorithm.
- **stc/cdefs.h** - A common include file with some general definitions.

The usage of the containers is similar to the C++ standard containers, so it should be easier for those who are familiar with them.

All containers mentioned above, except for CStr are generic (similar to templates in C++). A simple example:
```
#include <stc/cvec.h>
declare_CVec(i, int);

int main(void) {
    CVec_i vec = cvec_init;
    cvec_i_pushBack(&vec, 42);
    cvec_i_destroy(&vec);
}
```
Motivation
----------

The aim of this project was to create a small **Standard Container Library for the C99 language**. I suspect that most other attempts at this has failed because it did not meet one or several of the following requirements: I believe a standard container library should ..
- be easy to use, have intuitive naming and consistency across the library.
- be type safe. Have minimal usage of casting and void* pointers.
- be highly efficient. Both in speed and memory usage.
- be customizable without losing efficiency. E.g. inline replacable compare, hash, allocation functions per container type instantiation.
- have a fairly small code base, and easy to install, deploy and maintain.
- avoid bloat. It should not try to cover all thinkable functions, but limit itself to the most useful and commonly used.

The library is not complete or free of possible bugs, but I believe that it already covers a large portion of the most needed containers.

Installation
------------

Because it is headers only, files can simply be included in your program. The functions will be inlined by default. If containers are extensively used accross many tranlation units with common instantiated container types, it is recommended to build as a library, to minimize executable size. To enable this mode, specify **-DSTC_HEADER** as compiler argument, and put all the instantiations of the containers used in one C file, e.g.
```
#define STC_IMPLEMENTATION
#include <stc/cmap.h>
#include <stc/cvec.h>

declare_CMap(ii, int, int); // map
declare_CMap(ix, int64_t);  // set
declare_CVec(i, int);
...
```
Performance
-----------

This library is very efficent. Containers have templated intrusive elements. One of the most performance critical containers is the **CMap / CSet**. Luckily, CMap is among the fastest C/C++ map implementations available: **examples/benchmark.c** compiled with g++ v9.2.0 -O3 on windows (the results are similar with VC++ and g++ on linux):

**CMAP**=*CMap*, KMAP=*khash*, UMAP=*std::unordered_map*, BMAP=*ska::bytell_hash_map*, FMAP=*ska::flat_hash_map*, RMAP=*robin_hood::unordered_map*
```
Random keys are in range [0, 2^20):
map<uint64_t, uint64_t>: 7000000 repeats of Insert random key + (try to) remove a different random key:
CMAP(ii): sz: 523938, bucks: 1013337, time: 0.39, sum: 24500003500000, erase: 3237392 --> fastest
KMAP(ii): sz: 523938, bucks: 2097152, time: 0.46, sum: 24500003500000, erase: 3237392
UMAP(ii): sz: 523938, bucks: 1056323, time: 2.21, sum: 24500003500000, erase: 3237392
BMAP(ii): sz: 523938, bucks: 1048576, time: 0.46, sum: 24500003500000, erase: 3237392
FMAP(ii): sz: 523938, bucks: 1048576, time: 0.43, sum: 24500003500000, erase: 3237392
RMAP(ii): sz: 523938, bucks: 838860, time: 0.82, sum: 24500003500000, erase: 3237392

map<uint64_t, uint64_t>: Insert 10000000 sequensial keys, then remove them in same order:
CMAP(ii): sz: 0, bucks: 17001171, time: 0.75, erase 10000000 --> second fastest
KMAP(ii): sz: 0, bucks: 16777216, time: 0.48, erase 10000000
UMAP(ii): sz: 0, bucks: 17961079, time: 1.04, erase 10000000
BMAP(ii): sz: 0, bucks: 16777216, time: 1.04, erase 10000000
FMAP(ii): sz: 0, bucks: 16777216, time: 0.94, erase 10000000
RMAP(ii): sz: 0, bucks: 13421772, time: 0.84, erase 10000000

map<uint64_t, uint64_t>: Insert 10000000 random keys, then remove them in same order:
CMAP(ii): sz: 0, bucks: 1621347, time: 0.41, erase 1048490 --> fastest
KMAP(ii): sz: 0, bucks: 2097152, time: 0.77, erase 1048490
UMAP(ii): sz: 0, bucks: 2144977, time: 1.67, erase 1048490
BMAP(ii): sz: 0, bucks: 2097152, time: 0.52, erase 1048490
FMAP(ii): sz: 0, bucks: 2097152, time: 0.44, erase 1048490
RMAP(ii): sz: 0, bucks: 1677721, time: 0.65, erase 1048490
```
Memory efficiency
-----------------

The containers are memory efficent, i.e. they occupy as little memory as practical possible.
- **CStr**, **CVec**: Representaion: one pointer size. The size and capacity is stored as part of the heap allocation that also holds the vector elements.
- **CList**: Representation: one pointer size. Each node allocates block storing value and next pointer.
- **CSet**: Representation: 4 pointers size. CSet uses one table of keys, and one table of "used/hash-value", which occupies only one byte per bucket.
- **CMap**: Same as CSet, but this uses a table of (key, value) pairs, not only keys.
- **CArray**: CArray1, CArray2 and CArray3. Representation: One pointers, plus 1, 2, or 3 size_t variables to store dimensions. Elements are stored as one block of heap memory.

CMap, CSet and CVec discussion
----------------------------

**CMap/CSet** are the most complex of the containers (although, currently only ~370 lines of code). It uses open hashing, but does not rely on power-of-two size table, nor prime number lengths, and it does not have tombstone buckets. It is still among the fastest hash-tables, as shown above. The default max load-factor is 0.85, and it shrinks (and rehashes) when load-factor goes below 0.15, by default (can be set per hash container).

You may customize the destroy-, hash- and equals- function. It also supports a few other arguments in the declare-statement that allows to define a convertion from a raw/literal type to the key-type specified. This is handy when e.g. having CStr as key, as it enables the usage of string literals as key in *put() and *get() functions, instead of requering a constructed CStr. Without it, you would have to write: 
```
declare_CMap(si, CStr, int);
...
cmap_si_put(&map, cstr_make("mykey"), 12);
```
but the main incovenience is with lookup:
```
CStr lookup = cstr_make("mykey");
int x = cmap_si_get(&map, lookup)->value;
cstr_destroy(&lookup);
```
To avoid this, use *declare_CMap_str()*:
```
declare_CMap_str(si, int);
...
CMap_si map = cmap_init;
cmap_si_put(&map, "mykey", 12);            // constructs a CStr key from the const char* internally.
int x = cmap_si_get(&map, "mykey")->value; // no allocation of string key happens here.
cmap_si_destroy(&map);
```
An alternative would be to use *char* * as key type, but you would have to manage the memory of the hash char* keys yourself.
Note that this customization is also available for **CVec**, but only affects the *find()* function currently. See *declare_CVec_str()*.

Also look at **examples/advanced.c**, it demonstrates how to use a custom struct as a hash map key, using the feature mentioned.

Example usages
--------------
The first example has a very complex nested container type, which demonstrates the power of this library. Look at the simpler examples below to understand it better. The example adds an element into the data structure, and then accesses it. The type used, with c++ template syntax is:
**CMapMap**< **CStr**, **CMapMap**< *int*, **CList**< **CArray2**< *float* >>>>

Note: The *cmap_sm_destroy(&theMap)* call below, will destroy all the nested containers including the memory allocated for CStr keys in theMap object.
```
#include <stc/cstr.h>
#include <stc/cmap.h>
#include <stc/clist.h>
#include <stc/carray.h>

void verify_destroy(float* v) {printf("destroy %g\n", *v);}

declare_CArray(f, float, verify_destroy); // you should omit the last argument - float type need no destroy.
declare_CList(t2, CArray2_f, carray2_f_destroy, c_noCompare);
declare_CMap(il, int, CList_t2, clist_t2_destroy);
declare_CMap_str(sm, CMap_il, cmap_il_destroy);

int main() {
    int xdim = 4, ydim = 6;
    int x = 2, y = 5, entry = 42;
    CMap_sm theMap = cmap_init;
    {
        // Construct.
        CArray2_f table = carray2_f_make(xdim, ydim, 0.f);
        CList_t2 tableList = clist_init;
        CMap_il listMap = cmap_init;
        
        // Put in some data.
        carray2_f_data(table, x)[y] = 3.1415927; // table[x][y]
        clist_t2_pushBack(&tableList, table);
        cmap_il_put(&listMap, entry, tableList);
        cmap_sm_put(&theMap, "First", listMap);
    }

    // Access the data entry
    CArray2_f table = clist_back(cmap_il_get(&cmap_sm_get(&theMap, "First")->value, entry)->value);
    printf("value is: %f\n", carray2_f_value(table, x, y));

    cmap_sm_destroy(&theMap); // free up the whole shebang!
}
```
**CStr**
```
#include <stc/cstr.h>

int main() {
    CStr s1 = cstr_make("one-nine-three-seven-five");
    printf("%s.\n", s1.str);

    cstr_insert(&s1, 3, "-two");
    printf("%s.\n", s1.str);

    cstr_erase(&s1, 7, 5); // -nine
    printf("%s.\n", s1.str);

    cstr_replace(&s1, 0, "seven", "four");
    printf("%s.\n", s1.str);
    printf("find: %s\n", s1.str + cstr_find(s1, 0, "four"));

    // reassign:
    cstr_assign(&s1, "one two three four five six seven");
    cstr_append(&s1, " eight");
    printf("append: %s\n", s1.str);
    cstr_destroy(&s1);

    CStr s2 = cstr_from("Index %d: %f", 123, 4.56);
    cstr_destroy(&s2);
}
```
**CVec** of *int64_t*
```
#include <stc/cvec.h>
declare_CVec(ix, int64_t); // ix is just an example tag name, use anything without underscore.

int main() {
    CVec_ix bignums = cvec_init; // = (CVec_ix) cvec_init; if initializing after declaration.
    cvec_ix_reserve(&bignums, 100);
    for (size_t i = 0; i<100; ++i)
        cvec_ix_pushBack(&bignums, i * i * i);
    cvec_ix_popBack(&bignums); // erase the last

    uint64_t value;
    for (size_t i = 0; i < cvec_size(bignums); ++i)
        value = bignums.data[i];
    cvec_ix_destroy(&bignums);
}
```
**CVec** of *CStr*
```
#include <stc/cstr.h>
#include <stc/cvec.h>
declare_CVec_str();

int main() {
    CVec_str names = cvec_init;
    cvec_str_pushBack(&names, cstr_make("Mary"));
    cvec_str_pushBack(&names, cstr_make("Joe"));
    cstr_assign(&names.data[1], cstr_make("Jake")); // replace Joe

    printf("%s\n", names.data[1].str); // Access the string char*
    cvec_str_destroy(&names);
}
```
**CMap** of *int -> int*
```
#include <stdio.h>
#include <stc/cmap.h>
declare_CMap(ii, int, int);

int main() {
    CMap_ii nums = cmap_init;
    cmap_ii_put(&nums, 8, 64);
    cmap_ii_put(&nums, 11, 121);

    printf("%d\n", cmap_ii_get(nums, 8)->value);
    cmap_ii_destroy(&nums);
}
```
**CSet** of *CStr*
```
#include <stc/cstr.h>
#include <stc/cmap.h>
declare_CSet_str(); // CStr set. See the discussion above.

int main() {
    CSet_str words = cset_init;
    cset_str_put(&words, "Hello");
    cset_str_put(&words, "Groovy");
    cset_str_erase(&words, "Hello");

    // iterate the map:
    c_foreach (i, cset_str, words)
        printf("%s\n", i.item->key.str);
    cset_str_destroy(&words);
}
```
**CMap** of *CStr -> CStr*. Temporary CStr values are created by *cstr_make()*, and moved into the container
```
#include <stc/cstr.h>
#include <stc/cmap.h>
declare_CMap_str(ss, CStr, cstr_destroy); 

int main() {
    CMap_ss table = cmap_init;
    cmap_ss_put(&table, "Make", cstr_make("my"));
    cmap_ss_put(&table, "Sunny", cstr_make("day"));
    printf("Sunny: %s\n", cmap_ss_get(table, "Sunny")->value.str);
    cmap_ss_erase(&table, "Make");

    printf("size %d\n", cmap_size(table));
    cmap_ss_destroy(&table); // frees key and value CStrs, and hash table (CVec).
}
```
**CList** of *int64_t*. Similar to c++ *std::forward_list*, but can do both *pushFront()* and *pushBack()*.
```
#include <stdio.h>
#include <time.h>
#include <stc/clist.h>
#include <stc/crandom.h>
declare_CList(i, uint64_t);
 
int main() {
    CList_i list = clist_init;
    int N = 2000000, n;
    sfc64_t rng = sfc64_seed(time(NULL));
    for (int i=0; i<N; ++i) // one million random numbers
        clist_i_pushBack(&list, sfc64_rand(&rng));
    n = 0; 
    c_foreach (i, clist_i, list)
        if (++n % (N/50) == 0) printf("%10d: %zu\n", n, i.item->value);
    puts("sorting:");
    // Sort them...
    clist_i_sort(&list); // mergesort O(n*log n)
    n = 0;
    puts("done:");
    c_foreach (i, clist_i, list)
        if (++n % (N/50) == 0) printf("%10d: %zu\n", n, i.item->value);
    clist_i_destroy(&list);
}
```
**CArray**. 1D, 2D and 3D arrays, heap allocated in one memory block. *CArray3* can have sub-array "views" of *CArray2* and *CArray1* etc., as shown in the following example.
```
#include <stdio.h>
#include <stc/carray.h>
declare_CArray(f, float);

int main()
{
    CArray3_f a3 = carray3_f_make(30, 20, 10, 0.f);
    carray3_f_data(a3, 5, 4)[3] = 10.2f; // a3[5][4][3]
    CArray2_f a2 = carray3_f_at(a3, 5);    // sub-array reference (no data copy).

    printf("%f\n", carray2_f_value(a2, 4, 3));    // readonly lookup a2[4][3] (=10.2f)
    printf("%f\n", carray2_f_data(a2, 4)[3]);     // same, but this is writable.
    printf("%f\n", carray2_f_at(a2, 4).data[3]);  // same, via sub-array access.
    
    printf("%f\n", carray3_f_value(a3, 5, 4, 3)); // same data location, via a3 array.
    printf("%f\n", carray3_f_data(a3, 5, 4)[3]);
    printf("%f\n", carray3_f_at2(a3, 5, 4).data[3]);
    
    carray2_f_destroy(&a2); // does nothing, since it is a sub-array.
    carray3_f_destroy(&a3); // also invalidates a2.
}
```
