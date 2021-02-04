# STC [ccommon](../stc/ccommon.h): Common definitions and safe macros

This describes the features the ccommon.h header file.

## Macros
The following macros a completely safe to use, with no side-effects.


#### c_new, c_del

| Usage                          | Meaning                                 |
|:-------------------------------|:----------------------------------------|
| `c_new (type)`                 | `(type *) c_malloc(sizeof(type))`       |
| `c_new (type, N)`              | `(type *) c_malloc((N) * sizeof(type))` |
| `c_del (ctype, c1, ..., cN)`   | `ctype_del(c1); ... ctype_del(cN)`      |
```c
int* array = c_new (int, 100);
c_free(array);

cstr a = cstr_from("Hello"), b = cstr_from("World");
c_del(cstr, &a, &b);
```

#### c_malloc, c_calloc, c_realloc, c_free
Memory allocator for the entire library. Macros can be overloaded by the user.

#### c_forrange
Declare an iterator and specify a range to iterate with a for loop. Like python's ***for i in range()*** function:

| Usage                                         | Python equivalent                    |
|:----------------------------------------------|:-------------------------------------|
| `c_forrange (stop)`                           | `for _ in range(stop):`              |
| `c_forrange (i, stop) // IterType=size_t`     | `for i in range(stop):`              |
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

#### c_foreach

| Usage                                         | Description                     |
|:----------------------------------------------|:--------------------------------|
| `c_foreach (it, ctype, container)`            | `Iteratate all elements `       |
| `c_foreach (it, ctype, it1, it2)`             | `Iterate the range [it1, it2)`  |

```c
using_csset(x, int);
...
c_init (csset_x, set, {23, 3, 7, 5, 12});
c_foreach (i, csset_x, set) printf(" %d", *i.ref);
// 3 5 7 12 23
csset_x_iter_t it = csset_x_find(&set, 7);
c_foreach (i, csset_x, it, csset_x_end(&set)) printf(" %d", *i.ref);
// 7 12 23
```

#### c_withbuffer

#### c_withfile

#### c_push_items

#### c_swap
