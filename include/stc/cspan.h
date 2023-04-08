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
    
    for (int i=0; i<ms.shape[0]; i++)
        for (int j=0; j<ms.shape[1]; j++)
            *cspan_at(&ms, i, j) = i*1000 + j;

    printf("%f\n", *cspan_at(&ms, 3, 4));
}

int demo2() {
    int array[] = {10, 20, 30, 23, 22, 21};
    Intspan span = cspan_from_array(array);
    
    c_foreach (i, Intspan, span)
        printf(" %d", *i.ref);
    puts("");
    
    c_forfilter (i, Intspan, span,
        c_flt_skipwhile(i, *i.ref < 25) &&
        (*i.ref & 1) == 0               && // even only
        c_flt_take(i, 2)                   // break after 2
    ){
        printf(" %d", *i.ref);
    }
    puts("");
}
*/
#ifndef STC_CSPAN_H_INCLUDED
#define STC_CSPAN_H_INCLUDED

#include "ccommon.h"

#define using_cspan(...) c_MACRO_OVERLOAD(using_cspan, __VA_ARGS__)
#define using_cspan_2(Self, T) \
    using_cspan_3(Self, T, 1)

#define using_cspan_3(Self, T, RANK) \
    typedef T Self##_value; typedef T Self##_raw; \
    typedef struct { \
        Self##_value *data; \
        int32_t shape[RANK]; \
        cspan_idx##RANK stride; \
    } Self; \
    \
    typedef struct { Self##_value *ref; int32_t pos[RANK]; const Self *_s; } Self##_iter; \
    \
    STC_INLINE Self Self##_from_n(Self##_raw* raw, const intptr_t n) { \
        return (Self){.data=raw, .shape={(int32_t)n}}; \
    } \
    STC_INLINE Self Self##_slice_(Self##_value* v, const int32_t shape[], const int32_t stri[], \
                                     const int rank, const int32_t a[][2]) { \
        Self s = {.data=v}; int outrank; \
        s.data += _cspan_slice(s.shape, s.stride.d, &outrank, shape, stri, rank, a); \
        c_ASSERT(outrank == RANK); \
        return s; \
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
        it->ref += _cspan_next##RANK(RANK, it->pos, it->_s->shape, it->_s->stride.d); \
        if (it->pos[0] == it->_s->shape[0]) it->ref = NULL; \
    } \
    struct stc_nostruct

#define using_cspan2(Self, T) using_cspan_3(Self, T, 1); using_cspan_3(Self##2, T, 2)
#define using_cspan3(Self, T) using_cspan2(Self, T); using_cspan_3(Self##3, T, 3)
#define using_cspan4(Self, T) using_cspan3(Self, T); using_cspan_3(Self##4, T, 4)
typedef struct { int32_t d[1]; } cspan_idx1;
typedef struct { int32_t d[2]; } cspan_idx2;
typedef struct { int32_t d[3]; } cspan_idx3;
typedef struct { int32_t d[4]; } cspan_idx4;
typedef struct { int32_t d[5]; } cspan_idx5;
typedef struct { int32_t d[6]; } cspan_idx6;
#define c_END -1
#define c_ALL 0,-1

#define cspan_md(array, ...) \
    {.data=array, .shape={__VA_ARGS__}, .stride={.d={__VA_ARGS__}}}

/* For static initialization, use cspan_make(). c_make() for non-static only. */
#define cspan_make(SpanType, ...) \
    {.data=(SpanType##_value[])__VA_ARGS__, .shape={sizeof((SpanType##_value[])__VA_ARGS__)/sizeof(SpanType##_value)}}

#define cspan_slice(OutSpan, parent, ...) \
    OutSpan##_slice_((parent)->data, (parent)->shape, (parent)->stride.d, cspan_rank(parent) + \
                     c_static_assert(cspan_rank(parent) == sizeof((int32_t[][2]){__VA_ARGS__})/sizeof(int32_t[2])), \
                     (const int32_t[][2]){__VA_ARGS__})
     
/* create a cspan from a cvec, cstack, cdeq, cqueue, or cpque (heap) */
#define cspan_from(container) \
    {.data=(container)->data, .shape={(int32_t)(container)->_len}}

#define cspan_from_array(array) \
    {.data=(array) + c_static_assert(sizeof(array) != sizeof(void*)), .shape={c_arraylen(array)}}

#define cspan_size(self) _cspan_size((self)->shape, cspan_rank(self))
#define cspan_rank(self) c_arraylen((self)->shape)

#define cspan_index(self, ...) c_PASTE(cspan_idx_, c_NUMARGS(__VA_ARGS__))(self, __VA_ARGS__)
#define cspan_idx_1 cspan_idx_4
#define cspan_idx_2 cspan_idx_4
#define cspan_idx_3 cspan_idx_4
#define cspan_idx_4(self, ...) \
    c_PASTE(_cspan_idx, c_NUMARGS(__VA_ARGS__))((self)->shape, (self)->stride, __VA_ARGS__) // small/fast
#define cspan_idx_5(self, ...) \
    (_cspan_idxN(c_NUMARGS(__VA_ARGS__), (self)->shape, (self)->stride.d, (int32_t[]){__VA_ARGS__}) + \
     c_static_assert(cspan_rank(self) == c_NUMARGS(__VA_ARGS__))) // general
#define cspan_idx_6 cspan_idx_5

#define cspan_at(self, ...) ((self)->data + cspan_index(self, __VA_ARGS__))
#define cspan_front(self) ((self)->data)
#define cspan_back(self) ((self)->data + cspan_size(self) - 1)

// cspan_subspanN. (N<4) Optimized, same as e.g. cspan_slice(Span3, &ms3, {off,off+count}, {c_ALL}, {c_ALL});
#define cspan_subspan(self, offset, count) \
    {.data=cspan_at(self, offset), .shape={count}}
#define cspan_subspan2(self, offset, count) \
    {.data=cspan_at(self, offset, 0), .shape={count, (self)->shape[1]}, .stride={(self)->stride}}
#define cspan_subspan3(self, offset, count) \
    {.data=cspan_at(self, offset, 0, 0), .shape={count, (self)->shape[1], (self)->shape[2]}, .stride={(self)->stride}}

// cspan_submdN: reduce rank (N<5) Optimized, same as e.g. cspan_slice(Span2, &ms4, {x}, {y}, {c_ALL}, {c_ALL});
#define cspan_submd4(...) c_MACRO_OVERLOAD(cspan_submd4, __VA_ARGS__)
#define cspan_submd3(...) c_MACRO_OVERLOAD(cspan_submd3, __VA_ARGS__)
#define cspan_submd2(self, x) \
    {.data=cspan_at(self, x, 0), .shape={(self)->shape[1]}}
#define cspan_submd3_2(self, x) \
    {.data=cspan_at(self, x, 0, 0), .shape={(self)->shape[1], (self)->shape[2]}, \
                                    .stride={.d={0, (self)->stride.d[2]}}}
#define cspan_submd3_3(self, x, y) \
    {.data=cspan_at(self, x, y, 0), .shape={(self)->shape[2]}}
#define cspan_submd4_2(self, x) \
    {.data=cspan_at(self, x, 0, 0, 0), .shape={(self)->shape[1], (self)->shape[2], (self)->shape[3]}, \
                                       .stride={.d={0, (self)->stride.d[2], (self)->stride.d[3]}}}
#define cspan_submd4_3(self, x, y) \
    {.data=cspan_at(self, x, y, 0, 0), .shape={(self)->shape[2], (self)->shape[3]}, .stride={.d={0, (self)->stride.d[3]}}}
#define cspan_submd4_4(self, x, y, z) \
    {.data=cspan_at(self, x, y, z, 0), .shape={(self)->shape[3]}}

// private definitions:

STC_INLINE intptr_t _cspan_size(const int32_t shape[], int rank) {
    intptr_t sz = shape[0];
    while (rank-- > 1) sz *= shape[rank];
    return sz;
}

STC_INLINE intptr_t _cspan_idx1(const int32_t shape[1], const cspan_idx1 stri, int32_t x)
    { c_ASSERT(c_LTu(x, shape[0])); return x; }

STC_INLINE intptr_t _cspan_idx2(const int32_t shape[2], const cspan_idx2 stri, int32_t x, int32_t y)
    { c_ASSERT(c_LTu(x, shape[0]) && c_LTu(y, shape[1])); return (intptr_t)stri.d[1]*x + y; }

STC_INLINE intptr_t _cspan_idx3(const int32_t shape[3], const cspan_idx3 stri, int32_t x, int32_t y, int32_t z) {
    c_ASSERT(c_LTu(x, shape[0]) && c_LTu(y, shape[1]) && c_LTu(z, shape[2]));
    return (intptr_t)stri.d[2]*(stri.d[1]*x + y) + z;
}
STC_INLINE intptr_t _cspan_idx4(const int32_t shape[4], const cspan_idx4 stri, int32_t x, int32_t y,
                                                                               int32_t z, int32_t w) {
    c_ASSERT(c_LTu(x, shape[0]) && c_LTu(y, shape[1]) && c_LTu(z, shape[2]) && c_LTu(w, shape[3]));
    return (intptr_t)stri.d[3]*(stri.d[2]*(stri.d[1]*x + y) + z) + w;
}

STC_API intptr_t _cspan_idxN(int rank, const int32_t shape[], const int32_t stri[], const int32_t a[]);
STC_API intptr_t _cspan_next2(int rank, int32_t pos[], const int32_t shape[], const int32_t stride[]);
#define _cspan_next1(r, pos, d, s) (++pos[0], 1)
#define _cspan_next3 _cspan_next2
#define _cspan_next4 _cspan_next2
#define _cspan_next5 _cspan_next2
#define _cspan_next6 _cspan_next2

STC_API intptr_t _cspan_slice(int32_t odim[], int32_t ostri[], int* orank, 
                              const int32_t shape[], const int32_t stri[], 
                              int rank, const int32_t a[][2]);

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined(i_implement)

STC_DEF intptr_t _cspan_idxN(int rank, const int32_t shape[], const int32_t stri[], const int32_t a[]) {
    intptr_t off = a[0];
    c_ASSERT(c_LTu(a[0], shape[0]));
    for (int i = 1; i < rank; ++i) {
        off *= stri[i];
        off += a[i];
        c_ASSERT(c_LTu(a[i], shape[i]));
    }
    return off;
}

STC_DEF intptr_t _cspan_next2(int rank, int32_t pos[], const int32_t shape[], const int32_t stride[]) {
    intptr_t off = 1, rs = 1;
    ++pos[rank - 1];
    while (--rank && pos[rank] == shape[rank]) {
        pos[rank] = 0, ++pos[rank - 1];
        const intptr_t ds = rs*shape[rank];
        rs *= stride[rank];
        off += rs - ds;
    }
    return off;
}

STC_DEF intptr_t _cspan_slice(int32_t odim[], int32_t ostri[], int* orank, 
                              const int32_t shape[], const int32_t stri[], 
                              int rank, const int32_t a[][2]) {
    intptr_t off = 0;
    int i = 0, j = 0;
    int32_t t, s = 1;
    for (; i < rank; ++i) {
        off *= stri[i];
        off += a[i][0];
        switch (a[i][1]) { 
            case 0: s *= stri[i]; c_ASSERT(c_LTu(a[i][0], shape[i])); continue;
            case -1: t = shape[i]; break;
            default: t = a[i][1]; break; 
        }
        odim[j] = t - a[i][0];
        ostri[j] = s*stri[i];
        c_ASSERT(c_LTu(0, odim[j]) & !c_LTu(shape[i], t));
        s = 1; ++j;
    }
    *orank = j;
    return off;
}
#endif
#endif
#undef i_opt
#undef i_header
#undef i_implement
#undef i_static
#undef i_extern
