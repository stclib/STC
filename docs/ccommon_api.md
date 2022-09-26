# STC [ccommon](../include/stc/ccommon.h): RAII and iterator macros

The following macros are recommended to use, and they safe/have no side-effects.

## Scope macros (RAII)
### c_auto, c_with, c_scope, c_defer
General ***defer*** mechanics for resource acquisition. These macros allows you to specify the
freeing of the resources at the point where the acquisition takes place.
The **checkauto** utility described below, ensures that the `c_auto*` macros are used correctly.

| Usage                                  | Description                                               |
|:---------------------------------------|:----------------------------------------------------------|
| `c_auto (Type, var...)`                | Same as `c_with (Type var=Type_init(), Type_drop(&var))`  |
| `c_with (Type var=init, drop)`         | Declare `var`. Defer `drop...` to end of scope            |
| `c_with (Type var=init, pred, drop)`   | Adds a predicate in order to exit early if init failed    |
| `c_scope (init, drop...)`              | Execute `init` and defer `drop...` to end of scope        |
| `c_defer (drop...)`                    | Defer `drop...` to end of scope                           |
| `continue`                             | Exit a `c_auto/c_with/c_scope...` without memory leaks    |

For multiple variables, use either multiple **c_with** in sequence, or declare variable outside
scope and use **c_scope**. For convenience, **c_auto** support up to 4 variables.
```c
// `c_with` is similar to python `with`: it declares and can drop a variable after going out of scope.
c_with (uint8_t* buf = malloc(BUF_SIZE), free(buf))
c_with (FILE* fp = fopen(fname, "rb"), fclose(fp))
{
    int n = 0;
    if (fp && buf) {
        n = fread(buf, 1, BUF_SIZE, fp);
        doSomething(buf, n);
    }
}

c_with (cstr str = cstr_new("Hello"), cstr_drop(&str))
{
    cstr_append(&str, " world");
    printf("%s\n", cstr_str(&str));
}

// `c_auto` automatically initialize and destruct up to 4 variables, like `c_with`.
c_auto (cstr, s1, s2)
{
    cstr_append(&s1, "Hello");
    cstr_append(&s1, " world");

    cstr_append(&s2, "Cool");
    cstr_append(&s2, " stuff");

    printf("%s %s\n", cstr_str(&s1), cstr_str(&s2));
}

// `c_scope` is like `c_with` but works with an already declared variable.
static pthread_mutex_t mut;
c_scope (pthread_mutex_lock(&mut), pthread_mutex_unlock(&mut))
{
    /* Do syncronized work. */
}

// `c_defer` executes the expressions when leaving scope.
cstr s1 = cstr_new("Hello"), s2 = cstr_new("world");
c_defer (cstr_drop(&s1), cstr_drop(&s2))
{
    printf("%s %s\n", cstr_str(&s1), cstr_str(&s2));
}
```
**Example**: Load each line of a text file into a vector of strings:
```c
#include <errno.h>
#include <stc/cstr.h>

#define i_val_str
#include <stc/cvec.h>

// receiver should check errno variable
cvec_str readFile(const char* name)
{
    cvec_str vec = cvec_str_init(); // returned

    c_with (FILE* fp = fopen(name, "r"), fclose(fp))
        c_with (cstr line = cstr_null, cstr_drop(&line))
            while (cstr_getline(&line, fp))
                cvec_str_emplace_back(&vec, cstr_str(&line));
    return vec;
}

int main()
{
    c_with (cvec_str x = readFile(__FILE__), cvec_str_drop(&x))
        c_foreach (i, cvec_str, x)
            printf("%s\n", cstr_str(i.ref));
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
    } // for
```
## Loop abstaction macros

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

// reverse the list:
c_forlist (i, int, {1, 2, 3})
    cvec_i_push_back(&vec, i.data[i.size - 1 - i.index]);
```

### c_foreach, c_forpair

| Usage                                    | Description                     |
|:-----------------------------------------|:--------------------------------|
| `c_foreach (it, ctype, container)`       | Iteratate all elements          |
| `c_foreach (it, ctype, it1, it2)`        | Iterate the range [it1, it2)    |
| `c_forpair (key, val, ctype, container)` | Iterate with structured binding |

```c
#define i_key int
#define i_val int
#define i_tag ii
#include <stc/csmap.h>
...
c_forlist (i, csmap_ii_raw, { {23,1}, {3,2}, {7,3}, {5,4}, {12,5} })
    csmap_ii_insert(&map, i.ref->first, i.ref->second);

c_foreach (i, csmap_ii, map)
    printf(" %d", i.ref->first);
// 3 5 7 12 23

csmap_ii_iter it = csmap_ii_find(&map, 7);
c_foreach (i, csmap_ii, it, csmap_ii_end(&map))
    printf(" %d", i.ref->first);
// 7 12 23

c_forpair (id, count, csmap_ii, map)
    printf(" (%d %d)", *_.id, *_.count);
// (3 2) (5 4) (7 3) (12 5) (23 1)
```

### c_forloop
Abstaction for iterating sequence of numbers. Like python's **for** *i* **in** *range()* loop.

| Usage                                       | Python equivalent                    |
|:--------------------------------------------|:-------------------------------------|
| `c_forloop (stop)`                          | `for _ in range(stop):`              |
| `c_forloop (i, stop) // i type = long long` | `for i in range(stop):`              |
| `c_forloop (i, start, stop)`                | `for i in range(start, stop):`       |
| `c_forloop (i, start, stop, step)`          | `for i in range(start, stop, step):` |

```c
c_forloop (5) printf("x");
// xxxxx
c_forloop (i, 5) printf(" %lld", i);
// 0 1 2 3 4
c_forloop (i, -3, 3) printf(" %lld", i);
// -3 -2 -1 0 1 2
c_forloop (i, 30, 0, -5) printf(" %lld", i);
// 30 25 20 15 10 5
```

### c_forwhile, c_forfilter
Iterate containers with stop-criteria and chained range filtering.

| Usage                                                         | Description                          |
|:--------------------------------------------------------------|:-------------------------------------|
| `c_forwhile (it, ctype, container, shortcircuit)`             | Iterate until shortcircuit is false  |
| `c_forfilter (it, ctype, container, filter(s))`               | Filter out items in chain            |
| `c_forfilter (it, ctype, container, filter(s), shortcircuit)` | Add a "short-circuit" pred/filter    |

| Built-in filter                   | Description                          |
|:----------------------------------|:-------------------------------------|
| `c_flt_drop(it, numItems)`        | Drop numItems                        |
| `c_flt_take(it, numItems)`        | Take numItems                        |
| `c_flt_dropwhile(it, predicate)`  | Drop items until predicate is false  |
| `c_flt_takewhile(it, predicate)`  | Take items until predicate is false  |

`it.index` holds the index of the source item.
```c
#define i_type IVec
#define i_val int
#include <stc/cstack.h>
#include <stdio.h>

bool isPrime(int i) {
    for (int j=2; j*j <= i; ++j) if (i % j == 0) return false;
    return true;
}
#define isOdd(i) ((i) & 1)

int main() {
    c_auto (IVec, vec) {
        c_forloop (i, 1000) IVec_push(&vec, 1000000 + i);

        c_forfilter (i, IVec, vec,
                        isOdd(*i.ref)
                     && c_flt_drop(i, 100) // built-in
                     && isPrime(*i.ref)
                      , c_flt_take(i, 10)) { // breaks loop on false.
            printf(" %d", *i.ref);  
        }
        puts("");
    }
}
// Out: 1000211 1000213 1000231 1000249 1000253 1000273 1000289 1000291 1000303 1000313 
```
Note that `c_flt_take()` is given as an optional argument, which makes the loop stop when it becomes false (for efficiency). Chaining it after `isPrime()` instead will give same result, but the full input is processed.

### crange
**crange** is a number sequence generator type. The **crange_value** type is `long long`. Below, *start*, *stop*, *step* are type *crange_value*:
```c
crange      crange_init(void);              // will generate 0, 1, 2, ...
crange      crange_make(stop);              // will generate 0, 1, ..., stop-1
crange      crange_make(start, stop);       // will generate start, start+1, ... stop-1
crange      crange_make(start, stop, step); // will generate start, start+step, ... upto-not-including stop
                                            // note that step may be negative.
crange_iter crange_begin(crange* self);
crange_iter crange_end(crange* self);
void        crange_next(crange_iter* it);

// 1. All primes less than 32:
crange r1 = crange_from(3, 32, 2);
printf("2"); // first prime
c_forfilter (i, crange, r1, 
                isPrime(*i.ref))
    printf(" %lld", *i.ref);
// 2 3 5 7 11 13 17 19 23 29 31

// 2. The 11 first primes:
crange r2 = crange_make(3, INTMAX_MAX, 2);
printf("2");
c_forfilter (i, crange, r2, 
                isPrime(*i.ref)
              , c_flt_take(10))
    printf(" %lld", *i.ref);
// 2 3 5 7 11 13 17 19 23 29 31
```
### c_find_if, c_find_in, c_erase_if
Find or erase linearily in containers using a predicate
```c
// Search vec for first value > 2:
cvec_i_iter i;
c_find_if(i, cvec_i, vec, *i.ref > 2);
if (i.ref) printf("%d\n", *i.ref);

// Search map for a string containing "hello" and erase it:
cmap_str_iter it, it1 = ..., it2 = ...;
c_find_in(it, csmap_str, it1, it2, cstr_contains(it.ref, "hello"));
if (it.ref) cmap_str_erase_at(&map, it);

// Erase all numbers less than 100:
c_erase_if(k, cvec_i, *k.ref < 100);
```

### c_new, c_alloc, c_alloc_n, c_drop, c_make

| Usage                          | Meaning                                 |
|:-------------------------------|:----------------------------------------|
| `c_new (type, value)`          | Move value to a new object on the heap  |
| `c_alloc (type)`               | `(type *) c_malloc(sizeof(type))`       |
| `c_alloc_n (type, N)`          | `(type *) c_malloc((N)*sizeof(type))`   |
| `c_drop (ctype, &c1, ..., &cN)` | `ctype_drop(&c1); ... ctype_drop(&cN)`    |
| `c_make(type){value...}`       | `(type){value...}` // c++ compatability |

```c
struct Pnt { double x, y, z; };
struct Pnt *pnt = c_new (struct Pnt, {1.2, 3.4, 5.6});
c_free(pnt);

int* array = c_alloc_n (int, 100);
c_free(array);

cstr a = cstr_new("Hello"), b = cstr_new("World");
c_drop(cstr, &a, &b);
```

### General predefined template parameter functions
```
int         c_default_cmp(const Type*, const Type*);
Type        c_default_clone(Type val);           // simple copy
Type        c_default_toraw(const Type* val);    // dereference val
void        c_default_drop(Type* val);           // does nothing

typedef     const char* crawstr;
int         crawstr_cmp(const crawstr* x, const crawstr* y);
bool        crawstr_eq(const crawstr* x, const crawstr* y);
uint64_t    crawstr_hash(const crawstr* x);
```

### c_malloc, c_calloc, c_realloc, c_free
Memory allocator for the entire library. Macros can be overloaded by the user.

### c_swap, c_arraylen
- **c_swap(type, x, y)**: Simple macro for swapping internals of two objects.
- **c_arraylen(array)**: Return number of elements in an array, e.g. `int array[] = {1, 2, 3, 4};`
