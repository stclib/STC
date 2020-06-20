STC - C99 Standard Container Library
====================================

Introduction
------------

An elegant, modern, generic, customizable, typesafe, and very efficient standard container library for C99. This is a small headers only library with the most used container components: **cstring**, **cvector**, **chash**, **carray**, and **clist**.

The usage is quite similar to c++ standard containers, so it should be easy for those who are familiar with that.

All containers mentioned above, except for CString are generic (similar to templates in C++). The typical usage is:
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

Because it is headers only, files can simply be included in your program. The functions will be inlined by default. If containers are extensively used accross many files with the same instantiated type, it is recommended to build as a library to minimize executable size. In this case, specify -DSTC_HEADER to the compiler, and put all the instantiations of the containers used in one C file, e.g.
```
#define STC_IMPLEMENTATION
#include <stc/cvector.h>
#include <stc/chash.h>

declare_CVector(i, int);
declare_CHash(ii, map, int, int);
declare_CHash(64, set, int64_t);
...
```

Usage by examples
-----------------

CString demo
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
Simple CVector of 64bit int
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
CVector of CString
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
CHash map of int -> int
```
#include <stc/cmap.h>
declare_CHash(ii, map, int, int);

int main() {
    CHash_ii nums = cmap_init;
    chash_ii_put(&nums, 8, 64);
    chash_ii_put(&nums, 11, 121);

    printf("%d\n", chash_ii_get(nums, 8)->value);
    chash_ii_destroy(&nums);
}
```
CHash set of CString
```
#include <stc/cstring.h>
#include <stc/cmap.h>
declare_CHash_string(s, set); // Shorthand macro for the general declare_CHash expansion.
// CString keys are managed internally, although CHash is ignorant of CString.

int main() {
    CHash_s words = cmap_init;
    chash_s_put(&words, "Hello");
    chash_s_put(&words, "Groovy");
    chash_s_erase(&words, "Hello");

    // iterate the map:
    c_foreach (i, chash_s, words)
        printf("%s\n", i.item->key.str);
    chash_s_destroy(&words);
}
```
CHash map of CString -> CString. Temporary CString values are created by "make", and moved to the container
```
#include <stc/cstring.h>
#include <stc/cmap.h>
declare_CHash_string(ss, map, CString, cstring_destroy); 

int main() {
    CHash_ss table = cmap_init;
    chash_ss_put(&table, "Make", cstring_make("my"));
    chash_ss_put(&table, "Sunny", cstring_make("day"));
    printf("Sunny: %s\n", chash_ss_get(table, "Sunny")->value.str);
    chash_ss_erase(&table, "Make");

    printf("size %d\n", chash_size(table));
    chash_ss_destroy(&table); // frees key and value CStrings, and hash table (CVector).
}
```
