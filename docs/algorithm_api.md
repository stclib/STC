# STC Algorithms

"No raw loops" - Sean Parent
## Ranged for-loops

### c_foreach, c_foreach_reverse, c_foreach_n, c_forpair
```c
#include "stc/common.h"
```

| Usage                                    | Description                               |
|:-----------------------------------------|:------------------------------------------|
| `c_foreach (it, ctype, container)`       | Iteratate all elements                    |
| `c_foreach (it, ctype, it1, it2)`        | Iterate the range [it1, it2)              |
| `c_foreach_reverse (it, ctype, container)`| Iteratate all elements in reverse. *vec, deq, queue, stack* only! |
| `c_foreach_reverse (it, ctype, it1, it2)`| Iteratate range [it1, it2) elements in reverse. |
| `c_foreach_n (it, ctype, cnt, n)`        | Iterate up to n times using it.index and it.n |
| `c_forpair (key, val, ctype, container)` | Iterate with structured binding           |

```c
#define i_TYPE IMap,int,int
#include "stc/smap.h"
// ...
IMap map = c_init(IMap, { {23,1}, {3,2}, {7,3}, {5,4}, {12,5} });

c_foreach (i, IMap, map)
    printf(" %d", i.ref->first);
// 3 5 7 12 23

// same, with raw for loop:
for (IMap_iter i = IMap_begin(&map); i.ref; IMap_next(&i))
    printf(" %d", i.ref->first);

// iterate from iter to end
IMap_iter iter = IMap_find(&map, 7);
c_foreach (i, IMap, iter, IMap_end(&map))
    printf(" %d", i.ref->first);
// 7 12 23

// iterate with "structured binding":
c_forpair (id, count, IMap, map)
    printf(" (%d %d)", *_.id, *_.count);

// iterate first 3 with an index count enumeration
c_foreach_n (i, IMap, map, 3)
    printf(" %zd:(%d %d)", i.index, i.ref->first, i.ref->second);
// 0:(3 2) 1:(5 4) 2:(7 3)

// iterate with an already declared iter (useful in coroutines)
c_foreach_it (iter, IMap, map)
    printf(" (%d %d)", iter.ref->first, iter.ref->second);
```

### c_foritems
Iterate compound literal array elements. In addition to `i.ref`, you can access `i.index` and `i.size`.
```c
// apply multiple push_backs
c_foritems (i, int, {4, 5, 6, 7})
    list_i_push_back(&lst, *i.ref);

// insert in existing map
c_foritems (i, hmap_ii_value, { {4, 5}, {6, 7} })
    hmap_ii_insert(&map, i.ref->first, i.ref->second);

// string literals pushed to a stack of cstr elements:
c_foritems (i, const char*, {"Hello", "crazy", "world"})
    stack_cstr_emplace(&stk, *i.ref);
```
---

## Integer range loops

### c_forrange
Abstraction for iterating sequence of integers. Like python's **for** *i* **in** *range()* loop.

| Usage                                | Python equivalent                    |
|:-------------------------------------|:-------------------------------------|
| `c_forrange (stop)`                  | `for _ in range(stop):`              |
| `c_forrange (i, stop)`               | `for i in range(stop):`              |
| `c_forrange (i, start, stop)`        | `for i in range(start, stop):`       |
| `c_forrange (i, start, stop, step)`  | `for i in range(start, stop, step):` |

```c
c_forrange (5) printf("x");
// xxxxx
c_forrange (i, 5) printf(" %lld", i);
// 0 1 2 3 4
c_forrange (i, -3, 3) printf(" %lld", i);
// -3 -2 -1 0 1 2
c_forrange (i, 30, 0, -5) printf(" %lld", i);
// 30 25 20 15 10 5
```

### crange: Integer range generator object
A number sequence generator type, similar to [boost::irange](https://www.boost.org/doc/libs/release/libs/range/doc/html/range/reference/ranges/irange.html).
```c
crange      crange_make(stop);              // will generate 0, 1, ..., stop-1
crange      crange_make(start, stop);       // will generate start, start+1, ... stop-1
crange      crange_make(start, stop, step); // will generate start, start+step, ... upto-not-including stop
                                            // step may be negative.
crange_iter crange_begin(crange* self);
void        crange_next(crange_iter* it);

crange&     c_iota(start);                  // returns an l-value; will generate start, start+1, ...
crange&     c_iota(start, stop);            // l-value, otherwise like crange_make(start, stop)
crange&     c_iota(start, stop, step);      // l-value, otherwise like crange_make(start, stop, step)
```
 The **crange_value** type is *intptr_t*. Variables *start*, *stop*, and *step* are of type *crange_value*. Note that **crange** does not have *_end()* and *_advance()* functions; e.g. will not compile when used with *c_filter_pairwise()*.
```c
// 1. All primes less than 32: See below for c_filter() and is_prime()
crange r1 = crange_make(3, 32, 2);
printf("2"); // first prime
c_filter(crange, r1
     , is_prime(*value)
    && printf(" %zi", *value)
);
// 2 3 5 7 11 13 17 19 23 29 31

// 2. The first 11 primes:
// c_iota() can be used as argument to c_filter.
printf("2");
c_filter(crange, c_iota(3)
     , is_prime(*value)
    && (printf(" %zi", *value), c_flt_take(10))
);
// 2 3 5 7 11 13 17 19 23 29 31
```

### c_filter, c_filter_zip, c_filter_pairwise, c_forfilter
Functional programming with chained `&&` filtering. `value` is the pointer to current value.
It enables a subset of functional programming like in other popular languages.

- **Note 1**: The **_reverse** variants only works with *vec, deq, stack, queue* containers.
- **Note 2**: There is also a `c_forfilter` loop variant of `c_filter`. It uses the filter namings
`c_fflt_skip(it, numItems)`, etc. See [filter.h](../include/stc/algo/filter.h).

| Usage                                | Description                       |
|:-------------------------------------|:----------------------------------|
| `c_filter(CType, container, filters)` | Filter items in chain with the && operator |
| `c_filter_from(CType, start, filters)` | Filter from start iterator |
| `c_filter_reverse(CType, cnt, filters)` | Filter items in reverse order  |
| `c_filter_reverse_from(CType, rstart, filters)` | Filter reverse from rstart iterator |
| *c_filter_zip*, *c_filter_pairwise*: ||
| `c_filter_zip(CType, cnt1, cnt2, filters)` | Filter (cnt1, cnt2) items |
| `c_filter_zip(CType1, cnt1, CType2, cnt2, filters)` | May use different types for cnt1, cnt2 |
| `c_filter_reverse_zip(CType, cnt1, cnt2, filters)` | Filter (cnt1, cnt2) items in reverse order  |
| `c_filter_reverse_zip(CType1, cnt1, CType2, cnt2, filters)` | May use different types for cnt1, cnt2 |
| `c_filter_pairwise(CType, cnt, filters)` | Filter items pairwise as value1, value2 |

| Built-in filter              | Description                                |
|:-----------------------------|:-------------------------------------------|
| `c_flt_skip(numItems)`       | Skip numItems (increments count)           |
| `c_flt_take(numItems)`       | Take numItems only (increments count)      |
| `c_flt_skipwhile(predicate)` | Skip items until predicate is false        |
| `c_flt_takewhile(predicate)` | Take items until predicate is false        |
| `c_flt_counter()`            | Increment count and return it              |
| `c_flt_getcount()`           | Number of items passed skip/take/counter   |
| `c_flt_map(expr)`            | Map expr to current value. Input unchanged |
| `c_flt_src`                  | Pointer variable to current unmapped source value |
| `value`                      | Pointer variable to (possible mapped) value |
| For *c_filter_zip*, *c_filter_pairwise*: ||
| `c_flt_map1(expr)`           | Map expr to value1. Input unchanged |
| `c_flt_map2(expr)`           | Map expr to value2. Input unchanged |
| `c_flt_src1, c_flt_src2`     | Pointer variables to current unmapped source values |
| `value1, value2`             | Pointer variables to (possible mapped) values |

[ [Run this example](https://godbolt.org/z/7dP5a1s4s) ]
```c
#include <stdio.h>
#define i_TYPE Vec, int
#include "stc/stack.h"
#include "stc/algorithm.h"

int main(void)
{
    Vec vec = c_init(Vec, {1, 2, 3, 4, 5, 6, 7, 8, 1, 2, 3, 9, 10, 11, 12, 5});

    c_filter(Vec, vec
         , c_flt_skipwhile(*value < 3)  // skip leading values < 3
        && (*value & 1) == 1            // then use odd values only
        && c_flt_map(*value * 2)        // multiply by 2
        && c_flt_takewhile(*value < 20) // stop if mapped *value >= 20
        && printf(" %d", *value)        // print value
    );
    //  6 10 14 2 6 18
    puts("");
    Vec_drop(&vec);
}
```

---
## Generic algorithms

### c_init, c_drop

- **c_init** - construct any container from an initializer list:
- **c_drop** - drop (destroy) multiple containers of the same type:
```c
#define i_key_cstr // owned cstr string value type
#include "stc/hset.h"

#define i_key int
#define i_val int
#include "stc/hmap.h"
...
// Initializes with const char*, internally converted to cstr!
hset_cstr myset = c_init(hset_cstr, {"This", "is", "the", "story"});

int x = 7, y = 8;
hmap_int mymap = c_init(hmap_int, { {1, 2}, {3, 4}, {5, 6}, {x, y} });
```
Drop multiple containers of the same type:
```c
c_drop(hset_cstr, &myset, &myset2);
```

### c_find_if, c_find_reverse_if
Find linearily in containers using a predicate. `value` is a pointer to each element in predicate.
***outiter_ptr*** must be defined prior to call.
- `c_find_if(CntType, cnt, outiter_ptr, pred)`.
- `c_find_if(CntType, cnt, OutCnt, outiter_ptr, pred)`
- `c_find_reverse_if(CntType, cnt, outiter_ptr, pred)`
- `c_find_reverse_if(CntType, cnt, OutCnt, outiter_ptr, pred)`

### c_copy, c_copy_reverse, c_copy_if, c_copy_reverse_if
Copy linearily in containers using a predicate. `value` is a pointer to each element in predicate.
- `c_copy(CntType, cnt, outcnt_ptr)`
- `c_copy(CntType, cnt, OutCnt, outcnt_ptr)`
- `c_copy_reverse(CntType, cnt, outcnt_ptr)`
- `c_copy_reverse(CntType, cnt, OutCnt, outcnt_ptr)`
- `c_copy_if(CntType, cnt, outcnt_ptr, pred)`
- `c_copy_if(CntType, cnt, OutCnt, outcnt_ptr, pred)`
- `c_copy_reverse_if(CntType, cnt, outcnt_ptr, pred)`
- `c_copy_reverse_if(CntType, cnt, OutCnt, outcnt_ptr, pred)`

### c_erase_if, c_eraseremove_if
Erase linearily in containers using a predicate. `value` is a pointer to each element in predicate.
- `c_erase_if(CntType, cnt_ptr, pred)`. Use with **list**, **hmap**, **hset**, **smap**, and **sset**.
- `c_eraseremove_if(CntType, cnt_ptr, pred)`. Use with **stack**, **vec**, **deq**, and **queue** only.
```c
#include <stdio.h>
#define i_static
#include "stc/cstr.h"
#include "stc/algorithm.h"

#define i_TYPE Vec,int
#define i_use_cmp
#include "stc/vec.h"

#define i_TYPE List,int
#define i_use_cmp
#include "stc/list.h"

#define i_type Map
#define i_key_cstr
#define i_val int
#include "stc/smap.h"

int main(void)
{
    // Clone all *value > 10 to outvec. Note: `value` is a pointer to current element
    Vec vec = c_init(Vec, {2, 30, 21, 5, 9, 11});
    Vec outvec = {0};

    c_copy_if(Vec, vec, &outvec, *value > 10);
    c_foreach (i, Vec, outvec) printf(" %d", *i.ref);
    puts("");

    // Search vec for first value > 20.
    Vec_iter result;

    c_find_if(Vec, vec, &result, *value > 20);
    if (result.ref) printf("found %d\n", *result.ref);

    // Erase values between 20 and 25 in vec:
    c_eraseremove_if(Vec, &vec, 20 < *value && *value < 25);
    c_foreach (i, Vec, vec) printf(" %d", *i.ref);
    puts("");

    // Erase all values > 20 in a linked list:
    List list = c_init(List, {2, 30, 21, 5, 9, 11});

    c_erase_if(List, &list, *value > 20);
    c_foreach (i, List, list) printf(" %d", *i.ref);
    puts("");

    // Search a sorted map for the first string containing "hello" from it1, and erase it:
    Map map = c_init(Map, { {"yes",1}, {"no",2}, {"say hello from me",3}, {"goodbye",4} });
    Map_iter res, it1 = Map_begin(&map);

    c_find_from(Map, it1, &res, cstr_contains(&value->first, "hello"));
    if (res.ref) Map_erase_at(&map, res);

    // Erase all strings containing "good" in the sorted map:
    c_erase_if(Map, &map, cstr_contains(&value->first, "good"));
    c_foreach (i, Map, map) printf("%s, ", cstr_str(&i.ref->first));

    c_drop(Vec, &vec, &outvec);
    List_drop(&list);
    Map_drop(&map);
}
```

### c_all_of, c_any_of, c_none_of
Test a container/range using a predicate. ***result*** is output and must be declared prior to call.
- `void c_all_of(CntType, cnt, bool* result, pred)`
- `void c_any_of(CntType, cnt, bool* result, pred)`
- `void c_none_of(CntType, cnt, bool* result, pred)`
```c
#define DivisibleBy(n) (*value % (n) == 0) // `value` refers to the current element

bool result;
c_any_of(vec_int, vec, &result, DivisibleBy(7));
if (result)
    puts("At least one number is divisible by 7");
```

### quicksort, binary_search, lower_bound - 2X faster qsort on arrays

The **quicksort**, **quicksort_ij** algorithm is about twice as fast as *qsort()*,
and typically simpler to use. You may customize `i_type` and the comparison function
`i_cmp` or `i_less`. All containers with random access may be sorted, including regular C-arrays.
- `void MyType_quicksort(MyType* cnt, intptr_t n);`

There is a [benchmark/test file here](../misc/benchmarks/various/quicksort_bench.c).
```c
#define i_key int                    // note: "container" type becomes `ints` (i_type can override).
#include "stc/algo/quicksort.h"
#include <stdio.h>

int main(void) {
    int nums[] = {5, 3, 5, 9, 7, 4, 7, 2, 4, 9, 3, 1, 2, 6, 4};
    ints_quicksort(nums, c_arraylen(nums)); // note: function name derived from i_key
    c_forrange (i, c_arraylen(arr)) printf(" %d", arr[i]);
}
```
Also sorting deq/queue (with ring buffer) is possible, and very fast. Note that `i_more`
must be defined to extend the life of specified template parameters for use by quicksort:
```c
#define i_TYPE MyDeq,int
#define i_more
#include "stc/deq.h" // deque
#include "stc/algo/quicksort.h"
#include <stdio.h>

int main(void) {
    MyDeq deq = c_init(MyDeq, {5, 3, 5, 9, 7, 4, 7}); // pushed back
    c_foritems (i, int, {2, 4, 9, 3, 1, 2, 6, 4}) MyDeq_push_front(&deq, *i.ref);
    MyDeq_quicksort(&deq);
    c_foreach (i, MyDeq, deq) printf(" %d", *i.ref);
    MyDeq_drop(&deq);
}
```

### c_new, c_delete

- `c_new(Type, val)` - Allocate *and init* a new object on the heap
- `c_delete(Type, ptr)` - Drop *and free* an object allocated on the heap. NULL is OK.
```c
#include "stc/cstr.h"

cstr *str_p = c_new(cstr, cstr_lit("Hello"));
printf("%s\n", cstr_str(str_p));
c_delete(cstr, str_p);
```

### c_malloc, c_calloc, c_realloc, c_free
Memory allocator wrappers which uses signed sizes. Note that the signatures for
*c_realloc()* and *c_free()* have an extra size parameter. These will be used as
default unless `i_malloc`, `i_calloc`, `i_realloc`, and `i_free` are defined. See
[Per container-instance customization](../README.md#per-container-instance-customization)
- `void* c_malloc(intptr_t sz)`
- `void* c_calloc(intptr_t sz)`
- `void* c_realloc(void* old_p, intptr_t old_sz, intptr_t new_sz)`
- `void c_free(void* p, intptr_t sz)`

### c_arraylen
Return number of elements in an array. array must not be a pointer!
```c
int array[] = {1, 2, 3, 4};
intptr_t n = c_arraylen(array);
```

### c_swap, c_const_cast
```c
// Safe macro for swapping internals of two objects of same type:
c_swap(hmap_int, &map1, &map2);

// Type-safe casting a from const (pointer):
const char cs[] = "Hello";
char* s = c_const_cast(char*, cs); // OK
int* ip = c_const_cast(int*, cs);  // issues a warning!
```

### Predefined template parameter functions

**cstr_raw** - Non-owning `const char*` "class" element type: `#define i_keyclass cstr_raw`
```c
typedef     const char* cstr_raw;
int         cstr_raw_cmp(const cstr_raw* x, const cstr_raw* y);
uint64_t    cstr_raw_hash(const cstr_raw* x);
cstr_raw    cstr_raw_clone(cstr_raw sp);
void        cstr_raw_drop(cstr_raw* x);
```
Default implementations
```c
int         c_default_cmp(const Type*, const Type*);    // <=>
bool        c_default_less(const Type*, const Type*);   // <
bool        c_default_eq(const Type*, const Type*);     // ==
uint64_t    c_default_hash(const Type*);
Type        c_default_clone(Type val);                  // return val
Type        c_default_toraw(const Type* p);             // return *p
void        c_default_drop(Type* p);                    // does nothing
```
---
## RAII scope macros
General ***defer*** mechanics for resource acquisition. These macros allows you to specify the
freeing of the resources at the point where the acquisition takes place.
The **checkscoped** utility described below, ensures that the `c_scope*` and `c_defer` macros are used correctly.

| Usage                                     | Description                                               |
|:------------------------------------------|:----------------------------------------------------------|
| `c_defer (drop...) {}`                    | Defer `drop...` to end of scope                           |
| `c_scope (init, drop) {}`                 | Execute `init` and defer executing `drop` to end of scope |
| `c_scope (init, pred, drop) {}`           | Adds a predicate in order to exit early if init failed    |
| `c_scoped (Type var=init, drop) {}`       | Declare `var`. Defer `drop...` to end of scope            |
| `c_scoped (Type var=init, pred, drop) {}` | Adds a predicate in order to exit early if init failed    |
| `continue`                                | Exit a defer/scope*-block without resource leak                  |

```c
// `c_defer` executes the expression(s) when leaving scope.
cstr s1 = cstr_lit("Hello"), s2 = cstr_lit("world");
c_defer (cstr_drop(&s1), cstr_drop(&s2))
{
    printf("%s %s\n", cstr_str(&s1), cstr_str(&s2));
}

// `c_scope` initialize an already declared object or objects, and deinitializes at end of scope:
static pthread_mutex_t mut;
c_scope (pthread_mutex_lock(&mut), pthread_mutex_unlock(&mut))
{
    /* Do syncronized work. */
}

// `c_scoped`: declare and init a new scoped variable and specify the deinitialize call.
c_scoped (cstr str = cstr_lit("Hello"), cstr_drop(&str))
{
    cstr_append(&str, " world");
    printf("%s\n", cstr_str(&str));
}
```
**Example 1**: Use multiple **c_scoped** in sequence:
```c
bool ok = false;
c_scoped (uint8_t* buf = malloc(BUF_SIZE), buf != NULL, free(buf))
c_scoped (FILE* fp = fopen(fname, "rb"), fp != NULL, fclose(fp))
{
    int n = fread(buf, 1, BUF_SIZE, fp);
    if (n <= 0) continue; // NB! do not return here.
    ...
    ok = true;
}
return ok;
```
**Example 2**: Load each line of a text file into a vector of strings:
```c
#include <errno.h>
#define i_implement
#include "stc/cstr.h"

#define i_key_cstr
#include "stc/vec.h"

// receiver should check errno variable
vec_cstr readFile(const char* name)
{
    vec_cstr vec = {0}; // returned
    c_scoped (FILE* fp = fopen(name, "r"), fp != NULL, fclose(fp))
    c_scoped (cstr line = {0}, cstr_drop(&line))
        while (cstr_getline(&line, fp))
            vec_cstr_emplace(&vec, cstr_str(&line));
    return vec;
}

int main(void)
{
    c_scoped (vec_cstr vec = readFile(__FILE__), vec_cstr_drop(&vec))
        c_foreach (i, vec_cstr, vec)
            printf("| %s\n", cstr_str(i.ref));
}
```

### The **checkscoped** utility program (for RAII)
The **checkscoped** program will check the source code for any misuses of the `c_scope*` macros which
may lead to resource leakages. The `c_scope*`- macros are implemented as one-time executed **for-loops**,
so any `return` or `break` appearing within such a block will lead to resource leaks, as it will disable
the cleanup/drop method to be called. A `break` may originally be intended to break a loop or switch
outside the `c_scoped` scope.

NOTE: One must always make sure to unwind temporary allocated resources before a `return` in C. However, by using `c_scope*`-macros,
- it is much easier to automatically detect misplaced return/break between resource acquisition and destruction.
- it prevents forgetting to call the destructor at the end.

The **checkscoped** utility will report any misusages. The following example shows how to correctly break/return
from a `c_scope`:
```c
int flag = 0;
for (int i = 0; i<n; ++i) {
    c_scoped (cstr text = {0}, cstr_drop(&text))
    c_scoped (List list = {0}, List_drop(&list))
    {
        for (int j = 0; j<m; ++j) {
            List_push_back(&list, i*j);
            if (cond1())
                break;  // OK: breaks current for-loop only
        }
        // WRONG:
        if (cond2())
            break;      // checkscoped ERROR! break inside c_scoped.

        if (cond3())
            return -1;  // checkscoped ERROR! return inside c_scoped

        // CORRECT:
        if (cond2()) {
            flag = 1;   // flag to break outer for-loop
            continue;   // cleanup and leave c_scoped block
        }
        if (cond3()) {
            flag = -1;  // return -1
            continue;   // cleanup and leave c_scoped block
        }
        ...
    }
    // do the return/break outside of c_scoped
    if (flag < 0) return flag;
    else if (flag > 0) break;
    ...
}
```
