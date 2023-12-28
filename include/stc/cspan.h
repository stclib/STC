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
#include "stc/cspan.h"
#include "stc/algorithm.h"
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

    c_filter(Intspan, span
         , c_flt_skipwhile(*value < 25)
        && (*value & 1) == 0       // even only
        && (printf(" %d", *value),
            c_flt_take(2))
    );
    puts("");
}
*/
#ifndef STC_CSPAN_INDEX_TYPE
  #define STC_CSPAN_INDEX_TYPE int32_t
#endif

#ifndef STC_CSPAN_H_INCLUDED
#define STC_CSPAN_H_INCLUDED

#include "priv/linkage.h"
#include "common.h"
typedef STC_CSPAN_INDEX_TYPE cextent_t, cstride_t;

#define using_cspan(...) c_MACRO_OVERLOAD(using_cspan, __VA_ARGS__)
#define using_cspan_2(Self, T) \
    using_cspan_3(Self, T, 1); \
    STC_INLINE Self Self##_from_n(Self##_value* values, intptr_t n) { \
        return (Self)cspan_from_n(values, n); \
    } \
    struct stc_nostruct

#define using_cspan_3(Self, T, RANK) \
    typedef T Self##_value; \
    typedef T Self##_raw; \
    typedef struct { \
        Self##_value *data; \
        cextent_t shape[RANK]; \
        cspan_tuple##RANK stride; \
    } Self; \
    \
    typedef struct { \
        Self##_value *ref; \
        const Self *_s; \
        cextent_t pos[RANK]; \
    } Self##_iter; \
    \
    STC_INLINE Self Self##_slice_(Self##_value* d, const cextent_t shape[], const cstride_t stri[], \
                                  const intptr_t args[][3], const int rank) { \
        Self s; int outrank; \
        s.data = d + _cspan_slice(s.shape, s.stride.d, &outrank, shape, stri, args, rank); \
        c_assert(outrank == RANK); \
        return s; \
    } \
    STC_INLINE Self##_iter Self##_begin(const Self* self) { \
        return (Self##_iter){.ref=self->data, ._s=self}; \
    } \
    STC_INLINE Self##_iter Self##_end(const Self* self) { \
        (void)self; \
        return (Self##_iter){0}; \
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
#define using_cspan_tuple(N) typedef struct { cstride_t d[N]; } cspan_tuple##N
using_cspan_tuple(1); using_cspan_tuple(2);
using_cspan_tuple(3); using_cspan_tuple(4);
using_cspan_tuple(5); using_cspan_tuple(6);
using_cspan_tuple(7); using_cspan_tuple(8);

// cspan_init: static construction from initialization list
//
#define cspan_init(Span, ...) \
    ((Span){.data=(Span##_value[])__VA_ARGS__, \
            .shape={sizeof((Span##_value[])__VA_ARGS__)/sizeof(Span##_value)}, \
            .stride=(cspan_tuple1){.d={1}}})

// cspan_from* a pointer+size, c-array, or a cvec/cstack container
//
#define cspan_from_n(ptr, n) \
    {.data=(ptr), \
     .shape={(cextent_t)(n)}, \
     .stride=(cspan_tuple1){.d={1}}}

#define cspan_from_array(array) \
    cspan_from_n(array, c_arraylen(array))

#define cspan_from(container) \
    cspan_from_n((container)->data, (container)->_len)

// cspan_subspan on 1d spans
//
#define cspan_subspan(self, offset, count) \
    {.data=cspan_at(self, offset), \
     .shape={(cextent_t)(count)}, \
     .stride=(self)->stride}

// Accessors
//
#define cspan_size(self) _cspan_size((self)->shape, cspan_rank(self))
#define cspan_rank(self) c_arraylen((self)->shape) // constexpr
#define cspan_is_colmajor(self) ((self)->stride.d[0] < (self)->stride.d[cspan_rank(self) - 1])
#define cspan_is_rowmajor(self) (!cspan_is_colmajor(self))
#define cspan_get_layout(self) (cspan_is_colmajor(self) ? c_COLMAJOR : c_ROWMAJOR)
#define cspan_at(self, ...) ((self)->data + cspan_index(self, __VA_ARGS__))
#define cspan_front(self) ((self)->data)
#define cspan_back(self) ((self)->data + cspan_size(self) - 1)
#define cspan_index(self, ...) \
    _cspan_index((self)->shape, (self)->stride.d, ((const intptr_t[]){__VA_ARGS__}), \
                 cspan_rank(self) + c_static_assert(cspan_rank(self) == c_NUMARGS(__VA_ARGS__)))

// Multi-dimensional span constructors
//
typedef enum {c_ROWMAJOR, c_COLMAJOR} cspan_layout;

#define cspan_md(array, ...) \
    cspan_md_layout(c_ROWMAJOR, array, __VA_ARGS__)

#define cspan_md_layout(layout, array, ...) \
    {.data=array, \
     .shape={__VA_ARGS__}, \
     .stride=*(c_JOIN(cspan_tuple,c_NUMARGS(__VA_ARGS__))*) \
             _cspan_shape2stride(layout, ((cstride_t[]){__VA_ARGS__}), c_NUMARGS(__VA_ARGS__))}

// Transpose and swap axes
//
#define cspan_transposed2(self) \
    {.data=(self)->data + c_static_assert(cspan_rank(self) == 2), \
     .shape={(self)->shape[1], (self)->shape[0]}, \
     .stride=(cspan_tuple2){.d={(self)->stride.d[1], (self)->stride.d[0]}}}

#define cspan_transpose(self) \
    _cspan_transpose((self)->shape, (self)->stride.d, cspan_rank(self))

#define cspan_swap_axes(self, ax1, ax2) \
    _cspan_swap_axes((self)->shape, (self)->stride.d, ax1, ax2, cspan_rank(self))

// General slicing function.
//
#define c_END (cextent_t)(((size_t)1 << (sizeof(cextent_t)*8 - 1)) - 1)
#define c_ALL 0,c_END

#define cspan_slice(OutSpan, self, ...) \
    OutSpan##_slice_((self)->data, (self)->shape, (self)->stride.d, \
                     ((const intptr_t[][3]){__VA_ARGS__}), cspan_rank(self) + \
                     c_static_assert(cspan_rank(self) == sizeof((intptr_t[][3]){__VA_ARGS__})/sizeof(intptr_t[3])))

// submd#(): # <= 4 optimized. Reduce rank, like e.g. cspan_slice(Span2, &ms3, {x}, {c_ALL}, {c_ALL});
//
#define cspan_submd2(self, x) \
    {.data=cspan_at(self, x, 0), \
     .shape={(self)->shape[1]}, \
     .stride=(cspan_tuple1){.d={(self)->stride.d[1]}}}

#define cspan_submd3(...) c_MACRO_OVERLOAD(cspan_submd3, __VA_ARGS__)
#define cspan_submd3_2(self, x) \
    {.data=cspan_at(self, x, 0, 0), \
     .shape={(self)->shape[1], (self)->shape[2]}, \
     .stride=(cspan_tuple2){.d={(self)->stride.d[1], (self)->stride.d[2]}}}
#define cspan_submd3_3(self, x, y) \
    {.data=cspan_at(self, x, y, 0), \
     .shape={(self)->shape[2]}, \
     .stride=(cspan_tuple1){.d={(self)->stride.d[2]}}}

#define cspan_submd4(...) c_MACRO_OVERLOAD(cspan_submd4, __VA_ARGS__)
#define cspan_submd4_2(self, x) \
    {.data=cspan_at(self, x, 0, 0, 0), \
     .shape={(self)->shape[1], (self)->shape[2], (self)->shape[3]}, \
     .stride=(cspan_tuple3){.d={(self)->stride.d[1], (self)->stride.d[2], (self)->stride.d[3]}}}
#define cspan_submd4_3(self, x, y) \
    {.data=cspan_at(self, x, y, 0, 0), \
     .shape={(self)->shape[2], (self)->shape[3]}, \
     .stride=(cspan_tuple2){.d={(self)->stride.d[2], (self)->stride.d[3]}}}
#define cspan_submd4_4(self, x, y, z) \
    {.data=cspan_at(self, x, y, z, 0), \
     .shape={(self)->shape[3]}, \
     .stride=(cspan_tuple1){.d={(self)->stride.d[3]}}}

#define cspan_print(...) c_MACRO_OVERLOAD(cspan_print, __VA_ARGS__)
#define cspan_print_2(Span, span) /* c11 */ \
    cspan_print_3(Span, span, _Generic(*(span).data, \
        float:"%g", double:"%g", \
        uint8_t:"%d", int8_t:"%d", int16_t:"%d", \
        int32_t:"%" PRIi32, int64_t:"%" PRIi64))
#define cspan_print_3(Span, span, fmt) \
    cspan_print_4(Span, span, fmt, stdout)
#define cspan_print_4(Span, span, fmt, fp) \
    cspan_print_5(Span, span, fmt, fp, "[]")
#define cspan_print_5(Span, span, fmt, fp, brackets) \
    cspan_print_6(Span, span, fmt, fp, brackets, c_EXPAND)
#define cspan_print_complex(Span, span, prec) \
    cspan_print_6(Span, span, "%." #prec "f%+." #prec "fi", stdout, "[]", cspan_CMPLX_FLD)
#define cspan_CMPLX_FLD(x) creal(x), cimag(x)

#define cspan_print_6(Span, span, fmt, fp, brackets, field) do { \
    const Span _s = span; \
    const char *_f = fmt, *_b = brackets; \
    FILE* _fp = fp; \
    int _w, _max = 0; \
    char _res[2][16], _fld[128]; \
    c_foreach_3 (_it, Span, _s) { \
        _w = snprintf(NULL, 0ULL, _f, field(_it.ref[0])); \
        if (_w > _max) _max = _w; \
    } \
    c_foreach_3 (_it, Span, _s) { \
        _cspan_print_assist(_it.pos, _s.shape, cspan_rank(&_s), _res, _b); \
        _w = _max + (_it.pos[cspan_rank(&_s) - 1] > 0); \
        sprintf(_fld, _f, field(_it.ref[0])); \
        fprintf(_fp, "%s%*s%s", _res[0], _w, _fld, _res[1]); \
    } \
} while (0)

/* ------------------- PRIVAT DEFINITIONS ------------------- */

STC_INLINE intptr_t _cspan_size(const cextent_t shape[], int rank) {
    intptr_t size = shape[0];
    while (--rank) size *= shape[rank];
    return size;
}

STC_INLINE void _cspan_swap_axes(cextent_t shape[], cstride_t stride[], int i, int j, int rank) {
    (void)rank;
    c_assert(c_uless(i, rank) & c_uless(j, rank));
    c_swap(cextent_t, shape + i, shape + j);
    c_swap(cstride_t, stride + i, stride + j);
}

STC_INLINE void _cspan_transpose(cextent_t shape[], cstride_t stride[], int rank) {
    for (int i = 0; i < --rank; ++i) {
        c_swap(cextent_t, shape + i, shape + rank);
        c_swap(cstride_t, stride + i, stride + rank);
    }
}

STC_INLINE intptr_t _cspan_index(const cextent_t shape[], const cstride_t stride[],
                                 const intptr_t args[], int rank) {
    intptr_t off = 0;
    (void)shape;
    while (rank--) {
        c_assert(c_uless(args[rank], shape[rank]));
        off += args[rank]*stride[rank];
    }
    return off;
}

STC_API void _cspan_print_assist(cextent_t pos[], const cextent_t shape[], const int rank,
                                 char result[2][16], const char* brackets);

STC_API intptr_t _cspan_next2(cextent_t pos[], const cextent_t shape[], const cstride_t stride[],
                              int rank, int* done);
#define _cspan_next1(pos, shape, stride, rank, done) (*done = ++pos[0]==shape[0], stride[0])
#define _cspan_next3 _cspan_next2
#define _cspan_next4 _cspan_next2
#define _cspan_next5 _cspan_next2
#define _cspan_next6 _cspan_next2
#define _cspan_next7 _cspan_next2
#define _cspan_next8 _cspan_next2

STC_API intptr_t _cspan_slice(cextent_t oshape[], cstride_t ostride[], int* orank,
                              const cextent_t shape[], const cstride_t stride[],
                              const intptr_t args[][3], int rank);

STC_API cstride_t* _cspan_shape2stride(cspan_layout layout, cstride_t shape[], int rank);
#endif // STC_CSPAN_H_INCLUDED

/* --------------------- IMPLEMENTATION --------------------- */
#if defined(i_implement) || defined(i_static)

STC_DEF void _cspan_print_assist(cextent_t pos[], const cextent_t shape[], const int rank,
                                 char result[2][16], const char* brackets) {
    int n = 0, j = 0, r = rank - 1;
    memset(result, 0, 32);

    while (n <= r && pos[r - n] == 0) ++n;
    if (n) for (; j < rank; ++j)
        result[0][j] = j < rank - n ? ' ' : brackets[0];
    for (j = 0; r >= 0 && pos[r] + 1 == shape[r]; --r, ++j)
        result[1][j] = brackets[1];

    n = (j > 0) + ((j > 1) & (j < rank)); // newlines
    if (brackets[2] && j < rank) result[1][j++] = brackets[2]; // comma
    while (n--) result[1][j++] = '\n';
}

STC_DEF intptr_t _cspan_next2(cextent_t pos[], const cextent_t shape[], const cstride_t stride[],
                              int r, int* done) {
    intptr_t off = stride[--r];
    ++pos[r];

    for (; r && pos[r] == shape[r]; --r) {
        pos[r] = 0; ++pos[r - 1];
        off += stride[r - 1] - (intptr_t)shape[r]*stride[r];
    }
    *done = pos[r] == shape[r];
    return off;
}

STC_DEF cstride_t* _cspan_shape2stride(cspan_layout layout, cstride_t shpstri[], int rank) {
    int i, inc;
    if (layout == c_COLMAJOR) i = 0, inc = 1;
    else i = rank - 1, inc = -1;
    cstride_t k = 1, s1 = shpstri[i], s2;

    shpstri[i] = 1;
    while (--rank) {
        i += inc;
        s2 = shpstri[i];
        shpstri[i] = (k *= s1);
        s1 = s2;
    }
    return shpstri;
}

STC_DEF intptr_t _cspan_slice(cextent_t oshape[], cstride_t ostride[], int* orank,
                              const cextent_t shape[], const cstride_t stride[],
                              const intptr_t args[][3], int rank) {
    intptr_t end, off = 0;
    int i = 0, oi = 0;

    for (; i < rank; ++i) {
        off += args[i][0]*stride[i];
        switch (args[i][1]) {
            case 0: c_assert(c_uless(args[i][0], shape[i])); continue;
            case c_END: end = shape[i]; break;
            default: end = args[i][1];
        }
        oshape[oi] = (cextent_t)(end - args[i][0]);
        ostride[oi] = stride[i];
        c_assert((oshape[oi] > 0) & !c_uless(shape[i], end));
        if (args[i][2] > 0) {
            ostride[oi] *= (cextent_t)args[i][2];
            oshape[oi] = (oshape[oi] - 1)/(cextent_t)args[i][2] + 1;
        }
        ++oi;
    }
    *orank = oi;
    return off;
}

#include "priv/linkage2.h"
#endif
