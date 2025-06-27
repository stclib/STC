#ifndef MyAlloc_INCLUDED
#define MyAlloc_INCLUDED

#include <stdlib.h>
#include <stc/common.h>

typedef struct {
    isize bytes;
} MyAlloc;

static inline void* MyMalloc(MyAlloc* m, isize sz)
    { m->bytes += sz; return c_malloc(sz); }

static inline void* MyCalloc(MyAlloc* m, isize nitems, isize item_sz)
    { m->bytes += nitems * item_sz; return c_calloc(nitems, item_sz); }

static inline void* MyRealloc(MyAlloc* m, void* p, isize old_sz, isize sz)
    { m->bytes += sz - old_sz; return c_realloc(p, old_sz, sz); }

static inline void MyFree(MyAlloc* m, void* p, isize sz)
    { m->bytes -= sz; c_free(p, sz); }

#define my_malloc(sz)              MyMalloc(self->aux, sz)
#define my_calloc(nitems, item_sz) MyCalloc(self->aux, nitems, item_sz)
#define my_realloc(p, old_sz, sz)  MyRealloc(self->aux, p, old_sz, sz)
#define my_free(p, sz)             MyFree(self->aux, p, sz)

#endif

