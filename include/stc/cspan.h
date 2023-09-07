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
#include <stc/algorithm.h>
using_cspan(Span2f, float, 2);
using_cspan(Intspan, int);

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

#include "priv/linkage.h"
#include "ccommon.h"

#define using_cspan(...) c_MACRO_OVERLOAD(using_cspan, __VA_ARGS__)
#define using_cspan_2(Self, T) \
    using_cspan_3(Self, T, 1); \
    STC_INLINE Self Self##_from_n(Self##_raw* raw, const intptr_t n) { \
        return (Self){.data=raw, .shape={(int32_t)n}, .stride={.d={1}}}; \
    } \
    struct stc_nostruct

#define using_cspan_3(Self, T, RANK) \
    typedef T Self##_value; typedef T Self##_raw; \
    typedef struct { \
        Self##_value *data; \
        int32_t shape[RANK]; \
        cspan_tuple##RANK stride; \
    } Self; \
    \
    typedef struct { Self##_value *ref; int32_t pos[RANK]; const Self *_s; } Self##_iter; \
    \
    STC_INLINE Self Self##_slice_(Self##_value* d, const int32_t shape[], const intptr_t stri[], \
                                  const int rank, const int32_t a[][3]) { \
        Self s; int outrank; \
        s.data = d + _cspan_slice(s.shape, s.stride.d, &outrank, shape, stri, rank, a); \
        c_assert(outrank == RANK); \
        return s; \
    } \
    STC_INLINE Self##_iter Self##_begin(const Self* self) { \
        Self##_iter it = {.ref=self->data, ._s=self}; \
        return it; \
    } \
    STC_INLINE Self##_iter Self##_end(const Self* self) { \
        Self##_iter it = {0}; \
        return it; \
    } \
    STC_INLINE void Self##_next(Self##_iter* it) { \
        int done; \
        it->ref += _cspan_next##RANK(it->pos, it->_s->shape, it->_s->stride.d, RANK, &done); \
        if (done) it->ref = NULL; \
    } \
    struct stc_nostruct

#define using_cspan2(Self, T) using_cspan_2(Self, T); using_cspan_3(Self##2, T, 2)
#define using_cspan3(Self, T) using_cspan2(Self, T); using_cspan_3(Self##3, T, 3)
#define using_cspan4(Self, T) using_cspan3(Self, T); using_cspan_3(Self##4, T, 4)
#define using_cspan_tuple(N) typedef struct { intptr_t d[N]; } cspan_tuple##N
using_cspan_tuple(1); using_cspan_tuple(2);
using_cspan_tuple(3); using_cspan_tuple(4);
using_cspan_tuple(5); using_cspan_tuple(6);
using_cspan_tuple(7); using_cspan_tuple(8);

#define c_END -1
#define c_ALL 0,c_END
typedef enum {c_ROWMAJOR, c_COLMAJOR} cspan_layout;

/* Use cspan_init() for static initialization only. c_init() for non-static init. */
#define cspan_init(SpanType, ...) \
    {.data=(SpanType##_value[])__VA_ARGS__, .shape={sizeof((SpanType##_value[])__VA_ARGS__)/sizeof(SpanType##_value)}, .stride={.d={1}}}

/* create a cspan from a cvec, cstack, or cpque (heap) */
#define cspan_from(container) \
    {.data=(container)->data, .shape={(int32_t)(container)->_len}, .stride={.d={1}}}

#define cspan_from_n(ptr, n) \
    {.data=(ptr), .shape={n}, .stride={.d={1}}}

#define cspan_from_array(array) \
    cspan_from_n(array, c_arraylen(array))

#define cspan_size(self) _cspan_size((self)->shape, cspan_rank(self))
#define cspan_rank(self) c_arraylen((self)->shape)
#define cspan_is_colmajor(self) ((self)->stride.d[0] < (self)->stride.d[cspan_rank(self) - 1])
#define cspan_is_rowmajor(self) (!cspan_is_colmajor(self))
#define cspan_get_layout(self) (cspan_is_colmajor(self) ? c_COLMAJOR : c_ROWMAJOR)
#define cspan_at(self, ...) ((self)->data + cspan_index(self, __VA_ARGS__))
#define cspan_front(self) ((self)->data)
#define cspan_back(self) ((self)->data + cspan_size(self) - 1)
#define cspan_index(self, ...) \
    (_cspan_index(c_NUMARGS(__VA_ARGS__), (self)->shape, (self)->stride.d, (const int32_t[]){__VA_ARGS__}) + \
     c_static_assert(cspan_rank(self) == c_NUMARGS(__VA_ARGS__))) // general

// cspan_subspanX: (X <= 3) optimized. Similar to cspan_slice(Span3, &ms3, {off,off+count}, {c_ALL}, {c_ALL});
#define cspan_subspan(self, offset, count) \
    {.data=cspan_at(self, offset), .shape={count}, .stride=(self)->stride}
#define cspan_subspan2(self, offset, count) \
    {.data=cspan_at(self, offset, 0), .shape={count, (self)->shape[1]}, .stride=(self)->stride}
#define cspan_subspan3(self, offset, count) \
    {.data=cspan_at(self, offset, 0, 0), .shape={count, (self)->shape[1], (self)->shape[2]}, .stride=(self)->stride}

// cspan_submd(): Reduce rank (N <= 4) Optimized, same as e.g. cspan_slice(Span2, &ms4, {x}, {y}, {c_ALL}, {c_ALL});
#define cspan_submd2(self, x) \
    {.data=cspan_at(self, x, 0), .shape={(self)->shape[1]}, .stride=(cspan_tuple1){.d={(self)->stride.d[1]}}}
#define cspan_submd3(...) c_MACRO_OVERLOAD(cspan_submd3, __VA_ARGS__)
#define cspan_submd3_2(self, x) \
    {.data=cspan_at(self, x, 0, 0), .shape={(self)->shape[1], (self)->shape[2]}, \
                                    .stride=(cspan_tuple2){.d={(self)->stride.d[1], (self)->stride.d[2]}}}
#define cspan_submd3_3(self, x, y) \
    {.data=cspan_at(self, x, y, 0), .shape={(self)->shape[2]}, .stride=(cspan_tuple1){.d={(self)->stride.d[2]}}}
#define cspan_submd4(...) c_MACRO_OVERLOAD(cspan_submd4, __VA_ARGS__)
#define cspan_submd4_2(self, x) \
    {.data=cspan_at(self, x, 0, 0, 0), .shape={(self)->shape[1], (self)->shape[2], (self)->shape[3]}, \
                                       .stride=(cspan_tuple3){.d={(self)->stride.d[1], (self)->stride.d[2], (self)->stride.d[3]}}}
#define cspan_submd4_3(self, x, y) \
    {.data=cspan_at(self, x, y, 0, 0), .shape={(self)->shape[2], (self)->shape[3]}, \
                                       .stride=(cspan_tuple2){.d={(self)->stride.d[2], (self)->stride.d[3]}}}
#define cspan_submd4_4(self, x, y, z) \
    {.data=cspan_at(self, x, y, z, 0), .shape={(self)->shape[3]}, .stride=(cspan_tuple1){.d={(self)->stride.d[3]}}}

#define cspan_md(array, ...) cspan_md_layout(c_ROWMAJOR, array, __VA_ARGS__)
#define cspan_md_layout(layout, array, ...) \
    {.data=array, .shape={__VA_ARGS__}, \
     .stride=*(c_PASTE(cspan_tuple,c_NUMARGS(__VA_ARGS__))*)_cspan_shape2stride(layout, ((intptr_t[]){__VA_ARGS__}), c_NUMARGS(__VA_ARGS__))}

#define cspan_transpose(self) \
    _cspan_transpose((self)->shape, (self)->stride.d, cspan_rank(self))

// General slicing function;
#define cspan_slice(OutSpan, parent, ...) \
    OutSpan##_slice_((parent)->data, (parent)->shape, (parent)->stride.d, cspan_rank(parent) + \
                     c_static_assert(cspan_rank(parent) == sizeof((int32_t[][3]){__VA_ARGS__})/sizeof(int32_t[3])), \
                     (const int32_t[][3]){__VA_ARGS__})

/* ------------------- PRIVAT DEFINITIONS ------------------- */

STC_INLINE intptr_t _cspan_size(const int32_t shape[], int rank) {
    intptr_t sz = shape[0];
    while (--rank) sz *= shape[rank];
    return sz;
}

STC_INLINE void _cspan_transpose(int32_t shape[], intptr_t stride[], int rank) {
    for (int i = 0; i < --rank; ++i) {
        c_swap(int32_t, shape + i, shape + rank);
        c_swap(intptr_t, stride + i, stride + rank);
    }
}

STC_INLINE intptr_t _cspan_index(int rank, const int32_t shape[], const intptr_t stride[], const int32_t a[]) {
    intptr_t off = 0;
    while (rank--) {
        c_assert(c_less_unsigned(a[rank], shape[rank]));
        off += stride[rank]*a[rank];
    }
    return off;
}

#define _cspan_next1(pos, shape, stride, rank, done) (*done = ++pos[0]==shape[0], stride[0])
STC_API intptr_t 
        _cspan_next2(int32_t pos[], const int32_t shape[], const intptr_t stride[], int rank, int* done);
#define _cspan_next3 _cspan_next2
#define _cspan_next4 _cspan_next2
#define _cspan_next5 _cspan_next2
#define _cspan_next6 _cspan_next2
#define _cspan_next7 _cspan_next2
#define _cspan_next8 _cspan_next2

STC_API intptr_t _cspan_slice(int32_t oshape[], intptr_t ostride[], int* orank,
                              const int32_t shape[], const intptr_t stride[],
                              int rank, const int32_t a[][3]);

STC_API intptr_t* _cspan_shape2stride(cspan_layout layout, intptr_t shape[], int rank);
#endif // STC_CSPAN_H_INCLUDED

/* --------------------- IMPLEMENTATION --------------------- */
#if defined(i_implement) || defined(i_static)

STC_DEF intptr_t _cspan_next2(int32_t pos[], const int32_t shape[], const intptr_t stride[], int r, int* done) {
    intptr_t off = stride[--r];
    ++pos[r];

    for (; r && pos[r] == shape[r]; --r) {
        pos[r] = 0; ++pos[r - 1];
        off += stride[r - 1] - stride[r]*shape[r];
    }
    *done = pos[r] == shape[r];
    return off;
}

STC_DEF intptr_t* _cspan_shape2stride(cspan_layout layout, intptr_t stride[], int rank) {
    int i, inc;
    if (layout == c_COLMAJOR) i = 0, inc = 1;
    else i = rank - 1, inc = -1;
    intptr_t k = 1, s1 = stride[i], s2;

    stride[i] = 1;
    while (--rank) {
        i += inc;
        s2 = stride[i];
        stride[i] = (k *= s1);
        s1 = s2;
    }
    return stride;
}

STC_DEF intptr_t _cspan_slice(int32_t oshape[], intptr_t ostride[], int* orank, 
                              const int32_t shape[], const intptr_t stride[], 
                              int rank, const int32_t a[][3]) {
    intptr_t off = 0;
    int i = 0, oi = 0;
    int32_t end;

    for (; i < rank; ++i) {
        off += stride[i]*a[i][0];
        switch (a[i][1]) {
            case 0: c_assert(c_less_unsigned(a[i][0], shape[i])); continue;
            case -1: end = shape[i]; break;
            default: end = a[i][1];
        }
        oshape[oi] = end - a[i][0];
        ostride[oi] = stride[i];
        c_assert((oshape[oi] > 0) & !c_less_unsigned(shape[i], end));
        if (a[i][2]) {
            ostride[oi] *= a[i][2];
            oshape[oi] = (oshape[oi] - 1)/a[i][2] + 1;
        }
        ++oi;
    }
    *orank = oi;
    return off;
}

#endif
#undef i_opt
#undef i_header
#undef i_implement
#undef i_static
#undef i_import
