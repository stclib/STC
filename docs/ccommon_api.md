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
Declare an iterator and specify a range to iterate with a for loop. 
- c_forrange (end)
- c_forrange (it, end)
- c_forrange (it, IterType, end)
- c_forrange (it, IterType, begin, end)
- c_forrange (it, IterType, begin, end, step)

#### c_withbuffer

#### c_withfile

#### c_push_items

#### c_swap
