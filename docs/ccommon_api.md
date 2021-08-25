# STC [ccommon](../include/stc/ccommon.h): Common definitions and handy macros

The following handy macros are safe to use, i.e. have no side-effects.

### c_fordefer, c_forscope, c_forvar, c_forauto
General ***defer*** mechanics for resource acquisition. These macros allows to specify the release of the
resource where the resource acquisition takes place. Makes it easier to verify that resources are released.

**NB**: These macros are one-time executed **for-loops**. Use ***only*** `continue` in order to break out
of these four `c_for`-blocks. ***Do not*** use `return` or `goto` (or `break`) inside them, as they will
prevent the `end`-statement to be executed when leaving scope. This is not particular to the `c_for*()`
macros, as one must always make sure to unwind temporary allocated resources before a `return` in C.

| Usage                                  | Description                                       |
|:---------------------------------------|:--------------------------------------------------|
| `c_forvar (Type var=init, end...)`     | Declare `var`. Defer `end` to end of block        |
| `c_forauto (Type, var...)`             | `c_forvar (Type var=Type_init(), Type_del(&var))` |
| `c_forscope (start, end)`              | Execute `start`. Defer `end` to end of block      |
| `c_fordefer (end...)`                  | Defer execution of `end` to end of block          |

For multiple variables, use either multiple **c_forvar** in sequence, or declare variable outside
scope and use **c_forscope** or **c_fordefer**. Also, **c_forauto** support up to 3 vars.
```c
c_forvar (cstr s = cstr_lit("Hello"), cstr_del(&s))
{
    cstr_append(&s, " world");
    printf("%s\n", s.str);
}

c_forauto (cstr, s1, s2)
{
    cstr_append(&s1, "Hello");
    cstr_append(&s1, " world");

    cstr_append(&s2, "Cool");
    cstr_append(&s2, " stuff");

    printf("%s %s\n", s1.str, s2.str);
}

using_cvec(i32, int32_t);
...
cvec_i32 vec;
c_forscope (vec = cvec_i32_init(), cvec_i32_del(&vec))
{
    cvec_i32_push_back(&vec, 123);
    cvec_i32_push_back(&vec, 321);

    c_foreach (i, cvec_i32, vec) printf(" %d", *i.ref);
}

cstr s1 = cstr_lit("Hello"), s2 = cstr_lit("world");
c_fordefer (cstr_del(&s1), cstr_del(&s2))
{
    printf("%s %s\n", s1.str, s2.str);
}
```
**Example**: Load each line of a text file into a vector of strings:
```c
#include <errno.h>
#include <stc/cstr.h>
#include <stc/cvec.h>

using_cvec_str();

// receiver should check errno variable
cvec_str readFile(const char* name)
{
    cvec_str vec = cvec_str_init(); // returned

    c_forvar (FILE* fp = fopen(name, "r"), fclose(fp))
        c_forvar (cstr line = cstr_null, cstr_del(&line))
            while (cstr_getline(&line, fp))
                cvec_str_emplace_back(&vec, line.str);
    return vec;
}

int main()
{
    c_forvar (cvec_str x = readFile(__FILE__), cvec_str_del(&x))
        c_foreach (i, cvec_str, x)
            printf("%s\n", i.ref->str);
}
```

### c_foreach

| Usage                                | Description                  |
|:-------------------------------------|:-----------------------------|
| `c_foreach (it, ctype, container)`   | Iteratate all elements       |
| `c_foreach (it, ctype, it1, it2)`    | Iterate the range [it1, it2) |

```c
using_csset(x, int);
...
c_var (csset_x, set, {23, 3, 7, 5, 12});
c_foreach (i, csset_x, set) printf(" %d", *i.ref);
// 3 5 7 12 23
csset_x_iter_t it = csset_x_find(&set, 7);
c_foreach (i, csset_x, it, csset_x_end(&set)) printf(" %d", *i.ref);
// 7 12 23
```

### c_forrange
Declare an iterator and specify a range to iterate with a for loop. Like python's ***for i in range()*** loop:

| Usage                                         | Python equivalent                    |
|:----------------------------------------------|:-------------------------------------|
| `c_forrange (stop)`                           | `for _ in range(stop):`              |
| `c_forrange (i, stop) // IterType = size_t`   | `for i in range(stop):`              |
| `c_forrange (i, IterType, stop)`              | `for i in range(stop):`              |
| `c_forrange (i, IterType, start, stop)`       | `for i in range(start, stop):`       |
| `c_forrange (i, IterType, start, stop, step)` | `for i in range(start, stop, step):` |

```c
c_forrange (5) printf("x");
// xxxxx
c_forrange (i, 5) printf(" %zu", i);
// 0 1 2 3 4
c_forrange (i, int, -3, 3) printf(" %d", i);
// -3 -2 -1 0 1 2
c_forrange (i, int, 30, 0, -5) printf(" %d", i);
// 30 25 20 15 10 5
```

### c_var, c_emplace
**c_var** declares and initializes any container with an array of elements. **c_emplace** adds elements to any existing container:
```c
c_var (cvec_i, vec, {1, 2, 3});     // declare and emplace
c_emplace(cvec_i, vec, {4, 5, 6});  // adds to existing vec
```

### c_new, c_new_n, c_del, c_make

| Usage                          | Meaning                                 |
|:-------------------------------|:----------------------------------------|
| `c_new (type)`                 | `(type *) c_malloc(sizeof(type))`       |
| `c_new_n (type, N)`            | `(type *) c_malloc((N)*sizeof(type))`   |
| `c_del (ctype, &c1, ..., &cN)` | `ctype_del(&c1); ... ctype_del(&cN)`    |
| `c_make(type){value...}`       | `(type){value...}` // c++ compatability |

```c
int* array = c_new_n (int, 100);
c_free(array);

cstr a = cstr_from("Hello"), b = cstr_from("World");
c_del(cstr, &a, &b);
```

### c_malloc, c_calloc, c_realloc, c_free
Memory allocator for the entire library. Macros can be overloaded by the user.

### c_swap, c_arraylen
- **c_swap(type, x, y)**: Simple macro for swapping internals of two objects.
- **c_arraylen(array)**: Return number of elements in an array, e.g. `int array[] = {1, 2, 3, 4};`
