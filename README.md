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
- **stc/cptr.h** - Support for pointers in containers, and a reference counted shared pointer **csptr**.
- **stc/coption.h** - Implementation of a **getopt_long()**-like function, *coption_get()*, to parse command line arguments.
- **stc/crandom.h** - A few very efficent modern random number generators *pcg32* and my own *64-bit PRNG* inspired by *sfc64*. Both uniform and normal distributions.
- **stc/ccommon.h** - A common include file with a few general definitions.

The usage of the containers is vert similar to the C++ standard containers, so it should be easy if you are familiar with them.

All containers mentioned above, except cstr_t and cbitset_t, are generic and therefore typesafe (similar to templates in C++). No casting is used. A simple example:
```C
#include <stc/cvec.h>

using_cvec(i, int);

int main(void) {
    cvec_i vec = cvec_i_init();
    cvec_i_push_back(&vec, 1);
    cvec_i_push_back(&vec, 2);
    c_foreach (i, cvec_i, vec) 
        printf(" %d", *i.val);
    cvec_i_del(&vec);
}
```
Using containers with complex element types is simple:
```C
#include <stc/cstr.h>
#include <stc/cvec.h>

typedef struct { 
    cstr_t name; // dynamic string
    int id;
} User;

void User_del(User* u)
    { cstr_del(&u->name); }
int User_compare(User* u, User* v) 
    { int c = strcmp(u->name.str, v->name.str); return c != 0 ? c : u->id - v->id; }

using_cvec(u, User, User_del, User_compare);

int main(void) {
    cvec_u vec = cvec_u_init();
    cvec_u_push_back(&vec, (User) {cstr_from("admin"), 0}); // cstr_from() allocates string memory
    cvec_u_push_back(&vec, (User) {cstr_from("usera"), 1});
    c_foreach (i, cvec_u, vec) 
        printf("%s: %d\n", i.val->name.str, i.val->id);
    cvec_u_del(&vec); // free everything
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

Because it is headers only, files can simply be included in your program. The functions will be inlined by default. You may add the project folder to CPATH environment variable, to let gcc, clang, and tinyc locate the headers.

If containers are extensively used accross many translation units with common instantiated container types, it is recommended to build as a library, to minimize executable size. To enable this mode, specify **-DSTC_HEADER** as compiler option, and put all the instantiations of the containers used in one C file, like this:
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
From these tests *cmap*, *robin_hood* and *khash* are almost equally fast. std::unordered_map is horrible. With random numbers in 0 - 2^20 range, khash performs worse, though:
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

**cvec** of *int64_t*.
```C
#include <stc/cvec.h>
#include <stdio.h>

using_cvec(ix, int64_t); // ix is just an example type tag name.

int main() {
    cvec_ix bignums = cvec_ix_init(); // use cvec_ix_init() if initializing after declaration.
    cvec_ix_reserve(&bignums, 50);

    c_forrange (i, int, 1, 50)
        cvec_ix_push_back(&bignums, i * i * i);

    cvec_ix_pop_back(&bignums); // erase the last

    c_forrange (i, int, 1, cvec_ix_size(bignums))
        bignums.data[i] /= i; // make them smaller

    c_foreach (i, cvec_ix, bignums)
        printf(" %d", *i.val);
    cvec_ix_del(&bignums);
}
// Output:
 1 8 13 21 31 43 57 73 91 111 133 157 183 211 241 273 307 343 381 421 463 507 553 601 651 703 757 813 871 931 993 1057 1123 1191 1261 1333 1407 1483 1561 1641 1723 1807 1893 1981 2071 2163 2257 2353
```
**cvec** of *cstr_t*.
```C
#include <stc/cstr.h>
#include <stc/cvec.h>

using_cvec_str();

int main() {
    cvec_str names = cvec_str_init();
    cvec_str_emplace_back(&names, "Mary");
    cvec_str_emplace_back(&names, "Joe");
    cstr_assign(&names.data[1], "Jake"); // replace "Joe".

    // Use push_back() rather than emplace_back() when adding a cstr_t type:
    cstr_t tmp = cstr_from_fmt("%d elements so far", cvec_str_size(names));
    cvec_str_push_back(&names, tmp); // tmp is moved to names, do not del() it.

    printf("%s\n", names.data[1].str); // Access the second element

    c_foreach (i, cvec_str, names)
        printf("item: %s\n", i.val->str);
    cvec_str_del(&names);
}
// Output:
Jake
item: Mary
item: Jake
item: 2 elements so far
```
**cstr** string example.
```C
#include <stc/cstr.h>

int main() {
    cstr_t s1 = cstr_from("one-nine-three-seven-five");
    printf("%s.\n", s1.str);

    cstr_insert(&s1, 3, "-two");
    printf("%s.\n", s1.str);

    cstr_erase(&s1, 7, 5); // -nine
    printf("%s.\n", s1.str);

    cstr_replace(&s1, cstr_find(&s1, "seven"), 5, "four");
    printf("%s.\n", s1.str);

    // reassign:
    cstr_assign(&s1, "one two three four five six seven");
    cstr_append(&s1, " eight");
    printf("append: %s\n", s1.str);

    cstr_t full_path = cstr_from_fmt("%s/%s.%s", "directory", "filename", "ext");
    printf("%s\n", full_path.str);

    c_del(cstr, &s1, &full_path);
}
// Output:
one-nine-three-seven-five.
one-two-nine-three-seven-five.
one-two-three-seven-five.
one-two-three-four-five.
append: one two three four five six seven eight
directory/filename.ext
```
**cmap** of *int => int*, and **cmap** of *cstr_t* => *cstr_t*
```C
#include <stdio.h>
#include <stc/cmap.h>
#include <stc/cstr.h>

using_cmap(ii, int, int);
using_cmap_str();

int main() {
    // -- map of ints --
    cmap_ii nums = cmap_ii_init();
    cmap_ii_put(&nums, 8, 64); // similar to insert_or_assign()
    cmap_ii_emplace(&nums, 11, 121);

    printf("%d\n", cmap_ii_find(&nums, 8)->second);
    cmap_ii_del(&nums);

    // -- map of str --
    cmap_str strings = cmap_str_init();
    cmap_str_emplace(&strings, "Make", "my");
    cmap_str_emplace(&strings, "Rainy", "day");
    cmap_str_emplace(&strings, "Sunny", "afternoon");
    c_push_items(&strings, cmap_str, { {"Eleven", "XI"}, {"Six", "VI"} });

    printf("size = %zu\n", cmap_str_size(strings));
    c_foreach (i, cmap_str, strings)
        printf("%s: %s\n", i.val->first.str, i.val->second.str);
    cmap_str_del(&strings); // frees all strings and map.
}
// Output:
64
size = 5
Rainy: day
Sunny: afternoon
Six: VI
Make: my
Eleven: XI
```
**cset** of *cstr*.
```C
#include <stc/cstr.h>
#include <stc/cmap.h>

using_cset_str(); // cstr set. See the discussion above.

int main() {
    cset_str words = cset_str_init();
    cset_str_insert(&words, "Hello");
    cset_str_insert(&words, "Sad");
    cset_str_insert(&words, "World");

    cset_str_erase(&words, "Sad");

    // iterate the set of cstr_t values:
    c_foreach (i, cset_str, words)
        printf("%s ", i.val->str);
    cset_str_del(&words);
}
// Output:
Hello World
```
**clist** of *int64_t*. Similar to c++ *std::forward_list*, but can do both *push_front()* and *push_back()* as well as *pop_front()*.
```C
#include <stdio.h>
#include <stc/clist.h>

using_clist(fx, double);

int main() {
    clist_fx list = clist_fx_init();
    c_push_items(&list, clist_fx, {
        10.0, 20.0, 30.0, 40.0, 50.0, 60.0, 70.0, 80.0, 90.0
    });
    // interleave push_front / push_back:
    c_forrange (i, int, 1, 10) {
        if (i & 1) clist_fx_push_front(&list, (float) i);
        else       clist_fx_push_back(&list, (float) i);
    }

    printf("initial: ");
    c_foreach (i, clist_fx, list)
        printf(" %g", *i.val);

    clist_fx_sort(&list); // mergesort O(n*log n)

    printf("\nsorted: ");
    c_foreach (i, clist_fx, list)
        printf(" %g", *i.val);

    clist_fx_del(&list);
}
// Output:
initial:  9 7 5 3 1 10 20 30 40 50 60 70 80 90 2 4 6 8
sorted:  1 2 3 4 5 6 7 8 9 10 20 30 40 50 60 70 80 90
```
**cpqueue** priority queue demo:
```C
#include <stdio.h>
#include <stc/cpqueue.h>
#include <stc/crandom.h>

using_cvec(i, int64_t);
using_cpqueue(i, cvec_i, >); // adaptor type, '>' = min-heap

int main()
{
    size_t N = 10000000;
    crand_rng64_t rng = crand_rng64_init(1234);
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
// Output:
 -873 -343 -231 -32 -4 3 5 6 18 23 31 54 68 87 99 105 107 125 128 147 150 155 167 178 181 188 213 216 272 284 287 302 306 311 313 326 329 331 344 348 363 367 374 385 396 399 401 407 412 477
```
**cqueue** adapter container. Uses singly linked list as representation.
```C
#include <stc/cqueue.h>
#include <stdio.h>

using_clist(i, int);
using_cqueue(i, clist_i);

int main() {
    cqueue_i queue = cqueue_i_init();

    // push() and pop() a few.
    c_forrange (i, 20)
        cqueue_i_push(&queue, i);

    c_forrange (5)
        cqueue_i_pop(&queue);

    c_foreach (i, cqueue_i, queue)
        printf(" %d", *i.val);

    cqueue_i_del(&queue);
}
// Output:
 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19
```
**carray**. 1d, 2d and 3d arrays, allocated from heap in one memory block. *carray3* may have sub-array "views" of *carray2* and *carray1* etc., as shown in the following example:
```C
#include <stdio.h>
#include <stc/carray.h>

using_carray(f, float);

int main()
{
    carray3f a3 = carray3f_make(30, 20, 10, 0.0f);  // define a3[30][20][10], init with 0.0f.
    *carray3f_at(&a3, 5, 4, 3) = 3.14f;         // a3[5][4][3] = 3.14

    carray1f a1 = carray3f_at2(&a3, 5, 4);      // sub-array a3[5][4] (no data copy).
    carray2f a2 = carray3f_at1(&a3, 5);         // sub-array a3[5]

    printf("%f\n", *carray1f_at(&a1, 3));       // a1[3] (3.14f)
    printf("%f\n", *carray2f_at(&a2, 4, 3));    // a2[4][3] (3.14f)
    printf("%f\n", *carray3f_at(&a3, 5, 4, 3)); // a3[5][4][3] (3.14f)
    // ...
    carray1f_del(&a1); // does nothing, since it is a sub-array.
    carray2f_del(&a2); // same.
    carray3f_del(&a3); // free array, and invalidates a1, a2.
}
// Output:
3.140000
3.140000
3.140000
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
void display_hist(cvec_mi hist, int scale, int mean, int stddev);


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
    display_hist(hist, scale, mean, stddev);

    cvec_mi_del(&hist);
}

cvec_mi make_normal_dist(crand_rng64_t* rng, crand_normal_f64_t* dist, int n)
{
    cmap_ii mhist = cmap_ii_init();
    c_forrange (n) {
        cmap_ii_emplace(&mhist, (int) round(crand_normal_f64(rng, dist)), 0).first->second += 1;
    }

    // Transfer cmap entries to a cvec, sort and return it.
    cvec_mi hist = cvec_mi_init();
    c_foreach (i, cmap_ii, mhist) {
        cvec_mi_push_back(&hist, *i.val);
    }
    cvec_mi_sort(&hist);
    cmap_ii_del(&mhist);
    return hist;
}

void display_hist(cvec_mi hist, int scale, int mean, int stddev)
{
    cstr_t bar = cstr_init();
    int n = 0; // samples
    c_foreach (i, cvec_mi, hist)
        n += i.val->second;
    c_foreach (i, cvec_mi, hist) {
        int k = (int) (i.val->second * stddev * scale * 25ull / 10 / n);
        if (k > 0) {
            cstr_take(&bar, cstr_with_size(k, '*'));
            printf("%4d %s\n", i.val->first, bar.str);
        }
    }
    printf("Normal distribution with mean=%d, stddev=%d. '*' = %.0f samples out of %d.\n",
           mean, stddev, n / (2.5 * stddev * scale), n);
    cstr_del(&bar);
}
```
