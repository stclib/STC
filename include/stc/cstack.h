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
#endif // CSTACK_H_INCLUDED

#ifndef i_prefix
#define i_prefix cstack_
#endif
#include "template.h"

#if !defined i_fwd
cx_deftypes(_c_cstack_types, Self, i_val);
#endif
typedef i_valraw cx_rawvalue_t;

STC_INLINE Self cx_memb(_init)(void)
    { return c_make(Self){0, 0, 0}; }

STC_INLINE Self cx_memb(_with_capacity)(size_t cap) {
    Self out = {(cx_value_t *) c_malloc(cap*sizeof(i_val)), 0, cap};
    return out;
}

STC_INLINE Self cx_memb(_with_size)(size_t size, i_val fill) {
    Self out = {(cx_value_t *) c_malloc(size*sizeof fill), size, size};
    while (size) out.data[--size] = fill;
    return out;
}

STC_INLINE void cx_memb(_clear)(Self* self) {
    cx_value_t *p = self->data + self->size;
    while (p-- != self->data) i_valdel(p);
    self->size = 0;
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
    { cx_value_t* p = &self->data[--self->size]; i_valdel(p); }

STC_INLINE void cx_memb(_reserve)(Self* self, size_t n) {
    if (n >= self->size)
        self->data = (cx_value_t *)c_realloc(self->data, (self->capacity = n)*sizeof(cx_value_t));
}

STC_INLINE void cx_memb(_shrink_to_fit)(Self* self)
    { cx_memb(_reserve)(self, self->size); }

STC_INLINE cx_value_t* cx_memb(_push)(Self* self, cx_value_t val) {
    if (self->size == self->capacity) cx_memb(_reserve)(self, self->size*3/2 + 4);
    cx_value_t* vp = self->data + self->size++; 
    *vp = val; return vp;
}

STC_INLINE cx_value_t* cx_memb(_emplace)(Self* self, cx_rawvalue_t raw)
    { return cx_memb(_push)(self, i_valfrom(raw)); }

STC_INLINE cx_value_t* cx_memb(_at)(const Self* self, size_t idx)
    { assert(idx < self->size); return self->data + idx; }

STC_INLINE Self cx_memb(_clone)(Self v) {
    Self out = {(cx_value_t *) c_malloc(v.size*sizeof(cx_value_t)), v.size, v.size};
    for (size_t i = 0; i < v.size; ++i, ++v.data) out.data[i] = i_valfrom(i_valto(v.data));
    return out;
}

STC_INLINE void cx_memb(_copy)(Self *self, Self other) {
    if (self->data == other.data) return;
    cx_memb(_del)(self); *self = cx_memb(_clone)(other);
}

STC_INLINE i_val cx_memb(_value_clone)(cx_value_t val)
    { return i_valfrom(i_valto(&val)); }
STC_INLINE i_valraw cx_memb(_value_toraw)(cx_value_t* val)
    { return i_valto(val); }

STC_INLINE cx_iter_t cx_memb(_begin)(const Self* self)
    { return c_make(cx_iter_t){self->data}; }
STC_INLINE cx_iter_t cx_memb(_end)(const Self* self)
    { return c_make(cx_iter_t){self->data + self->size}; }
STC_INLINE void cx_memb(_next)(cx_iter_t* it) { ++it->ref; }
STC_INLINE cx_iter_t cx_memb(_advance)(cx_iter_t it, intptr_t offs)
    { it.ref += offs; return it; }

#include "template.h"
