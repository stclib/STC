#ifndef MyAlloc_INCLUDED
#define MyAlloc_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <stc/common.h>

typedef struct {
    isize_t bytes;
} MyAlloc;

static inline void* MyMalloc(MyAlloc* m, isize_t sz)
    { m->bytes += sz; return malloc((size_t)sz); }

static inline void* MyRealloc(MyAlloc* m, void* p, isize_t sz, isize_t new_sz)
    { m->bytes += new_sz - sz; return realloc(p, (size_t)new_sz); }

static inline void MyFree(MyAlloc* m, void* p, isize_t sz)
    { puts("free"); m->bytes -= sz; free(p); }

#define myalloc_malloc(sz)              MyMalloc(self->aux, sz)
#define myalloc_realloc(p, old_sz, sz)  MyRealloc(self->aux, p, old_sz, sz)
#define myalloc_free(p, sz)             MyFree(self->aux, p, sz)

#endif
