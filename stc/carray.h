/* MIT License
 *
 * Copyright (c) 2021 Tyge Løvset, NORCE, www.norceresearch.no
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
#ifndef CARRAY_H_INCLUDED
#define CARRAY_H_INCLUDED

#include "ccommon.h"
#include <stdlib.h>
/*
// carray2 and carray3 - 2D and 3D dynamic arrays in one memory block with easy indexing.
#include <stc/carray.h>
#include <stdio.h>
using_carray2(i, int);

int main() {
    int w = 7, h = 5;
    carray2i image = carray2i_init(w, h, 0);

    int *dat = carray2i_data(&image);
    for (int i = 0; i < carray2i_size(image); ++i)
        dat[i] = i;

    for (int x = 0; x < image.xdim; ++x)
        for (int y = 0; y < image.ydim; ++y)
            printf(" %d", image.at[x][y]);
    puts("\n");

    c_foreach (i, carray2i, image)
        printf(" %d", *i.ref);
    puts("");

    carray2i_del(&image);
}
*/

// carray2:

#define using_carray2(...) c_MACRO_OVERLOAD(using_carray2, __VA_ARGS__)

#define using_carray2_2(X, Value) \
    using_carray2_4(X, Value, c_trivial_del, c_trivial_fromraw)

#define using_carray2_4(X, Value, valueDel, valueClone) \
\
    typedef Value carray2##X##_value_t; \
    typedef struct { carray2##X##_value_t **at; size_t xdim, ydim; } carray2##X; \
    typedef struct { carray2##X##_value_t *ref; } carray2##X##_iter_t; \
\
    STC_API carray2##X carray2##X##_from(carray2##X##_value_t* block, size_t xdim, size_t ydim); \
    STC_API carray2##X carray2##X##_init(size_t xdim, size_t ydim, Value value); \
    STC_API carray2##X carray2##X##_clone(carray2##X src); \
\
    STC_INLINE size_t carray2##X##_size(carray2##X arr) { return arr.xdim*arr.ydim; } \
    STC_INLINE carray2##X##_value_t *carray2##X##_data(carray2##X* self) { return *self->at; } \
    STC_INLINE carray2##X##_value_t *carray2##X##_at(carray2##X* self, size_t x, size_t y) { \
        return *self->at + self->ydim*x + y; \
    } \
    STC_INLINE carray2##X##_value_t *carray2##X##_release(carray2##X* self) { \
        carray2##X##_value_t *t = *self->at; c_free(self->at); self->at = NULL; return t; \
    } \
\
    STC_INLINE carray2##X##_iter_t carray2##X##_begin(const carray2##X* self) { \
        carray2##X##_iter_t it = {*self->at}; return it; \
    } \
    STC_INLINE carray2##X##_iter_t carray2##X##_end(const carray2##X* self) { \
        carray2##X##_iter_t it = {*self->at + carray2##X##_size(*self)}; return it; \
    } \
    STC_INLINE void carray2##X##_next(carray2##X##_iter_t* it) { ++it->ref; } \
\
    _c_implement_carray2_4(X, Value, valueDel, valueClone) \
    STC_API void carray2##X##_del(carray2##X* self)

// carray3:

#define using_carray3(...) c_MACRO_OVERLOAD(using_carray3, __VA_ARGS__)

#define using_carray3_2(X, Value) \
    using_carray3_4(X, Value, c_trivial_del, c_trivial_fromraw)

#define using_carray3_4(X, Value, valueDel, valueClone) \
\
    typedef Value carray3##X##_value_t; \
    typedef struct { carray3##X##_value_t ***at; size_t xdim, ydim, zdim; } carray3##X; \
    typedef struct { carray3##X##_value_t *ref; } carray3##X##_iter_t; \
\
    STC_API carray3##X carray3##X##_from(carray3##X##_value_t* block, size_t xdim, size_t ydim, size_t zdim); \
    STC_API carray3##X carray3##X##_init(size_t xdim, size_t ydim, size_t zdim, Value value); \
    STC_API carray3##X carray3##X##_clone(carray3##X src); \
\
    STC_INLINE size_t carray3##X##_size(carray3##X arr) { return arr.xdim*arr.ydim*arr.zdim; } \
    STC_INLINE carray3##X##_value_t *carray3##X##_data(carray3##X* self) { return **self->at; } \
    STC_INLINE carray3##X##_value_t *carray3##X##_at(carray3##X* self, size_t x, size_t y, size_t z) { \
        return **self->at + self->zdim*(self->ydim*x + y) + z; \
    } \
    STC_INLINE carray3##X##_value_t *carray3##X##_release(carray3##X* self) { \
        carray3##X##_value_t *t = **self->at; c_free(self->at); self->at = NULL; return t; \
    } \
\
    STC_INLINE carray3##X##_iter_t carray3##X##_begin(const carray3##X* self) { \
        carray3##X##_iter_t it = {**self->at}; return it; \
    } \
    STC_INLINE carray3##X##_iter_t carray3##X##_end(const carray3##X* self) { \
        carray3##X##_iter_t it = {**self->at + carray3##X##_size(*self)}; return it; \
    } \
    STC_INLINE void carray3##X##_next(carray3##X##_iter_t* it) { ++it->ref; } \
\
    _c_implement_carray3_4(X, Value, valueDel, valueClone) \
    STC_API void carray3##X##_del(carray3##X* self)

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)

#define _c_implement_carray2_4(X, Value, valueDel, valueClone) \
\
    STC_DEF carray2##X carray2##X##_from(carray2##X##_value_t* block, size_t xdim, size_t ydim) { \
        size_t n = xdim * ydim; \
        carray2##X _arr = {c_new_2(carray2##X##_value_t*, xdim), xdim, ydim}; \
        for (size_t x = 0; x < xdim; ++x, block += ydim) \
            _arr.at[x] = block; \
        return _arr; \
    } \
\
    STC_DEF carray2##X carray2##X##_init(size_t xdim, size_t ydim, Value value) { \
        size_t n = xdim*ydim; \
        carray2##X _arr = carray2##X##_from(c_new_2(carray2##X##_value_t, n), xdim, ydim); \
        for (carray2##X##_value_t* p = _arr.at[0], *e = p + n; p != e; ++p) \
            *p = value; \
        return _arr; \
    } \
\
    STC_DEF carray2##X carray2##X##_clone(carray2##X src) { \
        size_t n = src.xdim*src.ydim; \
        carray2##X _arr = carray2##X##_from(c_new_2(carray2##X##_value_t, n), src.xdim, src.ydim); \
        for (carray2##X##_value_t* p = _arr.at[0], *q = src.at[0], *e = p + n; p != e; ++p, ++q) \
            *p = valueClone(*q); \
        return _arr; \
    } \
\
    STC_DEF void carray2##X##_del(carray2##X* self) { \
        if (!self->at) return; \
        for (carray2##X##_value_t* p = self->at[0], *e = p + self->xdim*self->ydim; p != e; ++p) \
            valueDel(p); \
        c_free(self->at[0]); /* data */ \
        c_free(self->at); \
    }

#define _c_implement_carray3_4(X, Value, valueDel, valueClone) \
\
    STC_DEF carray3##X carray3##X##_from(carray3##X##_value_t* block, size_t xdim, size_t ydim, size_t zdim) { \
        carray3##X _arr = {c_new_2(carray3##X##_value_t**, xdim*(ydim + 1)), xdim, ydim, zdim}; \
        carray3##X##_value_t** p = (carray3##X##_value_t**) &_arr.at[xdim]; \
        for (size_t x = 0, y; x < xdim; ++x, p += ydim) \
            for (_arr.at[x] = p, y = 0; y < ydim; ++y, block += zdim) \
                _arr.at[x][y] = block; \
        return _arr; \
    } \
\
    STC_DEF carray3##X carray3##X##_init(size_t xdim, size_t ydim, size_t zdim, Value value) { \
        size_t n = xdim*ydim*zdim; \
        carray3##X _arr = carray3##X##_from(c_new_2(carray3##X##_value_t, n), xdim, ydim, zdim); \
        for (carray3##X##_value_t* p = **_arr.at, *e = p + n; p != e; ++p) \
            *p = value; \
        return _arr; \
    } \
\
    STC_DEF carray3##X carray3##X##_clone(carray3##X src) { \
        size_t n = carray3##X##_size(src); \
        carray3##X _arr = carray3##X##_from(c_new_2(carray3##X##_value_t, n), src.xdim, src.ydim, src.zdim); \
        for (carray3##X##_value_t* p = **_arr.at, *q = **src.at, *e = p + n; p != e; ++p, ++q) \
            *p = valueClone(*q); \
        return _arr; \
    } \
\
    STC_DEF void carray3##X##_del(carray3##X* self) { \
        if (!self->at) return; \
        for (carray3##X##_value_t* p = **self->at, *e = p + carray3##X##_size(*self); p != e; ++p) \
            valueDel(p); \
        c_free(self->at[0][0]); /* data */ \
        c_free(self->at); /* pointers */ \
    }

#else
#define _c_implement_carray2_4(X, Value, valueDel, valueClone)
#define _c_implement_carray3_4(X, Value, valueDel, valueClone)
#endif

#endif
