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
#ifndef CARR2_H_INCLUDED
#define CARR2_H_INCLUDED
#include "ccommon.h"
#include "forward.h"
#include <stdlib.h>
#endif
/*
// carr2- 2D dynamic array in one memory block with easy indexing.
#define i_val int
#include <stc/carr2.h>
#include <stdio.h>

int main() {
    int w = 7, h = 5;
    c_autovar (carr2_int image = carr2_int_init(w, h), carr2_int_del(&image))
    {
        int *dat = carr2_int_data(&image);
        for (int i = 0; i < carr2_int_size(image); ++i)
            dat[i] = i;

        for (int x = 0; x < image.xdim; ++x)
            for (int y = 0; y < image.ydim; ++y)
                printf(" %d", image.data[x][y]);
        puts("\n");

        c_foreach (i, carr2_int, image)
            printf(" %d", *i.ref);
        puts("");
    }
}
*/

#ifndef i_prefix
#define i_prefix carr2_
#endif
#include "template.h"

#ifndef i_fwd
cx_deftypes(_c_carr2_types, Self, i_val);
#endif

STC_API Self cx_memb(_with_values)(size_t xdim, size_t ydim, i_val value);
STC_API Self cx_memb(_with_storage)(size_t xdim, size_t ydim, cx_value_t* storage);
STC_API Self cx_memb(_clone)(Self src);
STC_API cx_value_t* cx_memb(_release)(Self* self);
STC_API void cx_memb(_del)(Self* self);

STC_INLINE Self cx_memb(_init)(size_t xdim, size_t ydim) {
    return cx_memb(_with_storage)(xdim, ydim, c_new_n(cx_value_t, xdim*ydim));
}
STC_INLINE size_t cx_memb(_size)(Self arr)
    { return arr.xdim*arr.ydim; }

STC_INLINE cx_value_t *cx_memb(_data)(Self* self)
    { return *self->data; }

STC_INLINE cx_value_t *cx_memb(_at)(Self* self, size_t x, size_t y)
    { return *self->data + self->ydim*x + y; }

STC_INLINE void cx_memb(_copy)(Self *self, Self other) {
    if (self->data == other.data) return;
    cx_memb(_del)(self); *self = cx_memb(_clone)(other);
}

STC_INLINE cx_iter_t cx_memb(_begin)(const Self* self)
    { return c_make(cx_iter_t){*self->data}; }

STC_INLINE cx_iter_t cx_memb(_end)(const Self* self)
    { return c_make(cx_iter_t){*self->data + self->xdim*self->ydim}; }

STC_INLINE void cx_memb(_next)(cx_iter_t* it)
    { ++it->ref; }

/* -------------------------- IMPLEMENTATION ------------------------- */
#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION) || defined(i_imp)

STC_DEF Self cx_memb(_with_storage)(size_t xdim, size_t ydim, cx_value_t* block) {
    Self _arr = {c_new_n(cx_value_t*, xdim), xdim, ydim};
    for (size_t x = 0; x < xdim; ++x, block += ydim)
        _arr.data[x] = block;
    return _arr;
}

STC_DEF Self cx_memb(_with_values)(size_t xdim, size_t ydim, i_val value) {
    Self _arr = cx_memb(_init)(xdim, ydim);
    for (cx_value_t* p = _arr.data[0], *e = p + xdim*ydim; p != e; ++p)
        *p = value;
    return _arr;
}

STC_DEF Self cx_memb(_clone)(Self src) {
    Self _arr = cx_memb(_init)(src.xdim, src.ydim);
    for (cx_value_t* p = _arr.data[0], *q = src.data[0], *e = p + cx_memb(_size)(src); p != e; ++p, ++q)
        *p = i_valfrom(i_valto(q));
    return _arr;
}

STC_DEF cx_value_t *cx_memb(_release)(Self* self) {
    cx_value_t *values = self->data[0];
    c_free(self->data);
    self->data = NULL;
    return values;
}

STC_DEF void cx_memb(_del)(Self* self) {
    if (!self->data) return;
    for (cx_value_t* p = self->data[0], *e = p + cx_memb(_size)(*self); p != e; ++p)
        i_valdel(p);
    c_free(self->data[0]); /* values */
    c_free(self->data);    /* pointers */
}

#endif
#include "template.h"
