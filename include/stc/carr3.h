/* MIT License
 *
 * Copyright (c) 2022 Tyge Løvset, NORCE, www.norceresearch.no
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
#include "ccommon.h"

#ifndef CARR3_H_INCLUDED
#define CARR3_H_INCLUDED
#include "forward.h"
#include <stdlib.h>
#endif
/*
// carr3 - 3D dynamic array in one memory block with easy indexing.
#define i_val int
#include <stc/carr3.h>
#include <stdio.h>

int main() {
    int w = 7, h = 5, d = 3;
    c_autovar (carr3_int image = carr3_int_init(w, h, d), carr3_int_drop(&image))
    {
        int *dat = carr3_int_data(&image);
        for (int i = 0; i < carr3_int_size(image); ++i)
            dat[i] = i;

        for (int x = 0; x < image.xdim; ++x)
            for (int y = 0; y < image.ydim; ++y)
                for (int z = 0; z < image.zdim; ++z)
                    printf(" %d", image.data[x][y][z]);
        puts("\n");

        c_foreach (i, carr3_int, image)
            printf(" %d", *i.ref);
        puts("");
    }
}
*/

#ifndef _i_prefix
#define _i_prefix carr3_
#endif
#include "template.h"

#if !c_option(c_is_fwd)
_cx_deftypes(_c_carr3_types, _cx_self, i_val);
#endif

STC_API _cx_self   _cx_memb(_with_values)(size_t xdim, size_t ydim, size_t zdim, i_val value);
STC_API _cx_self   _cx_memb(_with_storage)(size_t xdim, size_t ydim, size_t zdim, _cx_value* storage);
STC_API _cx_value* _cx_memb(_release)(_cx_self* self);
STC_API void       _cx_memb(_drop)(_cx_self* self);
#if !defined _i_no_clone
STC_API _cx_self   _cx_memb(_clone)(_cx_self src);
STC_API void       _cx_memb(_copy)(_cx_self *self, _cx_self other);
#endif

STC_INLINE _cx_self _cx_memb(_init)(size_t xdim, size_t ydim, size_t zdim) {
    return _cx_memb(_with_storage)(xdim, ydim, zdim, c_alloc_n(_cx_value, xdim*ydim*zdim));
}

STC_INLINE size_t _cx_memb(_size)(_cx_self arr)
    { return arr.xdim*arr.ydim*arr.zdim; }

STC_INLINE _cx_value* _cx_memb(_data)(_cx_self* self)
    { return **self->data; }

STC_INLINE const _cx_value* _cx_memb(_at)(const _cx_self* self, size_t x, size_t y, size_t z) {
    assert(x < self->xdim && y < self->ydim && z < self->zdim);
    return **self->data + self->zdim*(self->ydim*x + y) + z;
}

STC_INLINE size_t _cx_memb(_idx)(const _cx_self* self, size_t x, size_t y, size_t z) {
    return self->zdim*(self->ydim*x + y) + z;
}

STC_INLINE _cx_iter _cx_memb(_begin)(const _cx_self* self)
    { return c_make(_cx_iter){**self->data}; }

STC_INLINE _cx_iter _cx_memb(_end)(const _cx_self* self)
    { return c_make(_cx_iter){**self->data + _cx_memb(_size)(*self)}; }

STC_INLINE void _cx_memb(_next)(_cx_iter* it)
    { ++it->ref; }

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined(_i_implement)

STC_DEF _cx_self _cx_memb(_with_storage)(size_t xdim, size_t ydim, size_t zdim, _cx_value* block) {
    _cx_self _arr = {c_alloc_n(_cx_value**, xdim*(ydim + 1)), xdim, ydim, zdim};
    _cx_value** p = (_cx_value**) &_arr.data[xdim];
    for (size_t x = 0, y; x < xdim; ++x, p += ydim)
        for (y = 0, _arr.data[x] = p; y < ydim; ++y, block += zdim)
            p[y] = block;
    return _arr;
}

STC_DEF _cx_self _cx_memb(_with_values)(size_t xdim, size_t ydim, size_t zdim, i_val value) {
    _cx_self _arr = _cx_memb(_init)(xdim, ydim, zdim);
    for (_cx_value* p = **_arr.data, *e = p + xdim*ydim*zdim; p != e; ++p)
        *p = value;
    return _arr;
}

#if !defined _i_no_clone

STC_DEF _cx_self _cx_memb(_clone)(_cx_self src) {
    _cx_self _arr = _cx_memb(_init)(src.xdim, src.ydim, src.zdim);
    for (_cx_value* p = **_arr.data, *q = **src.data, *e = p + _cx_memb(_size)(src); p != e; ++p, ++q)
        *p = i_valclone((*q));
    return _arr;
}

STC_DEF void _cx_memb(_copy)(_cx_self *self, _cx_self other) {
    if (self->data == other.data) return;
    _cx_memb(_drop)(self); *self = _cx_memb(_clone)(other);
}
#endif

STC_DEF _cx_value* _cx_memb(_release)(_cx_self* self) {
    _cx_value *values = self->data[0][0];
    c_free(self->data);
    self->data = NULL;
    return values;
}

STC_DEF void _cx_memb(_drop)(_cx_self* self) {
    if (!self->data) return;
    for (_cx_value* p = **self->data, *q = p + _cx_memb(_size)(*self);  p != q; ) {
        --q; i_valdrop(q);
    }
    c_free(self->data[0][0]); /* data */
    c_free(self->data);       /* pointers */
}

#endif
#include "template.h"
