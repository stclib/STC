/*
 MIT License
 *
 * Copyright (c) 2023 Tyge Løvset
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/*
#include <stdio.h>
#include <stc/cspan.h>
#include <stc/algo/filter.h>
using_cspan(Span2f, float, 2);
using_cspan(Intspan, int, 1);

int demo1() {
    float raw[4*5];
    Span2f ms = cspan_md(raw, 4, 5);
    
    for (size_t i=0; i<ms.dim[0]; i++)
        for (size_t j=0; j<ms.dim[1]; j++)
            *cspan_at(&ms, i, j) = i*1000 + j;

    printf("%f\n", *cspan_at(&ms, 3, 4));
}

int demo2() {
    int array[] = {10, 20, 30, 23, 22, 21};
    Intspan span = cspan_from_array(array);
    
    c_FOREACH (i, Intspan, span)
        printf(" %d", *i.ref);
    puts("");
    
    c_FORFILTER (i, Intspan, span,
                  , c_flt_skipwhile(i, *i.ref < 25)
                 && (*i.ref & 1) == 0 // even only
                  , c_flt_take(i, 2)) // break after 2
        printf(" %d", *i.ref);
    puts("");
}
*/
#ifndef STC_CSPAN_H_INCLUDED
#define STC_CSPAN_H_INCLUDED

#include "ccommon.h"

#define using_cspan(Self, T, RANK) \
    typedef T Self##_value; typedef T Self##_raw; \
    typedef struct { \
        Self##_value *data; \
        uint32_t dim[RANK]; \
        cspan_idx##RANK stride; \
    } Self; \
    typedef struct { Self##_value *ref; uint32_t pos[RANK]; const Self *_s; } Self##_iter; \
    \
    STC_INLINE Self Self##_from_n(Self##_raw* raw, const size_t n) { \
        return (Self){.data=raw, .dim={(uint32_t)n}}; \
    } \
    STC_INLINE Self##_iter Self##_begin(const Self* self) { \
        Self##_iter it = {.ref=self->data, .pos={0}, ._s=self}; \
        return it; \
    } \
    STC_INLINE Self##_iter Self##_end(const Self* self) { \
        Self##_iter it = {.ref=NULL}; \
        return it; \
    } \
    STC_INLINE void Self##_next(Self##_iter* it) { \
        it->ref += _cspan_next_##RANK(RANK, it->pos, it->_s->dim, it->_s->stride.d); \
        if (it->pos[0] == it->_s->dim[0]) it->ref = NULL; \
    } \
    struct stc_nostruct

#define using_cspan2(Self, T) using_cspan(Self, T, 1); using_cspan(Self##2, T, 2)
#define using_cspan3(Self, T) using_cspan2(Self, T); using_cspan(Self##3, T, 3)
#define using_cspan4(Self, T) using_cspan3(Self, T); using_cspan(Self##4, T, 4)
typedef struct { uint32_t d[1]; } cspan_idx1;
typedef struct { uint32_t d[2]; } cspan_idx2;
typedef struct { uint32_t d[3]; } cspan_idx3;
typedef struct { uint32_t d[4]; } cspan_idx4;

#define cspan_md(array, ...) \
    {.data=array, .dim={__VA_ARGS__}, .stride={.d={__VA_ARGS__}}}

/* For static initialization, use cspan_make(). c_make() for non-static only. */
#define cspan_make(SpanType, ...) \
    {.data=(SpanType##_value[])__VA_ARGS__, .dim={sizeof((SpanType##_value[])__VA_ARGS__)/sizeof(SpanType##_value)}}

/* create a cspan from a cvec, cstack, cdeq, cqueue, or cpque (heap) */
#define cspan_from(container) \
    {.data=(container)->data, .dim={(uint32_t)(container)->_len}}

#define cspan_from_array(array) \
    {.data=(array) + c_static_assert(sizeof(array) != sizeof(void*)), .dim={c_ARRAYLEN(array)}}

#define cspan_size(self) _cspan_size((self)->dim, cspan_rank(self))
#define cspan_rank(self) c_ARRAYLEN((self)->dim)
#define cspan_index(self, ...) \
    c_PASTE(_cspan_i, c_NUMARGS(__VA_ARGS__))((self)->dim, (self)->stride, __VA_ARGS__)

#define cspan_at(self, ...) ((self)->data + cspan_index(self, __VA_ARGS__))
#define cspan_front(self) ((self)->data)
#define cspan_back(self) ((self)->data + cspan_size(self) - 1)

// cspan_subspanN:

#define cspan_subspan(self, offset, count) \
    {.data=cspan_at(self, offset), .dim={count}}
#define cspan_subspan2(self, offset, count) \
    {.data=cspan_at(self, offset, 0), .dim={count, (self)->dim[1]}, .stride={(self)->stride}}
#define cspan_subspan3(self, offset, count) \
    {.data=cspan_at(self, offset, 0, 0), .dim={count, (self)->dim[1], (self)->dim[2]}, \
                                         .stride={(self)->stride}}
#define cspan_subspan4(self, offset, count) \
    {.data=cspan_at(self, offset, 0, 0, 0), .dim={count, (self)->dim[1], (self)->dim[2], (self)->dim[3]}, \
                                            .stride={(self)->stride}}

// cspan_submdN:

#define cspan_submd4(...) c_MACRO_OVERLOAD(cspan_submd4, __VA_ARGS__)
#define cspan_submd3(...) c_MACRO_OVERLOAD(cspan_submd3, __VA_ARGS__)
#define cspan_submd2(self, x) \
    {.data=cspan_at(self, x, 0), .dim={(self)->dim[1]}}
#define cspan_submd3_2(self, x) \
    {.data=cspan_at(self, x, 0, 0), .dim={(self)->dim[1], (self)->dim[2]}, \
                                    .stride={.d={0, (self)->stride.d[2]}}}
#define cspan_submd3_3(self, x, y) \
    {.data=cspan_at(self, x, y, 0), .dim={(self)->dim[2]}}
#define cspan_submd4_2(self, x) \
    {.data=cspan_at(self, x, 0, 0, 0), .dim={(self)->dim[1], (self)->dim[2], (self)->dim[3]}, \
                                       .stride={.d={0, (self)->stride.d[2], (self)->stride.d[3]}}}
#define cspan_submd4_3(self, x, y) \
    {.data=cspan_at(self, x, y, 0, 0), .dim={(self)->dim[2], (self)->dim[3]}, \
                                       .stride={.d={0, (self)->stride.d[3]}}}
#define cspan_submd4_4(self, x, y, z) \
    {.data=cspan_at(self, x, y, z, 0), .dim={(self)->dim[3]}}

// cspan_slice:
//  e.g.: cspan_slice(&ms3, {1,3}, {0}, {1,4});

#define cspan_slice(self, ...) \
    ((void)((self)->data += _cspan_slice(cspan_rank(self), (self)->dim, (self)->stride.d, \
                                         (const uint32_t[][2]){__VA_ARGS__}) + \
                            c_static_assert(cspan_rank(self) == \
                            sizeof((const uint32_t[][2]){__VA_ARGS__})/8)))

// FUNCTIONS

STC_INLINE size_t _cspan_i1(const uint32_t dim[1], const cspan_idx1 stri, uint32_t x)
    { c_ASSERT(x < dim[0]); return x; }

STC_INLINE size_t _cspan_i2(const uint32_t dim[2], const cspan_idx2 stri, uint32_t x, uint32_t y)
    { c_ASSERT(x < dim[0] && y < dim[1]); return stri.d[1]*x + y; }

STC_INLINE size_t _cspan_i3(const uint32_t dim[3], const cspan_idx3 stri, uint32_t x, uint32_t y, uint32_t z) {
    c_ASSERT(x < dim[0] && y < dim[1] && z < dim[2]);
    return stri.d[2]*(stri.d[1]*x + y) + z;
}
STC_INLINE size_t _cspan_i4(const uint32_t dim[4], const cspan_idx4 stri, uint32_t x, uint32_t y, \
                                                                          uint32_t z, uint32_t w) {
    c_ASSERT(x < dim[0] && y < dim[1] && z < dim[2] && w < dim[3]);
    return stri.d[3]*(stri.d[2]*(stri.d[1]*x + y) + z) + w;
}

STC_INLINE size_t _cspan_size(const uint32_t dim[], unsigned rank) {
    size_t sz = dim[0];
    while (rank-- > 1) sz *= dim[rank];
    return sz;
}

#define _cspan_next_1(r, pos, d, s) (++pos[0], 1)
#define _cspan_next_3 _cspan_next_2
#define _cspan_next_4 _cspan_next_2

static inline size_t _cspan_next_2(int rank, uint32_t pos[], const uint32_t dim[], const uint32_t stride[]) {
    size_t off = 1, rs = 1;
    ++pos[rank - 1];
    while (--rank && pos[rank] == dim[rank]) {
        pos[rank] = 0, ++pos[rank - 1];
        const size_t ds = rs*dim[rank];
        rs *= stride[rank];
        off += rs - ds;
    }
    return off;
}

STC_INLINE size_t _cspan_slice(int rank, uint32_t dim[], const uint32_t stri[], const uint32_t a[][2]) {
    uint32_t t = a[0][1] ? a[0][1] : dim[0];
    c_ASSERT(t <= dim[0]);
    dim[0] = t - a[0][0];

    size_t off = a[0][0];
    for (int i = 1; i < rank; ++i) {
        off *= stri[i];
        off += a[i][0];
        t = a[i][1] ? a[i][1] : dim[i];
        c_ASSERT(t <= dim[i]);
        dim[i] = t - a[i][0];
    }
    return off;
}

#endif
