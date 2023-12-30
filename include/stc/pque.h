/* MIT License
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
#include "priv/linkage.h"

#ifndef STC_PQUE_H_INCLUDED
#define STC_PQUE_H_INCLUDED
#include "common.h"
#include <stdlib.h>
#include "types.h"
#endif // STC_PQUE_H_INCLUDED

#ifndef _i_prefix
  #define _i_prefix pque_
#endif
#define _i_ispque
#include "priv/template.h"
#ifndef i_is_forward
  _c_DEFTYPES(_c_pque_types, i_type, i_key);
#endif
typedef i_keyraw _m_raw;

STC_API void        _c_MEMB(_make_heap)(i_type* self);
STC_API void        _c_MEMB(_erase_at)(i_type* self, intptr_t idx);
STC_API _m_value*   _c_MEMB(_push)(i_type* self, _m_value value);

STC_INLINE i_type _c_MEMB(_init)(void)
    { return c_LITERAL(i_type){NULL}; }

STC_INLINE void _c_MEMB(_put_n)(i_type* self, const _m_raw* raw, intptr_t n)
    { while (n--) _c_MEMB(_push)(self, i_keyfrom(*raw++)); }

STC_INLINE i_type _c_MEMB(_from_n)(const _m_raw* raw, intptr_t n)
    { i_type cx = {0}; _c_MEMB(_put_n)(&cx, raw, n); return cx; }

STC_INLINE bool _c_MEMB(_reserve)(i_type* self, const intptr_t cap) {
    if (cap != self->_len && cap <= self->_cap) return true;
    _m_value *d = (_m_value *)i_realloc(self->data, self->_cap*c_sizeof *d, cap*c_sizeof *d);
    return d ? (self->data = d, self->_cap = cap, true) : false;
}

STC_INLINE void _c_MEMB(_shrink_to_fit)(i_type* self)
    { _c_MEMB(_reserve)(self, self->_len); }

STC_INLINE i_type _c_MEMB(_with_capacity)(const intptr_t cap) {
    i_type out = {NULL}; _c_MEMB(_reserve)(&out, cap);
    return out;
}

STC_INLINE i_type _c_MEMB(_with_size)(const intptr_t size, _m_value null) {
    i_type out = {NULL}; _c_MEMB(_reserve)(&out, size);
    while (out._len < size) out.data[out._len++] = null;
    return out;
}

STC_INLINE void _c_MEMB(_clear)(i_type* self) {
    intptr_t i = self->_len; self->_len = 0;
    while (i--) { i_keydrop((self->data + i)); }
}

STC_INLINE void _c_MEMB(_drop)(const i_type* cself) {
    i_type* self = (i_type*)cself;
    _c_MEMB(_clear)(self);
    i_free(self->data, self->_cap*c_sizeof(*self->data));
}

STC_INLINE intptr_t _c_MEMB(_size)(const i_type* q)
    { return q->_len; }

STC_INLINE bool _c_MEMB(_empty)(const i_type* q)
    { return !q->_len; }

STC_INLINE intptr_t _c_MEMB(_capacity)(const i_type* q)
    { return q->_cap; }

STC_INLINE const _m_value* _c_MEMB(_top)(const i_type* self)
    { return &self->data[0]; }

STC_INLINE void _c_MEMB(_pop)(i_type* self)
    { c_assert(!_c_MEMB(_empty)(self)); _c_MEMB(_erase_at)(self, 0); }

STC_INLINE _m_value _c_MEMB(_pull)(i_type* self)
    { _m_value v = self->data[0]; _c_MEMB(_erase_at)(self, 0); return v; }

#if !defined i_no_clone
STC_API i_type _c_MEMB(_clone)(i_type q);

STC_INLINE void _c_MEMB(_copy)(i_type *self, const i_type* other) {
    if (self->data == other->data) return;
    _c_MEMB(_drop)(self);
    *self = _c_MEMB(_clone)(*other);
}
STC_INLINE _m_value _c_MEMB(_value_clone)(_m_value val)
    { return i_keyclone(val); }
#endif // !i_no_clone

#if !defined i_no_emplace
STC_INLINE void _c_MEMB(_emplace)(i_type* self, _m_raw raw)
    { _c_MEMB(_push)(self, i_keyfrom(raw)); }
#endif // !i_no_emplace

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined(i_implement) || defined(i_static)

STC_DEF void
_c_MEMB(_sift_down_)(i_type* self, const intptr_t idx, const intptr_t n) {
    _m_value t, *arr = self->data - 1;
    for (intptr_t r = idx, c = idx*2; c <= n; c *= 2) {
        c += i_less((&arr[c]), (&arr[c + (c < n)]));
        if (!(i_less((&arr[r]), (&arr[c])))) return;
        t = arr[r], arr[r] = arr[c], arr[r = c] = t;
    }
}

STC_DEF void
_c_MEMB(_make_heap)(i_type* self) {
    intptr_t n = self->_len;
    for (intptr_t k = n/2; k != 0; --k)
        _c_MEMB(_sift_down_)(self, k, n);
}

#if !defined i_no_clone
STC_DEF i_type _c_MEMB(_clone)(i_type q) {
    i_type out = _c_MEMB(_with_capacity)(q._len);
    for (; out._len < out._cap; ++q.data)
        out.data[out._len++] = i_keyclone((*q.data));
    return out;
}
#endif

STC_DEF void
_c_MEMB(_erase_at)(i_type* self, const intptr_t idx) {
    i_keydrop((self->data + idx));
    const intptr_t n = --self->_len;
    self->data[idx] = self->data[n];
    _c_MEMB(_sift_down_)(self, idx + 1, n);
}

STC_DEF _m_value*
_c_MEMB(_push)(i_type* self, _m_value value) {
    if (self->_len == self->_cap)
        _c_MEMB(_reserve)(self, self->_len*3/2 + 4);
    _m_value *arr = self->data - 1; /* base 1 */
    intptr_t c = ++self->_len;
    for (; c > 1 && (i_less((&arr[c/2]), (&value))); c /= 2)
        arr[c] = arr[c/2];
    arr[c] = value;
    return arr + c;
}

#endif
#undef _i_ispque
#include "priv/template2.h"
#include "priv/linkage2.h"