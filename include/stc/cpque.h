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

#ifndef CPQUE_H_INCLUDED
#include <stdlib.h>
#include "forward.h"
#endif

#ifndef _i_prefix
#define _i_prefix cpque_
#endif

#include "template.h"

#if !c_option(c_is_fwd)
   _cx_deftypes(_c_cpque_types, _cx_self, i_val);
#endif
typedef i_valraw _cx_raw;

STC_API void _cx_memb(_make_heap)(_cx_self* self);
STC_API void _cx_memb(_erase_at)(_cx_self* self, size_t idx);
STC_API void _cx_memb(_push)(_cx_self* self, _cx_value value);

STC_INLINE _cx_self _cx_memb(_init)(void)
    { return c_make(_cx_self){NULL}; }

STC_INLINE bool _cx_memb(_reserve)(_cx_self* self, const size_t cap) {
    if (cap != self->size && cap <= self->capacity) return true;
    _cx_value *d = (_cx_value *)c_realloc(self->data, cap*sizeof *d);
    return d ? (self->data = d, self->capacity = cap, true) : false;
}

STC_INLINE void _cx_memb(_shrink_to_fit)(_cx_self* self)
    { _cx_memb(_reserve)(self, self->size); }

STC_INLINE _cx_self _cx_memb(_with_capacity)(const size_t cap) {
    _cx_self out = {NULL}; _cx_memb(_reserve)(&out, cap);
    return out;
}

STC_INLINE _cx_self _cx_memb(_with_size)(const size_t size, i_val null) {
    _cx_self out = {NULL}; _cx_memb(_reserve)(&out, size);
    while (out.size < size) out.data[out.size++] = null;
    return out;
}

STC_INLINE void _cx_memb(_clear)(_cx_self* self) {
    size_t i = self->size; self->size = 0;
    while (i--) { i_valdrop((self->data + i)); }
}

STC_INLINE void _cx_memb(_drop)(_cx_self* self)
    { _cx_memb(_clear)(self); c_free(self->data); }

STC_INLINE size_t _cx_memb(_size)(_cx_self q)
    { return q.size; }

STC_INLINE bool _cx_memb(_empty)(_cx_self q)
    { return !q.size; }

STC_INLINE size_t _cx_memb(_capacity)(_cx_self q)
    { return q.capacity; }

STC_INLINE _cx_value* _cx_memb(_top)(const _cx_self* self)
    { return &self->data[0]; }

STC_INLINE void _cx_memb(_pop)(_cx_self* self)
    { _cx_memb(_erase_at)(self, 0); }

#if !defined _i_no_clone
STC_API _cx_self _cx_memb(_clone)(_cx_self q);

STC_INLINE void _cx_memb(_copy)(_cx_self *self, _cx_self other) {
    if (self->data == other.data) return;
    _cx_memb(_drop)(self); *self = _cx_memb(_clone)(other);
}
STC_INLINE i_val _cx_memb(_value_clone)(_cx_value val)
    { return i_valclone(val); }

#if !defined _i_no_emplace
STC_INLINE void _cx_memb(_emplace)(_cx_self* self, _cx_raw raw)
    { _cx_memb(_push)(self, i_valfrom(raw)); }
#endif // !_i_no_emplace
#endif // !_i_no_clone

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined(_i_implement)

STC_DEF void
_cx_memb(_sift_down_)(_cx_value* arr, const size_t idx, const size_t n) {
    for (size_t r = idx, c = idx << 1; c <= n; c <<= 1) {
        c += (c < n && (i_cmp((&arr[c]), (&arr[c + 1]))) < 0);
        if ((i_cmp((&arr[r]), (&arr[c]))) >= 0) return;
        _cx_value t = arr[r]; arr[r] = arr[c]; arr[r = c] = t;
    }
}

STC_DEF void
_cx_memb(_make_heap)(_cx_self* self) {
    size_t n = _cx_memb(_size)(*self);
    _cx_value *arr = self->data - 1;
    for (size_t k = n >> 1; k != 0; --k)
        _cx_memb(_sift_down_)(arr, k, n);
}

#if !defined _i_no_clone
STC_DEF _cx_self _cx_memb(_clone)(_cx_self q) {
    _cx_self out = _cx_memb(_with_capacity)(q.size);
    for (; out.size < out.capacity; ++q.data)
        out.data[out.size++] = i_valclone((*q.data));
    return out;
}
#endif

STC_DEF void
_cx_memb(_erase_at)(_cx_self* self, const size_t idx) {
    i_valdrop((self->data + idx));
    const size_t n = --self->size;
    self->data[idx] = self->data[n];
    _cx_memb(_sift_down_)(self->data - 1, idx + 1, n);
}

STC_DEF void
_cx_memb(_push)(_cx_self* self, _cx_value value) {
    if (self->size == self->capacity)
        _cx_memb(_reserve)(self, self->size*3/2 + 4);
    _cx_value *arr = self->data - 1; /* base 1 */
    size_t c = ++self->size;
    for (; c > 1 && (i_cmp((&arr[c >> 1]), (&value))) < 0; c >>= 1)
        arr[c] = arr[c >> 1];
    arr[c] = value;
}

#endif
#define CPQUE_H_INCLUDED
#include "template.h"
