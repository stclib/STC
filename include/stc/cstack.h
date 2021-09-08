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

#ifndef CSTACK_H_INCLUDED
#define CSTACK_H_INCLUDED
#include <stdlib.h>
#include "ccommon.h"
#include "forward.h"
#endif

#define i_module cstack
#include "template.h"

#if !defined i_fwd
   cx_deftypes(_c_cstack_types, Self, i_val);
#endif
typedef i_valraw cx_rawvalue_t;

STC_INLINE Self cx_memb(_init)(void)
    { return (Self){0, 0, 0}; }

STC_INLINE Self cx_memb(_init_with_capacity)(size_t cap) {
    Self out = {(cx_value_t *) c_malloc(cap*sizeof(cx_value_t)), 0, cap};
    return out;
}

STC_INLINE void cx_memb(_clear)(Self* self) {
    size_t i = self->size; self->size = 0;
    while (i--) i_valdel(&self->data[i]);
}

STC_INLINE void cx_memb(_del)(Self* self)
    { cx_memb(_clear)(self); c_free(self->data); }

STC_INLINE size_t cx_memb(_size)(Self v)
    { return v.size; }

STC_INLINE bool cx_memb(_empty)(Self v)
    { return !v.size; }

STC_INLINE size_t cx_memb(_capacity)(Self v)
    { return v.capacity; }

STC_INLINE cx_value_t* cx_memb(_top)(const Self* self)
    { return &self->data[self->size - 1]; }

STC_INLINE void cx_memb(_pop)(Self* self)
    { --self->size; }

STC_INLINE void cx_memb(_push)(Self* self, cx_value_t value) {
    if (self->size == self->capacity)
        self->data = (cx_value_t *) c_realloc(self->data, (self->capacity = self->size*3/2 + 4)*sizeof value);
    self->data[ self->size++ ] = value;
}

STC_INLINE void cx_memb(_emplace)(Self* self, cx_rawvalue_t raw)
    { cx_memb(_push)(self, i_valfrom(raw)); }

STC_INLINE Self cx_memb(_clone)(Self v) {
    Self out = {(cx_value_t *) c_malloc(v.size*sizeof(cx_value_t)), v.size, v.size};
    for (size_t i = 0; i < v.size; ++i, ++v.data) out.data[i] = i_valfrom(i_valto(v.data));
    return out;
}

STC_INLINE i_val cx_memb(_value_clone)(cx_value_t val)
    { return i_valfrom(i_valto(&val)); }

STC_INLINE cx_iter_t cx_memb(_begin)(const Self* self)
    { return c_make(cx_iter_t){self->data}; }
STC_INLINE cx_iter_t cx_memb(_end)(const Self* self)
    { return c_make(cx_iter_t){self->data + self->size}; }
STC_INLINE void cx_memb(_next)(cx_iter_t* it) { ++it->ref; }

#include "template.h"
