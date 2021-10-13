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

#ifndef CPQUE_H_INCLUDED
#include <stdlib.h>
#include "ccommon.h"
#include "forward.h"
#endif

#ifndef i_prefix
#define i_prefix cpque_
#endif

#include "template.h"

#if !defined i_fwd
   cx_deftypes(_c_cpque_types, Self, i_val);
#endif
typedef i_valraw cx_rawvalue_t;

STC_API void cx_memb(_make_heap)(Self* self);
STC_API void cx_memb(_erase_at)(Self* self, size_t idx);
STC_API void cx_memb(_push)(Self* self, cx_value_t value);
STC_API Self cx_memb(_clone)(Self q);

STC_INLINE Self cx_memb(_init)(void)
    { return c_make(Self){0, 0, 0}; }

STC_INLINE Self cx_memb(_with_capacity)(size_t cap) {
    Self out = {(cx_value_t *) c_malloc(cap*sizeof(cx_value_t)), 0, cap};
    return out;
}

STC_INLINE void cx_memb(_reserve)(Self* self, size_t n) {
    if (n >= self->size)
        self->data = (cx_value_t *)c_realloc(self->data, (self->capacity = n)*sizeof(cx_rawvalue_t));
}

STC_INLINE void cx_memb(_clear)(Self* self) {
    size_t i = self->size; self->size = 0;
    while (i--) i_valdel(&self->data[i]);
}

STC_INLINE void cx_memb(_del)(Self* self)
    { cx_memb(_clear)(self); c_free(self->data); }

STC_INLINE void cx_memb(_copy)(Self *self, Self other) {
    if (self->data == other.data) return;
    cx_memb(_del)(self); *self = cx_memb(_clone)(other);
}

STC_INLINE size_t cx_memb(_size)(Self q)
    { return q.size; }

STC_INLINE bool cx_memb(_empty)(Self q)
    { return !q.size; }

STC_INLINE size_t cx_memb(_capacity)(Self q)
    { return q.capacity; }

STC_INLINE cx_value_t* cx_memb(_top)(const Self* self)
    { return &self->data[0]; }

STC_INLINE void cx_memb(_pop)(Self* self)
    { cx_memb(_erase_at)(self, 0); }

STC_INLINE void cx_memb(_emplace)(Self* self, cx_rawvalue_t raw)
    { cx_memb(_push)(self, i_valfrom(raw)); }

STC_INLINE i_val cx_memb(_value_clone)(cx_value_t val)
    { return i_valfrom(i_valto(&val)); }

STC_INLINE void
cx_memb(_push_back)(Self* self, cx_value_t value) {
    if (self->size == self->capacity) cx_memb(_reserve)(self, self->size*3/2 + 4);
    self->data[ self->size++ ] = value;
}

STC_INLINE void
cx_memb(_pop_back)(Self* self)
    { cx_value_t* p = &self->data[--self->size]; i_valdel(p); }

/* -------------------------- IMPLEMENTATION ------------------------- */
#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION) || defined(i_imp)

STC_DEF void
cx_memb(_sift_down_)(cx_value_t* arr, size_t idx, size_t n) {
    for (size_t r = idx, c = idx << 1; c <= n; c <<= 1) {
        c += (c < n && i_cmp(&arr[c], &arr[c + 1]) < 0);
        if (i_cmp(&arr[r], &arr[c]) >= 0) return;
        cx_value_t t = arr[r]; arr[r] = arr[c]; arr[r = c] = t;
    }
}

STC_DEF void
cx_memb(_make_heap)(Self* self) {
    size_t n = cx_memb(_size)(*self);
    cx_value_t *arr = self->data - 1;
    for (size_t k = n >> 1; k != 0; --k)
        cx_memb(_sift_down_)(arr, k, n);
}

STC_DEF Self cx_memb(_clone)(Self q) {
    Self out = {(cx_value_t *) c_malloc(q.size*sizeof(cx_value_t)), q.size, q.size};
    for (size_t i = 0; i < q.size; ++i, ++q.data) out.data[i] = i_valfrom(i_valto(q.data));
    return out;
}

STC_DEF void
cx_memb(_erase_at)(Self* self, size_t idx) {
    size_t n = cx_memb(_size)(*self) - 1;
    self->data[idx] = self->data[n];
    cx_memb(_pop_back)(self);
    cx_memb(_sift_down_)(self->data - 1, idx + 1, n);
}

STC_DEF void
cx_memb(_push)(Self* self, cx_value_t value) {
    if (self->size == self->capacity)
        cx_memb(_reserve)(self, self->size*3/2 + 4);
    cx_value_t *arr = self->data - 1; /* base 1 */
    size_t c = ++self->size;
    for (; c > 1 && i_cmp(&arr[c >> 1], &value) < 0; c >>= 1)
        arr[c] = arr[c >> 1];
    arr[c] = value;
}

#endif
#include "template.h"
#define CPQUE_H_INCLUDED