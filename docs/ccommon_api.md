# STC [ccommon](../stc/ccommon.h): Common definitions and safe macros

This describes the features the ccommon.h header file.

## Macros
The following macros a completely safe to use, with no side-effects.

#### c_new, c_del
- Type* c_new (VType)
- Type* c_new (VType, size_t N)
- c_del (CType, CType* x1, ..., CType* xN)

#### c_malloc, c_calloc, c_realloc, c_free
Macros that can be overloaded by user to use a different allocator for the entire library

#### c_foreach
- c_foreach (it, CType, container)
```c
using_cvec(x, double);
...
cvec_x vec = cvec_x_init();
double sum = 0;
c_foreach (i, cvec_x, vec) sum += *i.ref;
```

#### c_forrange
Declare an iterator and specify a range to iterate with a for loop. Like python's ***range()*** function:

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
c_forrange (i, 5) printf(" %zu");
// 0 1 2 3 4
c_forrange (i, int, -3, 3) printf(" %d", i);
// -3 -2 -1 0 1 2
c_forrange (i, int, 30, 0, -5) printf(" %d", i);
// 30 25 20 15 10 5
```

#### c_withbuffer

#### c_withfile

#### c_push_items

#### c_swap
