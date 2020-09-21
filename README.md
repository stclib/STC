STC - C99 Standard Container library
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
- **stc/cstack.h** - **stack** adapter, currently works with *cvec*.
- **stc/cqueue.h** - **queue** adapter, currently works with *clist*.
- **stc/cpqueue.h** - **priority queue** adapter. Works with *cvec*.
- **stc/coption.h** - Implementation of a **getopt_long()**-like function, *coption_get()*, to parse command line arguments.
- **stc/crandom.h** - A few very efficent modern random number generators *pcg32* and my own *64-bit PRNG* inspired by *sfc64*. Both uniform and normal distributions.
- **stc/cdefs.h** - A common include file with a few general definitions.

The usage of the containers is vert similar to the C++ standard containers, so it should be easy if you are familiar with them.

All containers mentioned above, except cstr_t and cbitset_t are generic and typesafe (similar to templates in C++). No casting is used. A simple example:
```C
#include <stc/cvec.h>

using_cvec(i, int);

int main(void) {
    cvec_i vec = cvec_INIT;
    cvec_i_push_back(&vec, 42);
    cvec_i_del(&vec);
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
```C
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

The library is very efficent. Containers have templated intrusive elements. One of the most performance critical containers is the **cmap / cset**. Luckily, cmap is among the fastest C/C++ map implementations available, see **examples/benchmark.c**

Compiled with clang.exe -O3 -x c++, v10.0 on windows, Ryzen 7 2700X CPU. Similar results with VC and g++.

- **CMAP** = stc/*cmap*
- KMAP = klib/*khash*
- UMAP = *std::unordered_map*
- SMAP = *spp::sparse_hash_map*
- BMAP = *ska::bytell_hash_map*
- FMAP = *ska::flat_hash_map*
- RMAP = *robin_hood::unordered_map*
- HMAP = *tsl::hopscotch_map*
```
Random keys are in range [0, 2^24), seed = 1600713678:

Unordered maps: 30000000 repeats of Insert random key + try to remove a random key:
CMAP: time:  4.44, sum: 450000015000000, erased 10921039
UMAP: time: 11.40, sum: 450000015000000, erased 10921039
SMAP: time: 14.27, sum: 450000015000000, erased 10921039
BMAP: time:  5.36, sum: 450000015000000, erased 10921039
FMAP: time:  4.40, sum: 450000015000000, erased 10921039
HMAP: time:  4.48, sum: 450000015000000, erased 10921039

Unordered maps: Insert 30000000 index keys, then remove them in same order:
CMAP: time:  3.29, erased 30000000
UMAP: time: 15.44, erased 30000000
SMAP: time: 22.21, erased 30000000
BMAP: time:  7.49, erased 30000000
FMAP: time:  5.71, erased 30000000
HMAP: time:  5.94, erased 30000000

Unordered maps: Insert 30000000 random keys, then remove them in same order:
CMAP: time:  3.19, erased 13970961
UMAP: time: 10.07, erased 13970961
SMAP: time: 13.41, erased 13970961
BMAP: time:  5.44, erased 13970961
FMAP: time:  4.45, erased 13970961
HMAP: time:  4.48, erased 13970961

Unordered maps: Iterate 30000000 random keys:
CMAP: time:  0.23, sum 1344646322743375
UMAP: time:  3.35, sum 1344646322743375
SMAP: time:  0.33, sum 1344646322743375
BMAP: time:  0.61, sum 1344646322743375
FMAP: time:  0.57, sum 1344646322743375
HMAP: time:  0.51, sum 1344646322743375
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

**cmap/cset** uses open hashing and is among the fastest hash-tables for C and C++. The default max load-factor is 0.85.

You can customize the destroy-, hash-, equals- functions, but also define a convertion from a raw/literal type to the key-type specified. This is very useful when e.g. having cstr as key, and therefore a few using-macros are pre-defined
for cmaps with cstr_t keys and/or values:

- *using_cmap_strkey(tag, valuetype)*
- *using_cmap_strval(tag, keytype)*
- *using_cmap_str()* // cstr_t -> cstr_t
- *using_cset_str()* // cstr_t set

To customize your own cmap type to work like these, you may want to look at **examples/advanced.c**. It demonstrates how to use a custom struct as a hash map key, by using the optional parameters to using_cmap().

Example usages
--------------
The examples folder contains further examples.

**cstr** string example.
```C
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
    
    c_del(cstr, &s1, &full_path);
}
```
**cvec** of *int64_t*. 
```C
#include <stc/cvec.h>

using_cvec(ix, int64_t); // ix is just an example type tag name.

int main() {
    cvec_ix bignums = cvec_INIT; // use cvec_ix_init() if initializing after declaration.
    cvec_ix_reserve(&bignums, 100);
    c_forrange (i, 100)
        cvec_ix_push_back(&bignums, i * i * i);
    cvec_ix_pop_back(&bignums); // erase the last

    uint64_t value;
    c_forrange (i, cvec_ix_size(bignums))
        value = bignums.data[i];
    cvec_ix_del(&bignums);
}
```
**cvec** of *cstr_t*.
```C
#include <stc/cstr.h>
#include <stc/cvec.h>

using_cvec_str();

int main() {
    cvec_str names = cvec_INIT;
    cvec_str_emplace_back(&names, "Mary");
    cvec_str_emplace_back(&names, "Joe");
    cstr_assign(&names.data[1], "Jake"); // replace "Joe".

    // Use push_back() rather than emplace_back() when adding a cstr_t type:
    cstr_t tmp = cstr_from("%d elements so far", cvec_str_size(names));
    cvec_str_push_back(&names, tmp); // tmp is moved to names, do not del() it.

    printf("%s\n", names.data[1].str); // Access the string char*
    c_foreach (i, cvec_str, names)
        printf("item: %s\n", i.get->str);
    cvec_str_del(&names);
}
```
**cmap** of *int => int*, and **cmap** of *cstr_t* => *cstr_t*
```C
#include <stdio.h>
#include <stc/cmap.h>

using_cmap(ii, int, int);
using_cmap_str(); 

int main() {
    // -- map of ints --
    cmap_ii nums = cmap_INIT;
    cmap_ii_put(&nums, 8, 64); // similar to insert_or_assign()
    cmap_ii_emplace(&nums, 11, 121); 

    printf("%d\n", cmap_ii_find(nums, 8)->second);
    cmap_ii_del(&nums);
    
    // -- map of str --
    cmap_str strings = cmap_INIT;
    cmap_str_emplace(&strings, "Make", "my");
    cmap_str_emplace(&strings, "Rainy", "day");
    cmap_str_emplace(&strings, "Sunny", "afternoon");
    c_push_items(&strings, cmap_str, {{"Eleven", "XI"}, {"Six", "VI"}});

    printf("size = %zu\n", cmap_str_size(table));
    c_foreach (i, cmap_str, strings)
        printf("%s: %s\n", i.get->first.str, i.get->second.str);
    cmap_str_del(&strings); // frees all strings and map.    
}
```
**cset** of *cstr*.
```C
#include <stc/cstr.h>
#include <stc/cmap.h>

using_cset_str(); // cstr set. See the discussion above.

int main() {
    cset_str words = cset_INIT;
    cset_str_insert(&words, "Hello");
    cset_str_insert(&words, "Sad");
    cset_str_insert(&words, "World");
    
    cset_str_erase(&words, "Sad");

    // iterate the set of cstr_t values:
    c_foreach (i, cset_str, words)
        printf("%s\n", i.get->str);
    cset_str_del(&words);
}
```
**clist** of *int64_t*. Similar to c++ *std::forward_list*, but can do both *push_front()* and *push_back()* as well as *pop_front()*.
```C
#include <stdio.h>
#include <stc/clist.h>

using_clist(fx, double);

int main() {
    clist_fx list = clist_INIT;
    c_push_items(&list, clist_fx, {
        10.0, 20.0, 30.0, 40.0, 50.0, 60.0, 70.0, 80.0, 90.0
    });
    
    c_forrange (i, int, 5, 95, 10)
        clist_fx_push_back(&list, (float) i);
    clist_fx_push_front(&list, 100.0);
    
    printf("current: ");
    c_foreach (i, clist_fx, list)
        printf(" %g", *i.get);
    puts("");
    
    clist_fx_sort(&list); // mergesort O(n*log n)

    printf("sorted: ");
    c_foreach (i, clist_fx, list)
        printf(" %g", *i.get);
    puts("");

    clist_fx_del(&list);
}
```
**cpqueue** priority queue demo:
```C
#include <stdio.h>
#include <time.h>
#include <stc/cpqueue.h>
#include <stc/crandom.h>

using_cvec(i, int64_t);
using_cpqueue(i, cvec_i, >); // adaptor type, '>' = min-heap

int main()
{
    size_t N = 10000000;
    crand_rng64_t rng = crand_rng64_init(time(NULL));
    crand_uniform_i64_t dist = crand_uniform_i64_init(0, N * 10);
    
    cpqueue_i heap = cpqueue_i_init();
    // Push ten million random numbers to priority queue, plus some negative ones.
    c_forrange (N)
        cpqueue_i_push(&heap, crand_uniform_i64(&rng, &dist));
    c_push_items(&heap, cpqueue_i, {-231, -32, -873, -4, -343});

    // Extract and disply the fifty smallest.
    c_forrange (50) {
        printf("%zd ", *cpqueue_i_top(&heap));
        cpqueue_i_pop(&heap);
    }
    cpqueue_i_del(&heap);
}
```
**carray**. 1d, 2d and 3d arrays, allocated from heap in one memory block. *carray3* may have sub-array "views" of *carray2* and *carray1* etc., as shown in the following example:
```C
#include <stdio.h>
#include <stc/carray.h>

using_carray(f, float);

int main()
{
    carray3f a3 = carray3f_make(30, 20, 10, 0.0f);  // define a3[30][20][10], init with 0.0f.
    *carray3f_at(a3, 5, 4, 3) = 3.14f;         // a3[5][4][3] = 3.14

    carray1f a1 = carray3f_at2(a3, 5, 4);      // sub-array a3[5][4] (no data copy).    
    carray2f a2 = carray3f_at1(a3, 5);         // sub-array a3[5]
    
    printf("%f\n", *carray1f_at(a1, 3));       // a1[3] (3.14f)
    printf("%f\n", *carray2f_at(a2, 4, 3));    // a2[4][3] (3.14f)
    printf("%f\n", *carray3f_at(a3, 5, 4, 3)); // a3[5][4][3] (3.14f)
    // ...
    carray1f_del(&a1); // does nothing, since it is a sub-array.
    carray2f_del(&a2); // same.
    carray3f_del(&a3); // free array, and invalidates a1, a2.
}
```
Finally, a demo of **cmap**, **cvec**, **cstr** and **random**: Normal distribution with a random mean and standard deviation, which may produce this:
```
  55 *
  56 **
  57 ****
  58 ******
  59 *********
  60 **************
  61 ********************
  62 ***************************
  63 ************************************
  64 ********************************************
  65 *****************************************************
  66 *************************************************************
  67 ********************************************************************
  68 ************************************************************************
  69 *************************************************************************
  70 ************************************************************************
  71 *******************************************************************
  72 *************************************************************
  73 *****************************************************
  74 ********************************************
  75 ***********************************
  76 ***************************
  77 ********************
  78 **************
  79 **********
  80 ******
  81 ****
  82 **
  83 *
Normal distribution with mean=69, stddev=5. '*' = 10811 samples out of 10000000.
```
Code:
```C
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stc/cmap.h>
#include <stc/cvec.h>
#include <stc/cstr.h>
#include <stc/crandom.h>

using_cmap(ii, int, int);

static int compare(cmap_ii_value_t *a, cmap_ii_value_t *b)
{
    return c_default_compare(&a->first, &b->first);
}
using_cvec(mi, cmap_ii_value_t, c_default_del, compare);

cvec_mi make_normal_dist(crand_rng64_t* rng, crand_normal_f64_t* dist, int n);
void display_hist(cvec_mi hist, int mean, int stddev, int n, int scale);


int main(int argc, char* argv[])
{
    // Seed with a real random value, if available
    crand_rng64_t rng = crand_rng64_init(time(NULL));

    // Choose random mean and standard deviation
    crand_uniform_i64_t mean_dist = crand_uniform_i64_init(-99, 99),
                        sdev_dist = crand_uniform_i64_init(5, 12);

    int mean = (int) crand_uniform_i64(&rng, &mean_dist);
    int stddev = (int) crand_uniform_i64(&rng, &sdev_dist);

    // Setup a normal distribution:
    crand_normal_f64_t norm_dist = crand_normal_f64_init(mean, stddev);

    int samples = 10000000, scale = 74;
    cvec_mi hist = make_normal_dist(&rng, &norm_dist, samples);
    display_hist(hist, mean, stddev, samples, scale);

    cvec_mi_del(&hist);
}

cvec_mi make_normal_dist(crand_rng64_t* rng, crand_normal_f64_t* dist, int n)
{
    cmap_ii mhist = cmap_INIT;
    c_forrange (i, int, n) {
        cmap_ii_emplace(&mhist, (int) round(crand_normal_f64(rng, dist)), 0).first->second += 1;
    }

    // Transfer map to vec, sort and return it.
    cvec_mi hist = cvec_INIT;
    c_foreach (i, cmap_ii, mhist) {
        cvec_mi_push_back(&hist, *i.get);
    }
    cvec_mi_sort(&hist);
    cmap_ii_del(&mhist);
    return hist;
}

void display_hist(cvec_mi hist, int mean, int stddev, int n, int scale)
{
    cstr_t bar = cstr_INIT;
    c_foreach (i, cvec_mi, hist) {
        int k = (int) (i.get->second * stddev * scale * 25ull / 10 / n);
        if (k > 0) {
            cstr_take(&bar, cstr_with_size(k, '*'));
            printf("%4d %s\n", i.get->first, bar.str);
        }
    }
    printf("Normal distribution with mean=%d, stddev=%d. '*' = %.0f samples out of %d.\n",
           mean, stddev, n / (2.5 * stddev * scale), n);
    cstr_del(&bar);
}
```
