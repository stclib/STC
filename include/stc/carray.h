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
    c_forvar (carray2i image = carray2i_init(w, h), carray2i_del(&image)) 
    {
        int *dat = carray2i_data(&image);
        for (int i = 0; i < carray2i_size(image); ++i)
            dat[i] = i;

        for (int x = 0; x < image.xdim; ++x)
            for (int y = 0; y < image.ydim; ++y)
                printf(" %d", image.data[x][y]);
        puts("\n");

        c_foreach (i, carray2i, image)
            printf(" %d", *i.ref);
        puts("");
    }
}
*/
    defTypes( _c_carray2_types(Self, i_VAL); )
\
    STC_API Self cx_memb(_with_values)(size_t xdim, size_t ydim, i_VAL value); \
    STC_API Self cx_memb(_with_storage)(size_t xdim, size_t ydim, cx_value_t* storage); \
    STC_API Self cx_memb(_clone)(Self src); \
\
    STC_INLINE Self cx_memb(_init)(size_t xdim, size_t ydim) { \
        return cx_memb(_with_storage)(xdim, ydim, c_new_n(cx_value_t, xdim*ydim)); \
    } \
    STC_INLINE size_t cx_memb(_size)(Self arr) { return arr.xdim*arr.ydim; } \
    STC_INLINE cx_value_t *cx_memb(_data)(Self* self) { return *self->data; } \
    STC_INLINE cx_value_t *cx_memb(_at)(Self* self, size_t x, size_t y) { \
        return *self->data + self->ydim*x + y; \
    } \
    STC_INLINE cx_value_t *cx_memb(_release)(Self* self) { \
        cx_value_t *t = *self->data; c_free(self->data); self->data = NULL; return t; \
    } \
\
    STC_INLINE cx_iter_t cx_memb(_begin)(const Self* self) { \
        return c_make(cx_iter_t){*self->data}; \
    } \
    STC_INLINE cx_iter_t cx_memb(_end)(const Self* self) { \
        return c_make(cx_iter_t){*self->data + self->xdim*self->ydim}; \
    } \
    STC_INLINE void cx_memb(_next)(cx_iter_t* it) { ++it->ref; } \
\
    _c_implement_carray2(Self, i_VAL, i_VALDEL, i_VALFROM) \
    STC_API void cx_memb(_del)(Self* self)

// carray3:


    defTypes( _c_carray3_types(Self, i_VAL); )
\
    STC_API Self cx_memb(_with_values)(size_t xdim, size_t ydim, size_t zdim, i_VAL value); \
    STC_API Self cx_memb(_with_storage)(size_t xdim, size_t ydim, size_t zdim, cx_value_t* storage); \
    STC_API Self cx_memb(_clone)(Self src); \
\
    STC_INLINE Self cx_memb(_init)(size_t xdim, size_t ydim, size_t zdim) { \
        return cx_memb(_with_storage)(xdim, ydim, zdim, c_new_n(cx_value_t, xdim*ydim*zdim)); \
    } \
    STC_INLINE size_t cx_memb(_size)(Self arr) { return arr.xdim*arr.ydim*arr.zdim; } \
    STC_INLINE cx_value_t* cx_memb(_data)(Self* self) { return **self->data; } \
    STC_INLINE cx_value_t* cx_memb(_at)(Self* self, size_t x, size_t y, size_t z) { \
        return **self->data + self->zdim*(self->ydim*x + y) + z; \
    } \
\
    STC_INLINE cx_value_t* cx_memb(_release)(Self* self) { \
        cx_value_t *values = **self->data; \
        c_free(self->data); self->data = NULL; \
        return values; \
    } \
\
    STC_INLINE cx_iter_t cx_memb(_begin)(const Self* self) { \
        return c_make(cx_iter_t){**self->data}; \
    } \
    STC_INLINE cx_iter_t cx_memb(_end)(const Self* self) { \
        return c_make(cx_iter_t){**self->data + cx_memb(_size)(*self)}; \
    } \
    STC_INLINE void cx_memb(_next)(cx_iter_t* it) { ++it->ref; } \
\
    _c_implement_carray3(Self, i_VAL, i_VALDEL, i_VALFROM) \
    STC_API void cx_memb(_del)(Self* self)

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)

#define _c_implement_carray2(Self, i_VAL, i_VALDEL, i_VALFROM) \
\
    STC_DEF Self cx_memb(_with_storage)(size_t xdim, size_t ydim, cx_value_t* block) { \
        Self _arr = {c_new_n(cx_value_t*, xdim), xdim, ydim}; \
        for (size_t x = 0; x < xdim; ++x, block += ydim) \
            _arr.data[x] = block; \
        return _arr; \
    } \
\
    STC_DEF Self cx_memb(_with_values)(size_t xdim, size_t ydim, i_VAL value) { \
        Self _arr = cx_memb(_init)(xdim, ydim); \
        for (cx_value_t* p = _arr.data[0], *e = p + xdim*ydim; p != e; ++p) \
            *p = value; \
        return _arr; \
    } \
\
    STC_DEF Self cx_memb(_clone)(Self src) { \
        Self _arr = cx_memb(_init)(src.xdim, src.ydim); \
        for (cx_value_t* p = _arr.data[0], *q = src.data[0], *e = p + cx_memb(_size)(src); p != e; ++p, ++q) \
            *p = i_VALFROM(*q); \
        return _arr; \
    } \
\
    STC_DEF void cx_memb(_del)(Self* self) { \
        if (!self->data) return; \
        for (cx_value_t* p = self->data[0], *e = p + cx_memb(_size)(*self); p != e; ++p) \
            i_VALDEL(p); \
        c_free(self->data[0]); /* data */ \
        c_free(self->data);    /* pointers */ \
    }

// carray3 impl.

#define _c_implement_carray3(Self, i_VAL, i_VALDEL, i_VALFROM) \
\
    STC_DEF Self cx_memb(_with_storage)(size_t xdim, size_t ydim, size_t zdim, cx_value_t* block) { \
        Self _arr = {c_new_n(cx_value_t**, xdim*(ydim + 1)), xdim, ydim, zdim}; \
        cx_value_t** p = (cx_value_t**) &_arr.data[xdim]; \
        for (size_t x = 0, y; x < xdim; ++x, p += ydim) \
            for (_arr.data[x] = p, y = 0; y < ydim; ++y, block += zdim) \
                _arr.data[x][y] = block; \
        return _arr; \
    } \
\
    STC_DEF Self cx_memb(_with_values)(size_t xdim, size_t ydim, size_t zdim, i_VAL value) { \
        Self _arr = cx_memb(_init)(xdim, ydim, zdim); \
        for (cx_value_t* p = **_arr.data, *e = p + xdim*ydim*zdim; p != e; ++p) \
            *p = value; \
        return _arr; \
    } \
\
    STC_DEF Self cx_memb(_clone)(Self src) { \
        Self _arr = cx_memb(_init)(src.xdim, src.ydim, src.zdim); \
        for (cx_value_t* p = **_arr.data, *q = **src.data, *e = p + cx_memb(_size)(src); p != e; ++p, ++q) \
            *p = i_VALFROM(*q); \
        return _arr; \
    } \
\
    STC_DEF void cx_memb(_del)(Self* self) { \
        if (!self->data) return; \
        for (cx_value_t* p = **self->data, *e = p + cx_memb(_size)(*self); p != e; ++p) \
            i_VALDEL(p); \
        c_free(self->data[0][0]); /* data */ \
        c_free(self->data);       /* pointers */ \
    }

#endif

#endif
