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

static inline void* MyCalloc(MyAlloc* m, isize_t nitems, isize_t item_sz)
    { m->bytes += nitems * item_sz; return calloc((size_t)nitems, (size_t)item_sz); }

static inline void* MyRealloc(MyAlloc* m, void* p, isize_t old_sz, isize_t sz)
    { m->bytes += sz - old_sz; return realloc(p, (size_t)sz); }

static inline void MyFree(MyAlloc* m, void* p, isize_t sz)
    { puts("free"); m->bytes -= sz; free(p); }

#define my_malloc(sz)              MyMalloc(self->aux, sz)
#define my_calloc(nitems, item_sz) MyCalloc(self->aux, nitems, item_sz)
#define my_realloc(p, old_sz, sz)  MyRealloc(self->aux, p, old_sz, sz)
#define my_free(p, sz)             MyFree(self->aux, p, sz)

#endif

