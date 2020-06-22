STC - C99 Standard Container Library
====================================

Introduction
------------

An elegant, modern, generic, customizable, typesafe, consistent, user-friendly, and very efficient standard container library for C99. This is a small headers only library with the most used container components, and a few algorithms:
- **cstring.h** - Compact and powerful **string** class.
- **cvector.h** - Dynamic generic **vector** class.
- **chash.h** - Unordered **map** and **set**. Highly customizable and fast.
- **carray.h** - Multi-dimensional dynamic **array**, implemented as a single contiguous section of memory.
- **clist.h** - A circular singly linked **list**, can be used as a **queue** (supports O(1) *pushBack, pushFront, and popFront*). Also contains various *splice* functions and (merge) *sorting*.
- **coption.h** - Implementation of *getopt_long*-"like" function, *coption_get*, to parse command line arguments.
- **crandom.h** - Collection of some efficent modern random number generators *xoroshiro128ss*, *sfc32/64* and Mersenne Twister *mt19937*. It also implements the crypto-strong *siphash* algorithm.

The usage of the containers is similar to the C++ standard containers, so it should be easier for those who are familiar with them.

All containers mentioned above, except for CString are generic (similar to templates in C++). A simple example:
```
#include <stc/vector.h>
declare_CVector(i, int);

int main(void) {
    CVector_i vec = cvector_init;
    cvector_i_pushBack(&vec, 42);
    cvector_i_destroy(&vec);
}
```
Motivation
----------

The goal of this project was to finally create a **Standard Container Library for the C language**. I believe that many/most earlier attempts at this has failed because they did not meet one, or several of the following requirements:
- Easy to use, intuitive naming and consistency across the library.
- Type safe. Minimal usage of casting and void* pointers.
- Highly efficient. Both in speed and memory usage.
- Customizable without losing efficiency. E.g. inline replacable compare, hash, allocation functions per container type instantiation.
- Small code base size and easy to install, deploy and maintain.
- Avoids bloat. Should not try to cover all thinkable functions, but limit itself to the most useful and commonly used.

That said, this library is far from complete or free of possible bugs, but I believe it is a good foundation.

Installation
------------

Because it is headers only, files can simply be included in your program. The functions will be inlined by default. If containers are extensively used accross many tranlation units with common instantiated container types, it is recommended to build as a library, to minimize executable size. In this case, specify **-DSTC_HEADER** as compiler argument, and put all the instantiations of the containers used in one C file, e.g.
```
#define STC_IMPLEMENTATION
#include <stc/cvector.h>
#include <stc/chash.h>

declare_CVector(i, int);
declare_CHash(ii, map, int, int);
declare_CHash(64, set, int64_t);
...
```
Performance
-----------

This library is very efficent. Containers have templated intrusive elements. One of the most performance critical containers is the **CHash map / CHash set**. Thankfully, is it among the fastest C / C++ map implementations: **examples/benchmark.c** compiled with g++ v9.2.0 -O3 on windows (the results are similar with VC++ and g++ on linux):

**CMAP**=*CHash map*, KMAP=*khash*, UMAP=*std::unordered_map*, BMAP=*ska::bytell_hash_map*, FMAP=*ska::flat_hash_map*, RMAP=*robin_hood::unordered_map*
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

All containers are memory efficent, they take up as little memory as practical possible.
- **CString**, **CVector**: Representaion: one pointer size. The size and capacity is stored as part of the heap allocation that also holds the vector elements.
- **CList**: Representation: one pointer size. Each node allocates block storing value and next pointer.
- **CHash set**: Representation: 4 pointers size. The hash table stores a key per bucket, and one table of "used/hash-value", occupying only one byte per bucket.
- **CHash map**: Same as CHash set, but each bucket in the array stores a (key, value) pair, not only the key.
- **CArray**: Elements are stored as one memory block. Representation: Two pointers, plus variables to store dimensionality.

Demos
-----
The first example demonstrates an advanced complex type that shows some of the capability of the library. Look at the simpler to understand this better. This create one element into a large data structure (using C++ template syntax for describing the type):
**CHash_map**< *CString*, *CHash_map*< *int*, *CList*< *CArray2*< *float* > > > >
```
#include "<stc/cstring.h>
#include "<stc/chash.h>
#include "<stc/clist.h>
#include "<stc/carray.h>

declare_CArray2(f, float);
declare_CList(a2, CArray2_f, carray_f_destroy);
declare_CHash(m2, map, CList_a2, clist_a2_destroy);
declare_CHash(m, map, CHash_m2, chash_m2_destroy);

int main() {
  
}
```
**CString**
```
#include <stc/cstring.h>

int main() {
    CString s1 = cstring_makeFmt("%10.g", 123.0 / 13.0);
    CString s2 = cstring_make("one-nine-three-seven-five");
    printf("%s.\n", cs.str);

    cstring_insert(&s2, 3, "-two");
    printf("%s.\n", cs.str);

    cstring_erase(&s2, 7, 5); // -nine
    printf("%s.\n", cs.str);

    cstring_replace(&s2, 0, "seven", "four");
    printf("%s.\n", s2.str);
    printf("find: %s\n", s2.str + cstring_find(s2, 0, "four"));

    // reassign:
    cstring_assign(&s2, "one two three four five six seven");
    cstring_append(&s2, " eight");
    printf("append: %s\n", s2.str);

    cstring_destroy(&s1);
    cstring_destroy(&s2);
}
```
**CVector** of *int64_t*
```
#include <stc/cvector.h>
declare_CVector(ix, int64_t); // ix is just an example tag name, use anything without underscore.

int main() {
    CVector_ix bignums = cvector_init; // = (CVector_ix) cvector_init; if initializing after declaration.
    cvector_ix_reserve(&bignums, 100);
    for (size_t i = 0; i<100; ++i)
        cvector_ix_pushBack(&bignums, i * i * i);
    cvector_ix_popBack(&bignums); // erase the last

    uint64_t value;
    for (size_t i = 0; i < cvector_size(bignums); ++i)
        value = bignums.data[i];
    cvector_ix_destroy(&bignums);
}
```
**CVector** of *CString*
```
#include <stc/cstring.h>
#include <stc/cvector.h>
declare_CVector_string(cs);

int main() {
    CVector_cs names = cvector_init;
    cvector_cs_pushBack(&names, cstring_make("Mary"));
    cvector_cs_pushBack(&names, cstring_make("Joe"));
    cstring_assign(&names.data[1], cstring_make("Jake")); // replace Joe

    printf("%s\n", names.data[1].str); // Access the string char*
    cvector_cs_destroy(&names);
}
```
**CHash map** of *int -> int*
```
#include <stc/chash.h>
declare_CHash(ii, map, int, int);

int main() {
    CHash_ii nums = chash_init;
    chash_ii_put(&nums, 8, 64);
    chash_ii_put(&nums, 11, 121);

    printf("%d\n", chash_ii_get(nums, 8)->value);
    chash_ii_destroy(&nums);
}
```
**CHash set** of *CString*
```
#include <stc/cstring.h>
#include <stc/chash.h>
declare_CHash_string(s, set); // Shorthand macro for the general declare_CHash expansion.
// CString keys are managed internally, although CHash is ignorant of CString.

int main() {
    CHash_s words = chash_init;
    chash_s_put(&words, "Hello");
    chash_s_put(&words, "Groovy");
    chash_s_erase(&words, "Hello");

    // iterate the map:
    c_foreach (i, chash_s, words)
        printf("%s\n", i.item->key.str);
    chash_s_destroy(&words);
}
```
**CHash map** of *CString -> CString*. Temporary CString values are created by "make", and moved to the container
```
#include <stc/cstring.h>
#include <stc/chash.h>
declare_CHash_string(ss, map, CString, cstring_destroy); 

int main() {
    CHash_ss table = chash_init;
    chash_ss_put(&table, "Make", cstring_make("my"));
    chash_ss_put(&table, "Sunny", cstring_make("day"));
    printf("Sunny: %s\n", chash_ss_get(table, "Sunny")->value.str);
    chash_ss_erase(&table, "Make");

    printf("size %d\n", chash_size(table));
    chash_ss_destroy(&table); // frees key and value CStrings, and hash table (CVector).
}
```
**CList** of *int64_t*. Similar to c++ std::forward_list, but can do both pushFront() and pushBack().
```
    #include <stc/clist.h>
    #include <stc/crandom.h>
    declare_CList(i, int64_t);
 
    int main() {
        CList_i list = clist_init;
        int n;
        sfc64_t rng = sfc64_seed(1234);
        for (int i=0; i<1000000; ++i) // one million random numbers
            clist_i_pushBack(&list, sfc64_rand(&rng));
        n = 0; 
        c_foreach (i, clist_i, list)
            if (++n % 10000 == 0) printf("%d: %zd\n", n, i.item->value);
        // Sort them...
        clist_i_sort(&list); // mergesort O(n*log n)
        n = 0;
        c_foreach (i, clist_i, list)
            if (++n % 10000 == 0) printf("%d: %zd\n", n, i.item->value);
        clist_i_destroy(&list);
    }
```
**CArray**
```
#include <stc/carray.h>
declare_CArray(f, float);

int main()
{
    CArray3f a3 = carray3f_make(30, 20, 10);
    carray3f_data(a3, 5, 4)[3] = 10.2f;  // a3[5][4][3]
    CArray2f a2 = carray3f_at(a3, 5);    // sub-array reference (no data copy).

    printf("%f\n", carray2f_value(a2, 4, 3));   // readonly lookup a2[4][3] (=10.2f)
    printf("%f\n", carray2f_data(a2, 4)[3]);    // same, but this is writable.
    printf("%f\n", carray2f_at(a2, 4).data[3]); // same, via sub-array access.
    
    printf("%f\n", carray3f_value(a3, 5, 4, 3)); // same data location, via a3 array.
    printf("%f\n", carray3f_data(a3, 5, 4)[3]);
    printf("%f\n", carray3f_at2(a3, 5, 4).data[3]);
    
    carray_destroy(a2); // does nothing, since it is a sub-array.
    carray_destroy(a3); // also invalidates a2.
}
```
