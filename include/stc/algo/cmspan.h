/*
#include <stdio.h>
#include <stc/algo/cmspan.h>
using_cmspan(S3f, float, 3);

int main()
{
    float raw[3*4*5];
    S3f span = cmspan_make(raw, 3, 4, 5);
    *cmspan_at(&span, 2, 3, 4) = 100;
    
    printf("%f\n", *cmspan_at(&span, 2, 3, 4));
}
*/
#ifndef STC_CMSPAN_H_INCLUDED
#define STC_CMSPAN_H_INCLUDED

#include <stc/ccommon.h>

#define using_cmspan(Self, T, DIM) \
    typedef struct { T *data; uint32_t dim[DIM]; } Self; \
    typedef T Self##_raw, Self##_value; \
    typedef struct { Self##_value *ref, *end; } Self##_iter; \
    \
    STC_INLINE Self##_iter Self##_begin(const Self* self) { \
        Self##_iter it = {self->data, self->data + cmspan_size(self)}; \
        return it; \
    } \
    STC_INLINE Self##_iter Self##_end(const Self* self) { \
        Self##_iter it = {NULL, self->data + cmspan_size(self)}; \
        return it; \
    } \
    STC_INLINE void Self##_next(Self##_iter* it) \
        { if (++it->ref == it->end) it->ref = NULL; } \
    struct stc_nostruct

#define cmspan_assert(self, rank) c_STATIC_ASSERT(cmspan_rank(self) == rank)

#define cmspan_init() {NULL}
#define cmspan_make(data, ...) {data, {__VA_ARGS__}}

#define cmspan_reshape(self, ...) \
    memcpy((self)->dim, (uint32_t[]){__VA_ARGS__}, \
            sizeof((self)->dim) + cmspan_assert(self, c_NUMARGS(__VA_ARGS__)))

#define cmspan_at(self, ...) \
    ((self)->data + c_PASTE(_cmspan_i, c_NUMARGS(__VA_ARGS__))((self)->dim, __VA_ARGS__) \
                  + cmspan_assert(self, c_NUMARGS(__VA_ARGS__)))

#define cmspan_size(self) _cmspan_size((self)->dim, cmspan_rank(self))
#define cmspan_rank(self) c_ARRAYLEN((self)->dim)

STC_INLINE uint32_t _cmspan_i1(const uint32_t dim[1], uint32_t x)
    { assert(x < dim[0]); return x; }

STC_INLINE uint32_t _cmspan_i2(const uint32_t dim[2], uint32_t x, uint32_t y)
    { assert(x < dim[0] && y < dim[1]); return dim[1]*x + y; }

STC_INLINE uint32_t _cmspan_i3(const uint32_t dim[3], uint32_t x, uint32_t y, uint32_t z) {
    assert(x < dim[0] && y < dim[1] && z < dim[2]);
    return dim[2]*(dim[1]*x + y) + z;
}
STC_INLINE uint32_t _cmspan_i4(const uint32_t dim[4], uint32_t x, uint32_t y, uint32_t z, uint32_t w) {
    assert(x < dim[0] && y < dim[1] && z < dim[3] && w < dim[3]);
    return dim[3]*(dim[2]*(dim[1]*x + y) + z) + w;
}

STC_INLINE size_t _cmspan_size(const uint32_t dim[], unsigned rank) {
    size_t sz = dim[0];
    while (rank --> 1) sz *= dim[rank];
    return sz;
}

#endif
