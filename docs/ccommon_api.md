# STC Algorithms

---
## Ranged for-loops

### c_foreach, c_foreach_rv, c_forpair

| Usage                                    | Description                               |
|:-----------------------------------------|:------------------------------------------|
| `c_foreach (it, ctype, container)`       | Iteratate all elements                    |
| `c_foreach (it, ctype, it1, it2)`        | Iterate the range [it1, it2)              |
| `c_foreach_rv (it, ctype, container)`    | Iteratate in reverse (cstack, cvec, cdeq) |
| `c_forpair (key, val, ctype, container)` | Iterate with structured binding           |

```c
#define i_key int
#define i_val int
#define i_tag ii
#include <stc/csmap.h>
...
csmap_ii map = c_make(csmap_ii, { {23,1}, {3,2}, {7,3}, {5,4}, {12,5} });

c_foreach (i, csmap_ii, map)
    printf(" %d", i.ref->first);
// 3 5 7 12 23
// same without using c_foreach:
for (csmap_ii_iter i = csmap_ii_begin(&map); i.ref; csmap_ii_next(&i))
    printf(" %d", i.ref->first);

csmap_ii_iter it = csmap_ii_find(&map, 7);
// iterate from it to end
c_foreach (i, csmap_ii, it, csmap_ii_end(&map))
    printf(" %d", i.ref->first);
// 7 12 23

// structured binding:
c_forpair (id, count, csmap_ii, map)
    printf(" (%d %d)", *_.id, *_.count);
// (3 2) (5 4) (7 3) (12 5) (23 1)
```

### c_forlist
Iterate compound literal array elements. Additional to `i.ref`, you can access `i.data`, `i.size`, and `i.index` of the input list/element.
```c
// apply multiple push_backs
c_forlist (i, int, {1, 2, 3})
    cvec_i_push_back(&vec, *i.ref);

// insert in existing map
c_forlist (i, cmap_ii_raw, { {4, 5}, {6, 7} })
    cmap_ii_insert(&map, i.ref->first, i.ref->second);

// string literals pushed to a stack of cstr:
c_forlist (i, const char*, {"Hello", "crazy", "world"})
    cstack_str_emplace(&stk, *i.ref);
```

---
## Range algorithms

### c_forrange
Abstraction for iterating sequence of integers. Like python's **for** *i* **in** *range()* loop.

| Usage                                        | Python equivalent                    |
|:---------------------------------------------|:-------------------------------------|
| `c_forrange (stop)`                          | `for _ in range(stop):`              |
| `c_forrange (i, stop) // i type = long long` | `for i in range(stop):`              |
| `c_forrange (i, start, stop)`                | `for i in range(start, stop):`       |
| `c_forrange (i, start, stop, step)`          | `for i in range(start, stop, step):` |

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

### crange
A number sequence generator type, similar to [boost::irange](https://www.boost.org/doc/libs/release/libs/range/doc/html/range/reference/ranges/irange.html). The **crange_value** type is `long long`. Below *start*, *stop*, and *step* are of type *crange_value*:
```c
crange&     crange_obj(...)              // create a compound literal crange object
crange      crange_make(stop);              // will generate 0, 1, ..., stop-1
crange      crange_make(start, stop);       // will generate start, start+1, ... stop-1
crange      crange_make(start, stop, step); // will generate start, start+step, ... upto-not-including stop
                                            // note that step may be negative.
crange_iter crange_begin(crange* self);
crange_iter crange_end(crange* self);
void        crange_next(crange_iter* it);

// 1. All primes less than 32:
crange r1 = crange_make(3, 32, 2);
printf("2"); // first prime
c_forfilter (i, crange, r1, isPrime(*i.ref))
    printf(" %lld", *i.ref);
// 2 3 5 7 11 13 17 19 23 29 31

// 2. The first 11 primes:
printf("2");
c_forfilter (i, crange, crange_obj(3, INT64_MAX, 2),
    isPrime(*i.ref) &&
    c_flt_take(10)
){
    printf(" %lld", *i.ref);
}
// 2 3 5 7 11 13 17 19 23 29 31
```

### c_forfilter
Iterate a container/range with chained range filtering.

| Usage                                               | Description                            |
|:----------------------------------------------------|:---------------------------------------|
| `c_forfilter (it, ctype, container, filter)`        | Filter out items in chain with &&      |
| `c_forfilter_it (it, ctype, startit, filter)`       | Filter from startit position           |

| Built-in filter                   | Description                                |
|:----------------------------------|:-------------------------------------------|
| `c_flt_skip(it, numItems)`        | Skip numItems (inc count)                  |
| `c_flt_take(it, numItems)`        | Take numItems (inc count)                  |
| `c_flt_skipwhile(it, predicate)`  | Skip items until predicate is false        |
| `c_flt_takewhile(it, predicate)`  | Take items until predicate is false        |
| `c_flt_counter(it)`               | Increment current and return count         |
| `c_flt_getcount(it)`              | Number of items passed skip*/take*/counter |

[ [Run this example](https://godbolt.org/z/n9aYrYPv8) ]
```c
#include <stc/calgo.h>
#include <stdio.h>

bool isPrime(long long i) {
    for (long long j=2; j*j <= i; ++j)
        if (i % j == 0) return false;
    return true;
}

int main() {
    // Get 10 prime numbers starting from 1000. Skip the first 15 primes,
    // then select every 25th prime (including the initial).
    crange R = crange_make(1001, INT64_MAX, 2); // 1001, 1003, ...

    c_forfilter (i, crange, R,
                    isPrime(*i.ref)            &&
                    c_flt_skip(i, 15)          &&
                    c_flt_counter(i) % 25 == 1 &&
                    c_flt_take(i, 10)
    ){
        printf(" %lld", *i.ref);
    }
}
// out: 1097 1289 1481 1637 1861 2039 2243 2417 2657 2803
```
Note that `c_flt_take()` and `c_flt_takewhile()` breaks the loop on false.

---
## Generic algorithms

### c_make, c_drop

Make any container from an initializer list:
```c
#define i_val_str // owned cstr string value type
#include <stc/cset.h>

#define i_key int
#define i_val int
#include <stc/cmap.h>
...
// Initializes with const char*, internally converted to cstr!
cset_str myset = c_make(cset_str, {"This", "is", "the", "story"});
cset_str myset2 = c_clone(myset); 

int x = 7, y = 8;
cmap_int mymap = c_make(cmap_int, { {1, 2}, {3, 4}, {5, 6}, {x, y} });
```
Drop multiple containers of the same type:
```c
c_drop(cset_str, &myset, &myset2);
```

### c_find_if, c_erase_if, c_eraseremove_if
Find or erase linearily in containers using a predicate
- For `c_find_if(iter, C, c, pred)`, ***iter*** is in/out and must be declared prior to call.
- Use `c_erase_if(iter, C, c, pred)` with **clist**, **cmap**, **cset**, **csmap**, and **csset**.
- Use `c_eraseremove_if(iter, C, c, pred)` with **cstack**, **cvec**, **cdeq**, and **cqueue**.
```c
// Search vec for first value > 2:
cvec_i_iter i;
c_find_if(i, cvec_i, vec, *i.ref > 2);
if (i.ref) printf("%d\n", *i.ref);

// Erase all values > 2 in vec:
c_eraseremove_if(i, cvec_i, vec, *i.ref > 2);

// Search map for a string containing "hello" and erase it:
cmap_str_iter it, it1 = ..., it2 = ...;
c_find_if(it, csmap_str, it1, it2, cstr_contains(it.ref, "hello"));
if (it.ref) cmap_str_erase_at(&map, it);

// Erase all strings containing "hello" in a sorted map:
c_erase_if(i, csmap_str, map, cstr_contains(i.ref, "hello"));
```

### csort - two times faster qsort

When very fast array sorting is required, **csort** is about twice as fast as *qsort()*, and often simpler to use.
You may customize `i_tag` and the comparison function `i_cmp` or `i_less`.  

There is a [benchmark/test file here](../misc/benchmarks/various/csort_bench.c).
```c
#define i_val int
#include <stc/algo/csort.h>

int main() {
    int array[] = {5, 3, 5, 9, 7, 4, 7, 2, 4, 9, 3, 1, 2, 6, 4};
    csort_int(array, c_arraylen(array));
}
```


### c_new, c_delete

- `c_new(Type, val)` - Allocate *and init* a new object on the heap
- `c_delete(Type, ptr)` - Drop *and free* an object allocated on the heap. NULL is OK.
```c
#include <stc/cstr.h>

cstr *str_p = c_new(cstr, cstr_from("Hello"));
printf("%s\n", cstr_str(str_p));
c_delete(cstr, str_p);
```

### c_malloc, c_calloc, c_realloc, c_free
Memory allocator wrappers that uses signed sizes.

### c_arraylen
Return number of elements in an array. array must not be a pointer!
```c
int array[] = {1, 2, 3, 4};
intptr_t n = c_arraylen(array);
```

### c_swap, c_const_cast
```c
// Safe macro for swapping internals of two objects of same type:
c_swap(cmap_int, &map1, &map2);

// Type-safe casting a from const (pointer):
const char cs[] = "Hello";
char* s = c_const_cast(char*, cs); // OK
int* ip = c_const_cast(int*, cs);  // issues a warning!
```

### Predefined template parameter functions

**crawstr** - Non-owned `const char*` "class" element type: `#define i_valclass crawstr`
```c
typedef     const char* crawstr;
int         crawstr_cmp(const crawstr* x, const crawstr* y);
bool        crawstr_eq(const crawstr* x, const crawstr* y);
uint64_t    crawstr_hash(const crawstr* x);
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
## Coroutines
This is a much improved implementation of 
[Simon Tatham's coroutines](https://www.chiark.greenend.org.uk/~sgtatham/coroutines.html),
which utilizes the *Duff's device* trick. Tatham's implementation is not typesafe,
and it always allocates the coroutine's internal state dynamically. But crucially,
it does not let the coroutine do self-cleanup on early finish - i.e. it
only frees the initial dynamically allocated memory.

In this implementation, a coroutine may have any signature, but it should
take a struct pointer as parameter, which must contain the member `int cco_state;`
The struct should normally store all the *local* variables to be used in the
coroutine. It can also store input and output data if desired.

The coroutine example below generates Pythagorian triples, but the calling loop
skips the triples which are upscaled version of smaller ones, by checking 
the gcd() function. It also ensures that it stops when the diagonal size >= 100:

[ [Run this code](https://godbolt.org/z/coqqrfbd5) ]
```c
#include <stc/calgo.h>

struct triples {
    int n; // input: max number of triples to be generated.
    int a, b, c;
    int cco_state; // required member
};

bool triples_next(struct triples* i) { // coroutine
    cco_begin(i);
        for (i->c = 5; i->n; ++i->c) {
            for (i->a = 1; i->a < i->c; ++i->a) {
                for (i->b = i->a + 1; i->b < i->c; ++i->b) {
                    if ((int64_t)i->a*i->a + (int64_t)i->b*i->b == (int64_t)i->c*i->c) {
                        cco_yield(true);
                        if (--i->n == 0) cco_return;
                    }
                }
            }
        }
        cco_final: // required label
        puts("done");
    cco_end(false);
}

int gcd(int a, int b) { // greatest common denominator
    while (b) {
        int t = a % b;
        a = b;
        b = t;
    }
    return a;
}

int main()
{
    struct triples t = {.n=INT32_MAX};
    int n = 0;

    while (triples_next(&t)) {
        // Skip triples with GCD(a,b) > 1
        if (gcd(t.a, t.b) > 1)
            continue;
        
        // Stop when c >= 100
        if (t.c < 100)
            printf("%d: [%d, %d, %d]\n", ++n, t.a, t.b, t.c);
        else
            cco_stop(&t); // cleanup in next coroutine call/resume
    }
}
```
### Coroutine API
**Note**: *cco_yield()* may not be called inside a `switch` statement. Use `if-else-if` constructs instead.
To resume the coroutine from where it was suspended with *cco_yield()*, simply call the coroutine again.

|           |  Function / operator                 | Description                             |
|:----------|:-------------------------------------|:----------------------------------------|
|           | `cco_final:`                         | Obligatory label in coroutine           |
|           | `cco_return;`                        | Early return from the coroutine         |
| `bool`    | `cco_alive(ctx)`                     | Is coroutine in initial or suspended state? |
| `bool`    | `cco_suspended(ctx)`                 | Is coroutine in suspended state?        |
| `void`    | `cco_begin(ctx)`                     | Begin coroutine block                   |
| `rettype` | `cco_end(retval)`                    | End coroutine block with return value   |
| `void`    | `cco_end()`                          | End coroutine block                     |
| `rettype` | `cco_yield(retval)`                  | Suspend execution and return a value    |
| `void`    | `cco_yield()`                        | Suspend execution                       |
| `rettype` | `cco_yield(corocall2, ctx2, retval)` | Yield from another coroutine and return val |
| `void`    | `cco_yield(corocall2, ctx2)`         | Yield from another coroutine            |
|           | From the caller side:                |                                         | 
| `void`    | `cco_stop(ctx)`                      | Next call of coroutine returns `cco_end()` |                  
| `void`    | `cco_reset(ctx)`                     | Reset state to initial (for reuse)      |

---
## RAII scope macros
General ***defer*** mechanics for resource acquisition. These macros allows you to specify the
freeing of the resources at the point where the acquisition takes place.
The **checkauto** utility described below, ensures that the `c_auto*` macros are used correctly.

| Usage                                  | Description                                               |
|:---------------------------------------|:----------------------------------------------------------|
| `c_defer (drop...)`                    | Defer `drop...` to end of scope                           |
| `c_scope (init, drop)`                 | Execute `init` and defer `drop` to end of scope           |
| `c_scope (init, pred, drop)`           | Adds a predicate in order to exit early if init failed    |
| `c_with (Type var=init, drop)`         | Declare `var`. Defer `drop...` to end of scope            |
| `c_with (Type var=init, pred, drop)`   | Adds a predicate in order to exit early if init failed    |
| `c_auto (Type, var1,...,var4)`         | `c_with (Type var1=Type_init(), Type_drop(&var1))` ...    |
| `continue`                             | Exit a defer-block without resource leak                  |

```c
// `c_defer` executes the expression(s) when leaving scope.
cstr s1 = cstr_lit("Hello"), s2 = cstr_lit("world");
c_defer (cstr_drop(&s1), cstr_drop(&s2))
{
    printf("%s %s\n", cstr_str(&s1), cstr_str(&s2));
}

// `c_scope` syntactically "binds" initialization and defer.
static pthread_mutex_t mut;
c_scope (pthread_mutex_lock(&mut), pthread_mutex_unlock(&mut))
{
    /* Do syncronized work. */
}

// `c_with` is similar to python `with`: declare a variable and defer the drop call.
c_with (cstr str = cstr_lit("Hello"), cstr_drop(&str))
{
    cstr_append(&str, " world");
    printf("%s\n", cstr_str(&str));
}

// `c_auto` automatically initialize and drops up to 4 variables:
c_auto (cstr, s1, s2)
{
    cstr_append(&s1, "Hello");
    cstr_append(&s1, " world");
    cstr_append(&s2, "Cool");
    cstr_append(&s2, " stuff");
    printf("%s %s\n", cstr_str(&s1), cstr_str(&s2));
}
```
**Example 1**: Use multiple **c_with** in sequence:
```c
bool ok = false;
c_with (uint8_t* buf = malloc(BUF_SIZE), buf != NULL, free(buf))
c_with (FILE* fp = fopen(fname, "rb"), fp != NULL, fclose(fp))
{
    int n = fread(buf, 1, BUF_SIZE, fp);
    if (n <= 0) continue; // auto cleanup! NB do not break or return here.
    ...
    ok = true;
}
return ok;
```
**Example 2**: Load each line of a text file into a vector of strings:
```c
#include <errno.h>
#include <stc/cstr.h>

#define i_val_str
#include <stc/cvec.h>

// receiver should check errno variable
cvec_str readFile(const char* name)
{
    cvec_str vec = {0}; // returned
    c_with (FILE* fp = fopen(name, "r"), fp != NULL, fclose(fp))
    c_with (cstr line = {0}, cstr_drop(&line))
        while (cstr_getline(&line, fp))
            cvec_str_emplace(&vec, cstr_str(&line));
    return vec;
}

int main()
{
    c_with (cvec_str vec = readFile(__FILE__), cvec_str_drop(&vec))
        c_foreach (i, cvec_str, vec)
            printf("| %s\n", cstr_str(i.ref));
}
```

### The **checkauto** utility program (for RAII)
The **checkauto** program will check the source code for any misuses of the `c_auto*` macros which
may lead to resource leakages. The `c_auto*`- macros are implemented as one-time executed **for-loops**,
so any `return` or `break` appearing within such a block will lead to resource leaks, as it will disable
the cleanup/drop method to be called. A `break` may originally be intended to break a loop or switch
outside the `c_auto` scope.

NOTE: One must always make sure to unwind temporary allocated resources before a `return` in C. However, by using `c_auto*`-macros,
- it is much easier to automatically detect misplaced return/break between resource acquisition and destruction.
- it prevents forgetting to call the destructor at the end.

The **checkauto** utility will report any misusages. The following example shows how to correctly break/return
from a `c_auto` scope:
```c
int flag = 0;
for (int i = 0; i<n; ++i) {
    c_auto (cstr, text)
    c_auto (List, list)
    {
        for (int j = 0; j<m; ++j) {
            List_push_back(&list, i*j);
            if (cond1())
                break;  // OK: breaks current for-loop only
        }
        // WRONG:
        if (cond2())
            break;      // checkauto ERROR! break inside c_auto.

        if (cond3())
            return -1;  // checkauto ERROR! return inside c_auto

        // CORRECT:
        if (cond2()) {
            flag = 1;   // flag to break outer for-loop
            continue;   // cleanup and leave c_auto block
        }
        if (cond3()) {
            flag = -1;  // return -1
            continue;   // cleanup and leave c_auto block
        }
        ...
    }
    // do the return/break outside of c_auto
    if (flag < 0) return flag;
    else if (flag > 0) break;
    ...
}
```
