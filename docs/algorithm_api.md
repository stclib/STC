# STC Algorithms

STC contains many generic algorithms and loop abstactions. Raw loops are one of the most prominent
sources for errors in C and C++ code. By using the loop abstractions below, your code becomes more
descriptive and reduces chances of making mistakes. It is generally easier to read and write too.

## Ranged for-loops
<details>
<summary><b>c_each</b> - Container / range iteration</summary>
"No raw loops" - Sean Parent

### c_each, c_each_reverse, c_each_n, c_each_kv
```c++
#include "stc/common.h"
```

| Usage                                       | Description                               |
|:--------------------------------------------|:------------------------------------------|
| `for (c_each(it, ctype, container))`        | Iteratate all elements                    |
| `for (c_each(it, ctype, it1, it2))`         | Iterate the range [it1, it2)              |
| `for (c_each_reverse(it, ctype, container))`| Iteratate elements in reverse: *vec, deque, queue, stack* |
| `for (c_each_reverse(it, ctype, it1, it2))` | Iteratate range [it1, it2) elements in reverse. |
| `for (c_each_n(it, ctype, container, n))`   | Iteratate `n` first elements. Index variable is `{it}_index`. |
| `for (c_each_kv(key, val, ctype, container))`| Iterate maps with "structured binding" |
<!--{%raw%}-->
[ [Run this code](https://godbolt.org/z/cYhTEr1vM) ]
```c++
#define i_type IMap, int, int
#include "stc/smap.h"
// ...
IMap map = c_make(IMap, {{23,1}, {3,2}, {7,3}, {5,4}, {12,5}});

for (c_each(i, IMap, map))
    printf(" %d", i.ref->first);
// 3 5 7 12 23

// same, with raw for loop:
for (IMap_iter i = IMap_begin(&map); i.ref; IMap_next(&i))
    printf(" %d", i.ref->first);

// iterate from iter to end
IMap_iter iter = IMap_find(&map, 7);
for (c_each(i, IMap, iter, IMap_end(&map)))
    printf(" %d", i.ref->first);
// 7 12 23

// iterate first 3 with an index count enumeration
for (c_each_n(i, IMap, map, 3))
    printf(" %zd:(%d %d)", i_index, i.ref->first, i.ref->second);
// 0:(3 2) 1:(5 4) 2:(7 3)

// iterate a map using "structured binding" of the key and val pair:
for (c_each_kv(id, count, IMap, map))
    printf(" (%d %d)", *id, *count);
```
<!--{%endraw%}-->
</details>
<details>
<summary><b>c_items</b> - Literal list iteration</summary>

### c_items
Iterate compound literal array elements. In addition to `i.ref`, you can access `i.index` and `i.size`.
<!--{%raw%}-->
```c++
// apply multiple push_backs
for (c_items(i, int, {4, 5, 6, 7}))
    list_i_push_back(&lst, *i.ref);

// insert in existing map
for (c_items(i, hmap_ii_value, {{4, 5}, {6, 7}}))
    hmap_ii_insert(&map, i.ref->first, i.ref->second);

// string literals pushed to a stack of cstr elements:
for (c_items(i, const char*, {"Hello", "crazy", "world"}))
    stack_cstr_emplace(&stk, *i.ref);
```
<!--{%endraw%}-->
</details>

## Integer range loops

<details>
<summary><b>c_range</b> - For-loop abstraction</summary>

### c_range, c_range32, c_range_t
- `c_range`: abstraction for iterating sequence of integers. Like python's **for** *i* **in** *range()* loop. Uses `isize` (*ptrdiff_t*) as control variable.
- `c_range32` is like *c_range*, but uses `int32` as control variable.
- `c_range_t` is like *c_range*, but takes an additional ***type*** for the control variable as first argument.

| Usage                                | Python equivalent                    |
|:-------------------------------------|:-------------------------------------|
| `for (c_range(stop))`                | `for _ in range(stop):`              |
| `for (c_range(i, stop))`             | `for i in range(stop):`              |
| `for (c_range(i, start, stop))`      | `for i in range(start, stop):`       |
| `for (c_range(i, start, stop, step))`| `for i in range(start, stop, step):` |

| Usage                                           |
|:------------------------------------------------|
| `for (c_range_t(IntType, i, stop))`             |
| `for (c_range_t(IntType, i, start, stop))`      |
| `for (c_range_t(IntType, i, start, stop, step))`|

```c++
for (c_range(5)) printf("x");
// xxxxx
for (c_range(i, 5)) printf(" %lld", i);
// 0 1 2 3 4
for (c_range(i, -3, 3)) printf(" %lld", i);
// -3 -2 -1 0 1 2
for (c_range(i, 30, 0, -5)) printf(" %lld", i);
// 30 25 20 15 10 5
```
</details>
<details>
<summary><b>c_range, c_iota</b> - Integer range objects</summary>

### crange, crange32, c_iota
An integer sequence generator type, similar to [boost::irange](https://www.boost.org/doc/libs/release/libs/range/doc/html/range/reference/ranges/irange.html).

- `crange` uses `isize` (ptrdiff_t) as control variable
- `crange32` is like *crange*, but uses `int32` as control variable, which may be faster.

```c++
crange      crange_make(stop);              // 0, 1, ... stop-1
crange      crange_make(start, stop);       // start, start+1, ... stop-1
crange      crange_make(start, stop, step); // start, start+step, ... upto-not-including stop,
                                            // step may be negative.
crange_iter crange_begin(crange* self);
void        crange_next(crange_iter* it);


crange&     c_iota(start);                  // l-value; NB! otherwise like crange_make(start, INTPTR_MAX)
crange&     c_iota(start, stop);            // l-value; otherwise like crange_make(start, stop)
crange&     c_iota(start, stop, step);      // l-value; otherwise like crange_make(start, stop, step)
```
 The **crange_value** type is *isize*. Variables *start*, *stop*, and *step* are of type *crange_value*.

[ [Run this code](https://godbolt.org/z/6aaq6qTro) ]
```c++
// 1. All primes less than 32: See below for c_filter() and is_prime()
crange r1 = crange_make(3, 32, 2);
printf("2"); // first prime
c_filter(crange, r1, true
    && is_prime(*value)
    && printf(" %zi", *value)
);
// 2 3 5 7 11 13 17 19 23 29 31

// 2. The first 11 primes:
// c_iota() can be used as argument to c_filter.
printf("2"); // first prime
c_filter(crange, c_iota(3), true
    && is_prime(*value)
    && (c_flt_take(10), printf(" %zi", *value))
);
// 2 3 5 7 11 13 17 19 23 29 31
```

</details>
<details>
<summary><b>c_filter</b> - Range filtering</summary>

### c_filter, c_filter_zip, c_filter_pairwise, for (c_ffilter(...))
Functional programming with chained `&&` filtering. `value` is the pointer to current value.
It enables a subset of functional programming like in other popular languages.

- **Note 1**: The **_reverse** variants only works with *vec, deque, stack, queue* containers.
- **Note 2**: There is also a `c_ffilter` loop variant of `c_filter`. It uses the filter namings
`c_fflt_skip(it, numItems)`, etc.

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

[ [Run this example](https://godbolt.org/z/W87fTdvYd) ]
```c++
#include <stdio.h>
#define i_type Vec, int
#include "stc/stack.h"
#include "stc/algorithm.h"

int main(void)
{
    Vec vec = c_make(Vec, {1, 2, 3, 4, 5, 6, 7, 8, 1, 2, 3, 9, 10, 11, 12, 5});

    c_filter(Vec, vec, true
        && c_flt_skipwhile(*value < 3)  // skip leading values < 3
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
</details>

## Sum types

<details>
<summary><b>Sum types</b> - aka <i>variants</i> or <i>tagged unions</i></summary>

This is a tiny, robust and fully typesafe implementation of sum types. They work
similarly as in Zig, Odin and Rust, and is just as easy and safe to use.
Each tuple/parentesized field is an enum (tag) with an associated data type (payload),
called a *variant* of the sum type. The sum type itself is a **union** type.

Synopsis:
```c++
// Define a sum type
c_sumtype (SumType,
    (VariantEnum1, VariantType1),
    ...
    (VariantEnumN, VariantTypeN)
);

SumType c_variant(VariantEnum tag, VariantType value); // Sum type constructor
bool    c_holds(const SumType* obj, VariantEnum tag);  // does obj hold VariantType?
int     c_tag_index(SumType* obj);                     // 1-based index (mostly for debug)

// Use a sum type (1)
c_when (SumType* obj) {
    c_is(VariantEnum1, VariantType1* x) <body>;
    c_is(VariantEnum2) c_or_is(VariantEnum3) <body>;
    ...
    c_otherwise <body>;
}

// Use a sum type (2)
if (c_is(SumType* obj, VariantEnum1, VariantType1* x)) <body>;
...
else <body>;
```
The **c_when** statement is exhaustive. The compiler will give a warning if not all variants are
covered by **c_is** (requires `-Wall` or `-Wswitch` gcc/clang compiler flag). The first enum value
is deliberately set to 1 in order to easier detect non/zero-initialized variants.

* Note: The `x` variables in the synopsis are "auto" type declared/defined - see examples.
* Caveat 1: The use of `continue` in a `c_when` or `if (c_is())` block, while `c_when` is inside a loop will
not work as expected. It will only break out of the `c_when`-block. Instead, use `goto` to jump to the
end of the loop. `break` will break out of `c_when`, i.e. it behaves like `switch`.
* Caveat 2: Sum types will generally not work in coroutines because the `x` variable is local and therefore
will not be preserved across `cco_yield..` / `cco_await..`.

### Example 1

[ [Run this code](https://godbolt.org/z/PEvjGff3E) ]
```c++
#include <stdio.h>
#include "stc/algorithm.h"

c_sumtype (Tree,
    (Empty, _Bool),
    (Leaf, int),
    (Node, struct {int value; Tree *left, *right;})
);

int tree_sum(Tree* t) {
    c_when (t) {
        c_is(Empty) return 0;
        c_is(Leaf, v) return *v;
        c_is(Node, n) return n->value + tree_sum(n->left) + tree_sum(n->right);
    }
    return -1;
}

int main(void) {
    Tree* tree =
    &c_variant(Node, {1,
        &c_variant(Node, {2,
            &c_variant(Leaf, 3),
            &c_variant(Leaf, 4)
        }),
        &c_variant(Leaf, 5)
    });

    printf("sum = %d\n", tree_sum(tree));
}
```

### Example 2
This example has two sum types. The `MessageChangeColor` variant uses the `Color` sum type as
its data type (payload). Because C does not have namespaces, it is recommended to prefix the variant names with the sum type name, as with regular enums.

[ [Run this code](https://godbolt.org/z/846bbjzGG) ]
```c++
// https://doc.rust-lang.org/book/ch18-03-pattern-syntax.html#destructuring-enums
#include <stdio.h>
#include <stc/algorithm.h>

c_sumtype (Color,
    (ColorRgb, struct {int32 r, g, b;}),
    (ColorHsv, struct {int32 h, s, v;})
);

c_sumtype (Message,
    (MessageQuit, _Bool),
    (MessageMove, struct {int32 x, y;}),
    (MessageWrite, const char*),
    (MessageChangeColor, Color)
);


int main(void) {
    Message msg[] = {
        c_variant(MessageMove, {42, 314}),
        c_variant(MessageChangeColor, c_variant(ColorHsv, {0, 160, 255})),
    };

    for (c_range(i, c_arraylen(msg)))
    c_when (&msg[i]) {
        c_is(MessageQuit) {
            printf("The Quit variant has no data to destructure.\n");
        }
        c_is(MessageMove, p) {
            printf("Move %d in the x direction and %d in the y direction\n", p->x, p->y);
        }
        c_is(MessageWrite, text) {
            printf("Text message: %s\n", *text);
        }
        c_is(MessageChangeColor, cc) c_when (cc) {
            c_is(ColorRgb, c)
                printf("Change color to red %d, green %d, and blue %d\n", c->r, c->g, c->b);
            c_is(ColorHsv, c)
                printf("Change color to hue %d, saturation %d, value %d\n", c->h, c->s, c->v);
        }
    }
}
```
</details>


## Generic algorithms

<details>
<summary><b>c_func</b> - Function with on-the-fly defined return type</summary>

### c_func

A macro for conveniently defining functions with multiple return values. This is for encouraging
to write functions that returns extra error context when error occurs, or just multiple return values.

[ [Run this code](https://godbolt.org/z/MsYG75Eae) ]
```c++
Vec get_data(void) {
    return c_make(Vec, {1, 2, 3, 4, 5, 6});
}

// same as get_data(), but with the c_func macro "syntax".
c_func (get_data1,(void), ->, Vec) {
    return c_make(Vec, {1, 2, 3, 4, 5, 6});
}

// return two Vec types "on-the-fly".
c_func (get_data2,(void), ->, struct {Vec v1, v2;}) {
    return (get_data2_result){
        .v1 = c_make(Vec, {1, 2, 3, 4, 5, 6}),
        .v2 = c_make(Vec, {7, 8, 9, 10, 11})
    };
}

// return a Vec, and an err code which is 0 if OK.
c_func (load_data,(const char* fname), ->, struct {Vec vec; int err;}) {
    FILE* fp = fopen(fname, "rb");
    if (fp == 0)
        return (load_data_result){.err = 1};

    load_data_result out = {Vec_with_size(1024, '\0')};
    fread(out.vec.data, sizeof(out.vec.data[0]), 1024, fp);
    fclose(fp);
    return out;
}
```
</details>
<details>
<summary><b>c_make, c_push_items, c_drop</b> - Generic container operations</summary>

These work on any container. *c_make()* may also be used for **cspan** views.

### c_make, c_push_items, c_drop

- **c_make** - construct any container from an initializer list
- **c_push_items** - push values onto any container from an initializer list
- **c_drop** - drop (destroy) multiple containers of the same type

[ [Run this code](https://godbolt.org/z/e4G4hW5n6) ]
<!--{%raw%}-->
```c++
#include <stdio.h>
#define i_type Vec, int
#include "stc/vec.h"

#define i_type Map, int, int
#include "stc/hmap.h"

c_func (split_map,(Map map), ->, struct {Vec keys, values;}) {
    split_map_result out = {0};
    for (c_each_kv(k, v, Map, map)) {
        Vec_push(&out.keys, *k);
        Vec_push(&out.values, *v);
    }
    return out;
}

int main(void) {
    Vec vec = c_make(Vec, {1, 2, 3, 4, 5, 6});
    Map map = c_make(Map, {{1, 2}, {3, 4}, {5, 6}});

    c_push_items(Vec, &vec, {7, 8, 9, 10, 11, 12});
    c_push_items(Map, &map, {{7, 8}, {9, 10}, {11, 12}});

    for (c_each(i, Vec, vec))
        printf("%d ", *i.ref);
    puts("");

    for (c_each_kv(k, v, Map, map))
        printf("[%d %d] ", *k, *v);
    puts("");

    split_map_result res = split_map(map);

    for (c_each(i, Vec, res.values))
        printf("%d ", *i.ref);
    puts("");

    c_drop(Vec, &vec, &res.keys, &res.values);
    c_drop(Map, &map);
}
```
<!--{%endraw%}-->
</details>
<details>
<summary><b>c_find, c_reverse, c_append, c_erase</b> - Container/array operations</summary>

### c_find_if, c_find_reverse_if
Find linearily in containers using a predicate. `value` is a pointer to each element in predicate.
***outiter_ptr*** must be defined prior to call.
- void `c_find_if`(**CntType**, cnt, outiter_ptr, pred).
- void `c_find_if`(**CntType**, startiter, enditer, outiter_ptr, pred)
- void `c_find_reverse_if`(**CntType**, cnt, outiter_ptr, pred)
- void `c_find_reverse_if`(**CntType**, startiter, enditer, outiter_ptr, pred)

### c_reverse, c_reverse_array

- void `c_reverse`(**CntType**, cnt);   // reverse a cspan, vec, stack, queue or deque type.
- void `c_reverse_array`(array, len);   // reverse an array of elements.

### c_append, c_append_if
Clones any container onto an arbitrary container type, optionally using a predicate to filter out elements.
Requires only that the element types are equal for the two containers.
`value` is the pointer to each element in predicate. See example below for usage.
- void `c_append`(**CntType**, outcnt_ptr, cnt)
- void `c_append`(**OutCnt**, outcnt_ptr, **CntType**, cnt)
- void `c_append_if`(**CntType**, outcnt_ptr, cnt, pred)
- void `c_append_if`(**OutCnt**, outcnt_ptr, **CntType**, cnt, pred)

### c_erase_if, c_eraseremove_if
Erase linearily in containers using a predicate. `value` is a pointer to each element in predicate.
- void `c_erase_if`(**CntType**, cnt_ptr, pred)`. Use with ***list**, ***hmap***, ***hset***, ***smap***, and ***sset***.
- void `c_eraseremove_if`(**CntType**, cnt_ptr, pred)`. Use with ***stack***, ***vec***, ***deque***, and ***queue*** only.

[ [Run this code](https://godbolt.org/z/n7c641WhE) ]
<!--{%raw%}-->
```c++
#include <stdio.h>
#include "stc/cstr.h"
#include "stc/algorithm.h"

#define i_type Vec, int
#define i_use_cmp
#include "stc/stack.h"

#define i_type List, int
#define i_use_cmp
#include "stc/list.h"

#define i_type Map
#define i_keypro cstr
#define i_val int
#include "stc/smap.h"

int main(void)
{
    Vec vec = c_make(Vec, {2, 30, 21, 5, 9, 11});
    Vec outvec = {0};

    // Clone all *value > 10 to outvec. Note: `value` is a pointer to current element
    c_append_if(Vec, &outvec, vec, *value > 10);
    for (c_each(i, Vec, outvec)) printf(" %d", *i.ref);
    puts("");

    // Search vec for first value > 20.
    Vec_iter result;
    c_find_if(Vec, vec, &result, *value > 20);
    if (result.ref) printf("found %d\n", *result.ref);

    // Erase values between 20 and 25 in vec:
    c_eraseremove_if(Vec, &vec, 20 < *value && *value < 25);
    for (c_each(i, Vec, vec)) printf(" %d", *i.ref);
    puts("");

    // Erase all values > 20 in a linked list:
    List list = c_make(List, {2, 30, 21, 5, 9, 11});

    c_erase_if(List, &list, *value > 20);
    for (c_each(i, List, list)) printf(" %d", *i.ref);
    puts("");

    // Search a sorted map from it1, for the first string containing "hello" and erase it:
    Map map = c_make(Map, {{"yes",1}, {"no",2}, {"say hello from me",3}, {"goodbye",4}});
    Map_iter res, it1 = Map_begin(&map);

    c_find_if(Map, it1, Map_end(&map), &res, cstr_contains(&value->first, "hello"));
    if (res.ref) Map_erase_at(&map, res);

    // Erase all strings containing "good" in the sorted map:
    c_erase_if(Map, &map, cstr_contains(&value->first, "good"));
    for (c_each(i, Map, map)) printf("%s, ", cstr_str(&i.ref->first));

    c_drop(Vec, &vec, &outvec);
    List_drop(&list);
    Map_drop(&map);
}
```
<!--{%endraw%}-->
</details>
<details>
<summary><b>c_all_of, c_any_of, c_none_of</b> - Boolean container operations</summary>

### c_all_of, c_any_of, c_none_of
Test a container/range using a predicate. ***result*** is output and must be declared prior to call.
- void `c_all_of`(**CntType**, cnt, bool* result, pred)
- void `c_any_of`(**CntType**, cnt, bool* result, pred)
- void `c_none_of`(**CntType**, cnt, bool* result, pred)
```c++
#define DivisibleBy(n) (*value % (n) == 0) // `value` refers to the current element

bool result;
c_any_of(vec_int, vec, &result, DivisibleBy(7));
if (result)
    puts("At least one number is divisible by 7");
```
</details>
<details>
<summary><b>sort, lower_bound, binary_search</b> - Faster quicksort and binary search</summary>

### sort, lower_bound, binary_search

- `X` refers to the template name specified by `i_type` or `i_key`.
- All containers with random access may be sorted, including regular C-arrays, i.e. **stack**, **vec**
and **deque** when either `i_use_cmp`, `i_cmp` or `i_less` is defined.
- Linked **list** may also be sorted, i.e. only *X_sort()* is available.
```c++
                // Sort c-arrays by defining i_type and include "stc/sort.h":
void            X_sort(const X array[], isize len);
isize           X_lower_bound(const X array[], i_key key, isize len);
isize           X_binary_search(const X array[], i_key key, isize len);

                // or random access containers when `i_less`, `i_cmp` is defined:
void            X_sort(X* self);
isize           X_lower_bound(const X* self, i_key key);
isize           X_binary_search(const X* self, i_key key);

                // functions for sub ranges:
void            X_sort_lowhigh(X* self, isize low, isize high);
isize           X_lower_bound_range(const X* self, i_key key, isize start, isize end);
isize           X_binary_search_range(const X* self, i_key key, isize start, isize end);
```
`i_type` may be customized in the normal way, along with comparison function `i_cmp` or `i_less`.

##### Performance
The *X_sort()*, *X_sort_lowhigh()* functions are about twice as fast as *qsort()* and comparable in
speed with *std::sort()**. Both *X_binary_seach()* and *X_lower_bound()* are about 30% faster than
c++ *std::lower_bound()*.
##### Usage examples

[ [Run this code](https://godbolt.org/z/v3ncM66az) ]
```c++
#define i_key int // sort a regular c-array of ints
#include "stc/sort.h"
#include <stdio.h>

int main(void) {
    int arr[] = {5, 3, 5, 9, 7, 4, 7, 2, 4, 9, 3, 1, 2, 6, 4};
    ints_sort(arr, c_arraylen(arr)); // `ints` derived from the `i_key` name

    for (c_range(i, c_arraylen(arr)))
        printf(" %d", arr[i]);
}
```
```c++
#define i_type MyDeq, int
#define i_use_cmp      // enable sorting
#include "stc/deque.h" // can be swapped with any of the above
#include <stdio.h>

int main(void) {
    MyDeq deq = c_make(MyDeq, {5, 3, 5, 9, 7, 4, 7});

    MyDeq_sort(&deq);

    for (c_each(i, MyDeq, deq))
        printf(" %d", *i.ref);
    puts("");

    MyDeq_drop(&deq);
}
```

</details>
<details>
<summary><b>c_new, c_delete, c_malloc, etc.</b> - Allocation helpers</summary>

### c_new, c_delete

- Type\* `c_new`(**Type**, value) - Allocate *and initialize* a new object on the heap with *value*.
- Type\* `c_new_n`(**Type**, n) - Allocate an array of ***n*** new objects on the heap, initialized to zero.
- void `c_delete`(**Type**, ptr) - *Type_drop(ptr)* and *c_free(ptr, ..)* allocated on the heap. NULL is OK.
- void `c_delete_n`(**Type**, arr, n) - *Type_drop(&arr[i])* and *c_free(arr, ..)* of ***n*** objects allocated on the heap. (NULL, 0) is OK.
```c++
#include "stc/cstr.h"

cstr* stringptr = c_new (cstr, cstr_from("Hello"));
printf("%s\n", cstr_str(stringp));
c_delete(cstr, stringptr);
```

### c_malloc, c_calloc, c_realloc, c_free
Memory allocator wrappers which uses signed sizes. Note that the signatures for
*c_realloc()* and *c_free()* have an extra size parameter. These will be used as
default in containers unless `i_malloc`, `i_calloc`, `i_realloc`, and `i_free` are user defined. See
[Per container-instance customization](../README.md#per-container-instance-customization)
- void* `c_malloc`(isize sz)
- void* `c_calloc`(isize n, isize sz)
- void* `c_realloc`(void* old_p, isize old_sz, isize new_sz)
- void  `c_free`(void* p, isize sz)

</details>
<details>
<summary>Miscellaneous macros</summary>

### c_arraylen
Return number of elements in an array. array must not be a pointer!
```c++
int array[] = {1, 2, 3, 4};
isize n = c_arraylen(array);
```

### c_swap, c_const_cast
```c++
// Side effect- and typesafe macro for swapping internals of two objects of same type:
c_swap(&map1, &map2);

// Type-safe casting a from const (pointer):
const char cs[] = "Hello";
char* s = c_const_cast(char*, cs); // OK
int* ip = c_const_cast(int*, cs);  // issues a warning!

// Type safe casting:
#define tofloat(d) c_safe_cast(float, double, d)
```
</details>
<!--
<details>
<summary>Predefined template parameter functions</summary>
### Predefined template parameter functions

**cstr_raw** - Non-owning `const char*` "class" element type: `#define i_keyclass cstr_raw`
```c++
typedef         const char* cstr_raw;
int             cstr_raw_cmp(const cstr_raw* x, const cstr_raw* y);
size_t          cstr_raw_hash(const cstr_raw* x);
cstr_raw        cstr_raw_clone(cstr_raw sp);
void            cstr_raw_drop(cstr_raw* x);
```
Default implementations
```c++
int             c_default_cmp(const Type*, const Type*);    // <=>
bool            c_default_less(const Type*, const Type*);   // <
bool            c_default_eq(const Type*, const Type*);     // ==
size_t          c_default_hash(const Type*);
Type            c_default_clone(Type val);                  // return val
Type            c_default_toraw(const Type* p);             // return *p
void            c_default_drop(Type* p);                    // does nothing
```
</details>
-->
<details>
<summary><b>c_defer, c_with</b> - RAII macros</summary>

### c_defer, c_with

| Usage                                 | Description                                            |
|:--------------------------------------|:-------------------------------------------------------|
| `c_defer (deinit, ...) {}`            | Defers execution of `deinit`s to end of scope          |
| `c_with (init, deinit) {}`            | Declare and/or initialize a variable. Defers execution of `deinit` to end of scope |
| `c_with (init, condition, deinit) {}` | Adds a predicate in order to exit early if init fails  |
| `continue`                            | Break out of a `c_with` scope without resource leakage |
*Note*: Regular `return`, `break` and `continue` must not be used
anywhere inside a defer scope.

```c++
// declare and init a new scoped variable and specify the deinitialize call:
c_with (cstr str = cstr_lit("Hello"), cstr_drop(&str))
{
    cstr_append(&str, " world");
    printf("%s\n", cstr_str(&str));
}

pthread_mutex_t lock;
...
// use c_with without variable declaration:
c_with (pthread_mutex_lock(&lock), pthread_mutex_unlock(&lock))
{
    // syncronized code
}
```

**Example 2**: Load each line of a text file into a vector of strings:
```c++
#include <errno.h>
#include "stc/cstr.h"

#define i_keypro cstr
#include "stc/vec.h"

// receiver should check errno variable
vec_cstr readFile(const char* name)
{
    vec_cstr vec = {0}; // returned
    c_with (FILE* fp = fopen(name, "r"), fp != NULL, fclose(fp))
    c_with (cstr line = {0}, cstr_drop(&line))
        while (cstr_getline(&line, fp))
            vec_cstr_emplace(&vec, cstr_str(&line));
    return vec;
}

int main(void)
{
    c_with (vec_cstr vec = readFile(__FILE__), vec_cstr_drop(&vec))
        for (c_each(i, vec_cstr, vec))
            printf("| %s\n", cstr_str(i.ref));
}
```
</details>

---
