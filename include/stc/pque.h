/* MIT License
 *
 * Copyright (c) 2024 Tyge Løvset
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
#include "priv/linkage.h"

#ifndef STC_PQUE_H_INCLUDED
#define STC_PQUE_H_INCLUDED
#include "common.h"
#include "types.h"
#include <stdlib.h>
#endif // STC_PQUE_H_INCLUDED

#ifndef _i_prefix
  #define _i_prefix pque_
#endif
#define _i_ispque
#include "priv/template.h"
#ifndef i_is_forward
  _c_DEFTYPES(_c_pque_types, _i_self, i_key);
#endif
typedef i_keyraw _m_raw;

STC_API void        _c_MEMB(_make_heap)(_i_self* self);
STC_API void        _c_MEMB(_erase_at)(_i_self* self, intptr_t idx);
STC_API _m_value*   _c_MEMB(_push)(_i_self* self, _m_value value);

STC_INLINE _i_self _c_MEMB(_init)(void)
    { return c_literal(_i_self){NULL}; }

STC_INLINE void _c_MEMB(_put_n)(_i_self* self, const _m_raw* raw, intptr_t n)
    { while (n--) _c_MEMB(_push)(self, i_keyfrom(*raw++)); }

STC_INLINE _i_self _c_MEMB(_from_n)(const _m_raw* raw, intptr_t n)
    { _i_self cx = {0}; _c_MEMB(_put_n)(&cx, raw, n); return cx; }

STC_INLINE bool _c_MEMB(_reserve)(_i_self* self, const intptr_t cap) {
    if (cap != self->size && cap <= self->capacity) return true;
    _m_value *d = (_m_value *)i_realloc(self->data, self->capacity*c_sizeof *d, cap*c_sizeof *d);
    return d ? (self->data = d, self->capacity = cap, true) : false;
}

STC_INLINE void _c_MEMB(_shrink_to_fit)(_i_self* self)
    { _c_MEMB(_reserve)(self, self->size); }

STC_INLINE _i_self _c_MEMB(_with_capacity)(const intptr_t cap) {
    _i_self out = {NULL}; _c_MEMB(_reserve)(&out, cap);
    return out;
}

STC_INLINE _i_self _c_MEMB(_with_size)(const intptr_t size, _m_value null) {
    _i_self out = {NULL}; _c_MEMB(_reserve)(&out, size);
    while (out.size < size) out.data[out.size++] = null;
    return out;
}

STC_INLINE void _c_MEMB(_clear)(_i_self* self) {
    intptr_t i = self->size; self->size = 0;
    while (i--) { i_keydrop((self->data + i)); }
}

STC_INLINE void _c_MEMB(_drop)(const _i_self* cself) {
    _i_self* self = (_i_self*)cself;
    _c_MEMB(_clear)(self);
    i_free(self->data, self->capacity*c_sizeof(*self->data));
}

STC_INLINE intptr_t _c_MEMB(_size)(const _i_self* q)
    { return q->size; }

STC_INLINE bool _c_MEMB(_is_empty)(const _i_self* q)
    { return !q->size; }

STC_INLINE intptr_t _c_MEMB(_capacity)(const _i_self* q)
    { return q->capacity; }

STC_INLINE const _m_value* _c_MEMB(_top)(const _i_self* self)
    { return &self->data[0]; }

STC_INLINE void _c_MEMB(_pop)(_i_self* self)
    { c_assert(!_c_MEMB(_is_empty)(self)); _c_MEMB(_erase_at)(self, 0); }

STC_INLINE _m_value _c_MEMB(_pull)(_i_self* self)
    { _m_value v = self->data[0]; _c_MEMB(_erase_at)(self, 0); return v; }

#if !defined i_no_clone
STC_API _i_self _c_MEMB(_clone)(_i_self q);

STC_INLINE void _c_MEMB(_copy)(_i_self *self, const _i_self* other) {
    if (self->data == other->data) return;
    _c_MEMB(_drop)(self);
    *self = _c_MEMB(_clone)(*other);
}
STC_INLINE _m_value _c_MEMB(_value_clone)(_m_value val)
    { return i_keyclone(val); }
#endif // !i_no_clone

#if !defined i_no_emplace
STC_INLINE void _c_MEMB(_emplace)(_i_self* self, _m_raw raw)
    { _c_MEMB(_push)(self, i_keyfrom(raw)); }
#endif // !i_no_emplace

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined(i_implement) || defined(i_static)

STC_DEF void
_c_MEMB(_sift_down_)(_i_self* self, const intptr_t idx, const intptr_t n) {
    _m_value t, *arr = self->data - 1;
    for (intptr_t r = idx, c = idx*2; c <= n; c *= 2) {
        c += i_less((&arr[c]), (&arr[c + (c < n)]));
        if (!(i_less((&arr[r]), (&arr[c])))) return;
        t = arr[r], arr[r] = arr[c], arr[r = c] = t;
    }
}

STC_DEF void
_c_MEMB(_make_heap)(_i_self* self) {
    intptr_t n = self->size;
    for (intptr_t k = n/2; k != 0; --k)
        _c_MEMB(_sift_down_)(self, k, n);
}

#if !defined i_no_clone
STC_DEF _i_self _c_MEMB(_clone)(_i_self q) {
    _i_self tmp = _c_MEMB(_with_capacity)(q.size);
    for (; tmp.size < q.size; ++q.data)
        tmp.data[tmp.size++] = i_keyclone((*q.data));
    q.data = tmp.data;
    q.capacity = tmp.capacity;
    return q;
}
#endif

STC_DEF void
_c_MEMB(_erase_at)(_i_self* self, const intptr_t idx) {
    i_keydrop((self->data + idx));
    const intptr_t n = --self->size;
    self->data[idx] = self->data[n];
    _c_MEMB(_sift_down_)(self, idx + 1, n);
}

STC_DEF _m_value*
_c_MEMB(_push)(_i_self* self, _m_value value) {
    if (self->size == self->capacity)
        _c_MEMB(_reserve)(self, self->size*3/2 + 4);
    _m_value *arr = self->data - 1; /* base 1 */
    intptr_t c = ++self->size;
    for (; c > 1 && (i_less((&arr[c/2]), (&value))); c /= 2)
        arr[c] = arr[c/2];
    arr[c] = value;
    return arr + c;
}

#endif
#undef _i_ispque
#include "priv/linkage2.h"
#include "priv/template2.h"