STC - C99 Standard Container Library
====================================

Introduction
------------

An elegant, modern, generic, customizable, typesafe, consistent, user-friendly, and very efficient standard container library for C99. This is a small headers only library with the most used container components, and a few algorithms:
- **cstring.h** - Compact and powerful **string** class.
- **cvector.h** - Dynamic generic **vector** class.
- **chash.h** - Unordered **map** and **set**.
- **carray.h** - Multi-dimensional dynamic **array**
- **clist.h** - A circular singly linked **list**, suited to be used as **queue**. Supports *pushBack, pushFront, and popFront*, as well as *splice* functions and (merge) *sorting*.
- **coption.h** - Header-only implementation of **getopt_long**-like function, to parse command line arguments.
- **crandom.h** - Header-only collection of efficent modern random number generators **xoroshiro128ss**, **sfc32/64** and Mersenne Twister **mt19937**. It also implements the crypto-strong **siphash** algorithm.

The usage of containers is similar to c++ standard containers, so it should be easy for those who are familiar with that.

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
Installation
------------

Because it is headers only, files can simply be included in your program. The functions will be inlined by default. If containers are extensively used accross many files with the same instantiated type, it is recommended to build as a library to minimize executable size. In this case, specify **-DSTC_HEADER** to the compiler, and put all the instantiations of the containers used in one C file, e.g.
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

The library is very efficent. The containers have templated "intrusive"/in-place elements. Possibly the most speed critical is the **CHash map / CHash set** implementation. This is among the fastest of C and C++ map implementations: benchmark.c compiled with g++ v9.2.0 -O3 on windows (results are similar with Visual Studio or g++ on linux):

**CMAP=this**, KMAP=khash, UMAP=std::unordered_map, BMAP=ska::bytell_hash_map, FMAP=ska::flat_hash_map, RMAP=robin_hood::unordered_map
```
Random keys are in range [0, 2^20):
map<uint64_t, uint64_t>: 7000000 repeats of Insert random key + (try to) remove a different random key:
CMAP(ii): sz: 523938, bucks: 1013337, time: 0.39, sum: 24500003500000, erase: 3237392 (fastest)
KMAP(ii): sz: 523938, bucks: 2097152, time: 0.46, sum: 24500003500000, erase: 3237392
UMAP(ii): sz: 523938, bucks: 1056323, time: 2.21, sum: 24500003500000, erase: 3237392
BMAP(ii): sz: 523938, bucks: 1048576, time: 0.46, sum: 24500003500000, erase: 3237392
FMAP(ii): sz: 523938, bucks: 1048576, time: 0.43, sum: 24500003500000, erase: 3237392
RMAP(ii): sz: 523938, bucks: 838860, time: 0.82, sum: 24500003500000, erase: 3237392

map<uint64_t, uint64_t>: Insert 10000000 sequensial keys, then remove them in same order:
CMAP(ii): sz: 0, bucks: 17001171, time: 0.75, erase 10000000 (second)
KMAP(ii): sz: 0, bucks: 16777216, time: 0.48, erase 10000000
UMAP(ii): sz: 0, bucks: 17961079, time: 1.04, erase 10000000
BMAP(ii): sz: 0, bucks: 16777216, time: 1.04, erase 10000000
FMAP(ii): sz: 0, bucks: 16777216, time: 0.94, erase 10000000
RMAP(ii): sz: 0, bucks: 13421772, time: 0.84, erase 10000000

map<uint64_t, uint64_t>: Insert 10000000 random keys, then remove them in same order:
CMAP(ii): sz: 0, bucks: 1621347, time: 0.41, erase 1048490 (fastest)
KMAP(ii): sz: 0, bucks: 2097152, time: 0.77, erase 1048490
UMAP(ii): sz: 0, bucks: 2144977, time: 1.67, erase 1048490
BMAP(ii): sz: 0, bucks: 2097152, time: 0.52, erase 1048490
FMAP(ii): sz: 0, bucks: 2097152, time: 0.44, erase 1048490
RMAP(ii): sz: 0, bucks: 1677721, time: 0.65, erase 1048490
```
Memory efficiency
-----------------

Near optimal memory usage for all the containers. The circular list is intrusive so only one allocation is needed for each node, however a custom allocator and various techniques could improve the linked lists memory usage.
- **CString**, **CVector**: one pointer for representation. Heap allocation holds size and capacity.
- **CList**: one pointer for representation. Each node allocates block storing value and next pointer.
- **CHash set**: Representation size of 4 pointers. One array of Key per bucket, one array of one byte per buckets.
- **CHash map**: Representation size of 4 pointers. One array of (Key, Value) per bucket, one array of one byte per buckets.

Usage by examples
-----------------

**CString** demo
```
#include <stc/cstring.h>

int main() {
    CString cs = cstring_make("one-nine-three-seven-five");
    printf("%s.\n", cs.str);

    cstring_insert(&cs, 3, "-two");
    printf("%s.\n", cs.str);

    cstring_erase(&cs, 7, 5); // -nine
    printf("%s.\n", cs.str);

    cstring_replace(&cs, 0, "seven", "four");
    printf("%s.\n", cs.str);
    printf("find: %s\n", cs.str + cstring_find(cs, 0, "four"));

    // reassign:
    cstring_assign(&cs, "one two three four five six seven");
    cstring_append(&cs, " eight");
    printf("append: %s\n", cs.str);

    cstring_destroy(&cs);
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
