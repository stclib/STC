# STC [ccommon](../include/stc/ccommon.h): Generic algorithms and macros

The following macros are recommended to use, and they safe/have no side-effects.

## Scope macros (RAII)
### c_AUTO, c_WITH, c_SCOPE, c_DEFER
General ***defer*** mechanics for resource acquisition. These macros allows you to specify the
freeing of the resources at the point where the acquisition takes place.
The **checkauto** utility described below, ensures that the `c_AUTO*` macros are used correctly.

| Usage                                  | Description                                               |
|:---------------------------------------|:----------------------------------------------------------|
| `c_WITH (Type var=init, drop)`         | Declare `var`. Defer `drop...` to end of scope            |
| `c_WITH (Type var=init, pred, drop)`   | Adds a predicate in order to exit early if init failed    |
| `c_AUTO (Type, var1,...,var4)`         | `c_WITH (Type var1=Type_init(), Type_drop(&var1))` ...    |
| `c_SCOPE (init, drop)`                 | Execute `init` and defer `drop` to end of scope           |
| `c_DEFER (drop...)`                    | Defer `drop...` to end of scope                           |
| `continue`                             | Exit a block above without memory leaks                   |

For multiple variables, use either multiple **c_WITH** in sequence, or declare variable outside
scope and use **c_SCOPE**. For convenience, **c_AUTO** support up to 4 variables.
```c
// `c_WITH` is similar to python `with`: it declares and can drop a variable after going out of scope.
bool ok = false;
c_WITH (uint8_t* buf = malloc(BUF_SIZE), buf != NULL, free(buf))
c_WITH (FILE* fp = fopen(fname, "rb"), fp != NULL, fclose(fp))
{
    int n = fread(buf, 1, BUF_SIZE, fp);
    if (n <= 0) continue; // auto cleanup! NB do not break or return here.
    ...
    ok = true;
}
return ok;

// `c_AUTO` automatically initialize and destruct up to 4 variables, like c_WITH.
c_AUTO (cstr, s1, s2)
{
    cstr_append(&s1, "Hello");
    cstr_append(&s1, " world");

    cstr_append(&s2, "Cool");
    cstr_append(&s2, " stuff");

    printf("%s %s\n", cstr_str(&s1), cstr_str(&s2));
}

c_WITH (cstr str = cstr_lit("Hello"), cstr_drop(&str))
{
    cstr_append(&str, " world");
    printf("%s\n", cstr_str(&str));
}

// `c_SCOPE` is like `c_WITH` but works with an already declared variable.
static pthread_mutex_t mut;
c_SCOPE (pthread_mutex_lock(&mut), pthread_mutex_unlock(&mut))
{
    /* Do syncronized work. */
}

// `c_DEFER` executes the expressions when leaving scope. Prefer c_WITH or c_SCOPE.
cstr s1 = cstr_lit("Hello"), s2 = cstr_lit("world");
c_DEFER (cstr_drop(&s1), cstr_drop(&s2))
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

    c_WITH (FILE* fp = fopen(name, "r"), fp != NULL, fclose(fp))
    c_WITH (cstr line = cstr_NULL, cstr_drop(&line))
        while (cstr_getline(&line, fp))
            cvec_str_emplace_back(&vec, cstr_str(&line));
    return vec;
}

int main()
{
    c_WITH (cvec_str x = readFile(__FILE__), cvec_str_drop(&x))
        c_FOREACH (i, cvec_str, x)
            printf("%s\n", cstr_str(i.ref));
}
```

### The **checkauto** utility program (for RAII)
The **checkauto** program will check the source code for any misuses of the `c_AUTO*` macros which
may lead to resource leakages. The `c_AUTO*`- macros are implemented as one-time executed **for-loops**,
so any `return` or `break` appearing within such a block will lead to resource leaks, as it will disable
the cleanup/drop method to be called. A `break` may originally be intended to break a loop or switch
outside the `c_AUTO` scope.

NOTE: One must always make sure to unwind temporary allocated resources before a `return` in C. However, by using `c_AUTO*`-macros,
- it is much easier to automatically detect misplaced return/break between resource acquisition and destruction.
- it prevents forgetting to call the destructor at the end.

The **checkauto** utility will report any misusages. The following example shows how to correctly break/return
from a `c_AUTO` scope: 
```c
    int flag = 0;
    for (int i = 0; i<n; ++i) {
        c_AUTO (cstr, text)
        c_AUTO (List, list)
        {
            for (int j = 0; j<m; ++j) {
                List_push_back(&list, i*j);
                if (cond1())
                    break;  // OK: breaks current for-loop only
            }
            // WRONG:
            if (cond2())
                break;      // checkauto ERROR! break inside c_AUTO.

            if (cond3())
                return -1;  // checkauto ERROR! return inside c_AUTO
            
            // CORRECT:
            if (cond2()) {
                flag = 1;   // flag to break outer for-loop
                continue;   // cleanup and leave c_AUTO block
            }
            if (cond3()) {
                flag = -1;  // return -1
                continue;   // cleanup and leave c_AUTO block
            }
            ...
        }
        // do the return/break outside of c_AUTO
        if (flag < 0) return flag;
        else if (flag > 0) break;
        ...
    } // for
```
## Loop abstraction macros

### c_FORLIST
Iterate compound literal array elements. Additional to `i.ref`, you can access `i.data`, `i.size`, and `i.index` of the input list/element.
```c
// apply multiple push_backs
c_FORLIST (i, int, {1, 2, 3})
    cvec_i_push_back(&vec, *i.ref);

// insert in existing map
c_FORLIST (i, cmap_ii_raw, { {4, 5}, {6, 7} })
    cmap_ii_insert(&map, i.ref->first, i.ref->second);

// string literals pushed to a stack of cstr:
c_FORLIST (i, const char*, {"Hello", "crazy", "world"})
    cstack_str_emplace(&stk, *i.ref);

// reverse the list:
c_FORLIST (i, int, {1, 2, 3})
    cvec_i_push_back(&vec, i.data[i.size - 1 - i.index]);
```

### c_FOREACH, c_FORPAIR

| Usage                                    | Description                     |
|:-----------------------------------------|:--------------------------------|
| `c_FOREACH (it, ctype, container)`       | Iteratate all elements          |
| `c_FOREACH (it, ctype, it1, it2)`        | Iterate the range [it1, it2)    |
| `c_FORPAIR (key, val, ctype, container)` | Iterate with structured binding |

```c
#define i_key int
#define i_val int
#define i_tag ii
#include <stc/csmap.h>
...
c_FORLIST (i, csmap_ii_raw, { {23,1}, {3,2}, {7,3}, {5,4}, {12,5} })
    csmap_ii_insert(&map, i.ref->first, i.ref->second);

c_FOREACH (i, csmap_ii, map)
    printf(" %d", i.ref->first);
// 3 5 7 12 23

csmap_ii_iter it = csmap_ii_find(&map, 7);
c_FOREACH (i, csmap_ii, it, csmap_ii_end(&map))
    printf(" %d", i.ref->first);
// 7 12 23

c_FORPAIR (id, count, csmap_ii, map)
    printf(" (%d %d)", *_.id, *_.count);
// (3 2) (5 4) (7 3) (12 5) (23 1)
```

### c_FORRANGE
Abstraction for iterating sequence of numbers. Like python's **for** *i* **in** *range()* loop.

| Usage                                       | Python equivalent                    |
|:--------------------------------------------|:-------------------------------------|
| `c_FORRANGE (stop)`                          | `for _ in range(stop):`              |
| `c_FORRANGE (i, stop) // i type = long long` | `for i in range(stop):`              |
| `c_FORRANGE (i, start, stop)`                | `for i in range(start, stop):`       |
| `c_FORRANGE (i, start, stop, step)`          | `for i in range(start, stop, step):` |

```c
c_FORRANGE (5) printf("x");
// xxxxx
c_FORRANGE (i, 5) printf(" %lld", i);
// 0 1 2 3 4
c_FORRANGE (i, -3, 3) printf(" %lld", i);
// -3 -2 -1 0 1 2
c_FORRANGE (i, 30, 0, -5) printf(" %lld", i);
// 30 25 20 15 10 5
```

### c_FORWHILE, c_FORFILTER
Iterate containers with stop-criteria and chained range filtering.

| Usage                                               | Description                            |
|:----------------------------------------------------|:---------------------------------------|
| `c_FORWHILE (it, ctype, start, pred)`               | Iterate until pred is false            |
| `c_FORFILTER (it, ctype, container, filter)`        | Filter out items in chain with &&      |
| `c_FORFILTER (it, ctype, container, filter, pred)`  | Filter and iterate until pred is false |

| Built-in filter                   | Description                          |
|:----------------------------------|:-------------------------------------|
| `c_FLT_SKIP(it, numItems)`        | Skip numItems                        |
| `c_FLT_TAKE(it, numItems)`        | Take numItems                        |
| `c_FLT_SKIPWHILE(it, predicate)`  | Skip items until predicate is false  |
| `c_FLT_TAKEWHILE(it, predicate)`  | Take items until predicate is false  |

`it.index` holds the index of the source item, and `it.count` the current number of items taken.
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
    c_AUTO (IVec, vec) {
        c_FORRANGE (i, 1000) IVec_push(&vec, 1000000 + i);

        c_FORFILTER (i, IVec, vec,
                        isOdd(*i.ref)
                     && c_FLT_SKIP(i, 100) // built-in
                     && isPrime(*i.ref)
                      , c_FLT_TAKE(i, 10)) { // breaks loop on false.
            printf(" %d", *i.ref);  
        }
        puts("");
    }
}
// Out: 1000211 1000213 1000231 1000249 1000253 1000273 1000289 1000291 1000303 1000313 
```
Note that `c_FLT_TAKE()` is given as an optional argument, which makes the loop stop when it becomes false (for efficiency). Chaining it after `isPrime()` instead will give same result, but the full input is processed.

### crange
**crange** is a number sequence generator type. The **crange_value** type is `long long`. Below, *start*, *stop*, *step* are type *crange_value*:
```c
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
c_FORFILTER (i, crange, r1 
              , isPrime(*i.ref))
    printf(" %lld", *i.ref);
// 2 3 5 7 11 13 17 19 23 29 31

// 2. The 11 first primes:
printf("2");
c_FORFILTER (i, crange, crange_literal(3, INT64_MAX, 2) 
              , isPrime(*i.ref)
              , c_FLT_TAKE(10))
    printf(" %lld", *i.ref);
// 2 3 5 7 11 13 17 19 23 29 31
```
### c_FIND_IF, c_ERASE_IF
Find or erase linearily in containers using a predicate
```c
// Search vec for first value > 2:
cvec_i_iter i;
c_FIND_IF(i, cvec_i, vec, *i.ref > 2);
if (i.ref) printf("%d\n", *i.ref);

// Search map for a string containing "hello" and erase it:
cmap_str_iter it, it1 = ..., it2 = ...;
c_FIND_IF(it, csmap_str, it1, it2, cstr_contains(it.ref, "hello"));
if (it.ref) cmap_str_erase_at(&map, it);

// Erase all strings containing "hello":
// Note 1: iter i need not be declared.
// Note 2: variables index and count can be accessed in predicate.
c_ERASE_IF(i, csmap_str, map, cstr_contains(i.ref, "hello"));
```

### c_NEW, c_ALLOC, c_ALLOC_N, c_DROP

| Usage                          | Meaning                                 |
|:-------------------------------|:----------------------------------------|
| `c_NEW (type, value)`          | Move value to a new object on the heap  |
| `c_ALLOC (type)`               | `(type *) c_MALLOC(sizeof(type))`       |
| `c_ALLOC_N (type, N)`          | `(type *) c_MALLOC((N)*sizeof(type))`   |
| `c_DROP (ctype, &c1, ..., &cN)` | `ctype_drop(&c1); ... ctype_drop(&cN)` |

```c
struct Pnt { double x, y, z; };
struct Pnt *pnt = c_NEW (struct Pnt, {1.2, 3.4, 5.6});
c_FREE(pnt);

int* array = c_ALLOC_N (int, 100);
c_FREE(array);

cstr a = cstr_lit("Hello"), b = cstr_lit("World");
c_DROP(cstr, &a, &b);
```

### General predefined template parameter functions
```c
int         c_default_cmp(const Type*, const Type*);
Type        c_default_clone(Type val);           // simple copy
Type        c_default_toraw(const Type* val);    // dereference val
void        c_default_drop(Type* val);           // does nothing

typedef     const char* crawstr;
int         crawstr_cmp(const crawstr* x, const crawstr* y);
bool        crawstr_eq(const crawstr* x, const crawstr* y);
uint64_t    crawstr_hash(const crawstr* x);
```

### c_MALLOC, c_CALLOC, c_REALLOC, c_FREE
Memory allocator for the entire library. Macros can be overridden by the user.

### c_SWAP, c_ARRAYLEN
- **c_SWAP(T, xp, yp)**: Safe macro for swapping internals of two objects of same type.
- **c_ARRAYLEN(array)**: Return number of elements in an array.
```c
cmap_int map1 = {0}, map2 = {0};
...
c_SWAP(cmap_int, &map1, &map2);

int array[] = {1, 2, 3, 4};
size_t n = c_ARRAYLEN(array);
```
