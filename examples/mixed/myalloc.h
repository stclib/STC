#ifndef MyAlloc_INCLUDED
#define MyAlloc_INCLUDED

#include <stdlib.h>
#include <stc/common.h>

struct MyAlloc { isize count; };
static inline void* MyMalloc(struct MyAlloc* m, isize sz)
    { m->count += sz; return c_malloc(sz); }
static inline void* MyCalloc(struct MyAlloc* m, isize nitems, isize item_sz)
    { m->count += nitems * item_sz; return c_calloc(nitems, item_sz); }
static inline void* MyRealloc(struct MyAlloc* m, void* p, isize old_sz, isize sz)
    { m->count += sz - old_sz; return c_realloc(p, old_sz, sz); }
static inline void MyFree(struct MyAlloc* m, void* p, isize sz)
    { m->count -= sz; c_free(p, sz); }

#define my_malloc(sz) MyMalloc(self->aux.alloc, sz)
#define my_calloc(nitems, item_sz) MyCalloc(self->aux.alloc, nitems, item_sz)
#define my_realloc(p, old_sz, sz) MyRealloc(self->aux.alloc, p, old_sz, sz)
#define my_free(p, sz)  MyFree(self->aux.alloc, p, sz)

#endif

#define i_aux struct { struct MyAlloc* alloc; }
#define i_allocator my
