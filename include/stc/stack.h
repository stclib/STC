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

#ifndef STC_STACK_H_INCLUDED
#define STC_STACK_H_INCLUDED
#include "common.h"
#include <stdlib.h>
#include "types.h"
#endif // STC_STACK_H_INCLUDED

#ifndef _i_prefix
  #define _i_prefix stack_
#endif
#include "priv/template.h"

#ifndef i_is_forward
#ifdef i_capacity
  #define i_no_clone
  _c_DEFTYPES(_c_stack_fixed, i_type, i_key, i_capacity);
#else
  _c_DEFTYPES(_c_stack_types, i_type, i_key);
#endif
#endif
typedef i_keyraw _m_raw;

#ifdef i_capacity
STC_INLINE void _c_MEMB(_init)(i_type* self)
    { self->_len = 0; }
#else
STC_INLINE i_type _c_MEMB(_init)(void) {
    i_type out = {0};
    return out;
}

STC_INLINE i_type _c_MEMB(_with_capacity)(intptr_t cap) {
    i_type out = {(_m_value *) i_malloc(cap*c_sizeof(_m_value)), 0, cap};
    return out;
}

STC_INLINE i_type _c_MEMB(_with_size)(intptr_t size, _m_value null) {
    i_type out = {(_m_value *) i_malloc(size*c_sizeof null), size, size};
    while (size) out.data[--size] = null;
    return out;
}
#endif // i_capacity

STC_INLINE void _c_MEMB(_clear)(i_type* self) {
    _m_value *p = self->data + self->_len;
    while (p-- != self->data) { i_keydrop(p); }
    self->_len = 0;
}

STC_INLINE void _c_MEMB(_drop)(const i_type* cself) {
    i_type* self = (i_type*)cself;
    _c_MEMB(_clear)(self);
#ifndef i_capacity
    i_free(self->data, self->_cap*c_sizeof(*self->data));
#endif
}

STC_INLINE intptr_t _c_MEMB(_size)(const i_type* self)
    { return self->_len; }

STC_INLINE bool _c_MEMB(_empty)(const i_type* self)
    { return !self->_len; }

STC_INLINE intptr_t _c_MEMB(_capacity)(const i_type* self) {
#ifndef i_capacity
    return self->_cap;
#else
    (void)self; return i_capacity;
#endif
}
STC_INLINE void _c_MEMB(_value_drop)(_m_value* val)
    { i_keydrop(val); }

STC_INLINE bool _c_MEMB(_reserve)(i_type* self, intptr_t n) {
    if (n < self->_len) return true;
#ifndef i_capacity
    _m_value *d = (_m_value *)i_realloc(self->data, self->_cap*c_sizeof *d, n*c_sizeof *d);
    if (d) { self->_cap = n, self->data = d; return true; }
#endif
    return false;
}

STC_INLINE _m_value* _c_MEMB(_append_uninit)(i_type *self, intptr_t n) {
    intptr_t len = self->_len;
    if (len + n > _c_MEMB(_capacity)(self))
        if (!_c_MEMB(_reserve)(self, len*3/2 + n))
            return NULL;
    self->_len += n;
    return self->data + len;
}

STC_INLINE void _c_MEMB(_shrink_to_fit)(i_type* self)
    { _c_MEMB(_reserve)(self, self->_len); }

STC_INLINE const _m_value* _c_MEMB(_top)(const i_type* self)
    { return &self->data[self->_len - 1]; }

STC_INLINE _m_value* _c_MEMB(_back)(const i_type* self)
    { return (_m_value*) &self->data[self->_len - 1]; }

STC_INLINE _m_value* _c_MEMB(_front)(const i_type* self)
    { return (_m_value*) &self->data[0]; }

STC_INLINE _m_value* _c_MEMB(_push)(i_type* self, _m_value val) {
    if (self->_len == _c_MEMB(_capacity)(self))
        if (!_c_MEMB(_reserve)(self, self->_len*3/2 + 4))
            return NULL;
    _m_value* vp = self->data + self->_len++;
    *vp = val; return vp;
}

STC_INLINE void _c_MEMB(_pop)(i_type* self)
    { c_assert(self->_len); _m_value* p = &self->data[--self->_len]; i_keydrop(p); }

STC_INLINE _m_value _c_MEMB(_pull)(i_type* self)
    { c_assert(self->_len); return self->data[--self->_len]; }

STC_INLINE void _c_MEMB(_put_n)(i_type* self, const _m_raw* raw, intptr_t n)
    { while (n--) _c_MEMB(_push)(self, i_keyfrom(*raw++)); }

STC_INLINE i_type _c_MEMB(_from_n)(const _m_raw* raw, intptr_t n)
    { i_type cx = {0}; _c_MEMB(_put_n)(&cx, raw, n); return cx; }

STC_INLINE const _m_value* _c_MEMB(_at)(const i_type* self, intptr_t idx)
    { c_assert(idx < self->_len); return self->data + idx; }
STC_INLINE _m_value* _c_MEMB(_at_mut)(i_type* self, intptr_t idx)
    { c_assert(idx < self->_len); return self->data + idx; }

#if !defined i_no_emplace
STC_INLINE _m_value* _c_MEMB(_emplace)(i_type* self, _m_raw raw)
    { return _c_MEMB(_push)(self, i_keyfrom(raw)); }
#endif // !i_no_emplace

#if !defined i_no_clone
STC_INLINE i_type _c_MEMB(_clone)(i_type v) {
    i_type out = {(_m_value *)i_malloc(v._len*c_sizeof(_m_value)), v._len, v._len};
    if (!out.data) out._cap = 0;
    else for (intptr_t i = 0; i < v._len; ++v.data)
        out.data[i++] = i_keyclone((*v.data));
    return out;
}

STC_INLINE void _c_MEMB(_copy)(i_type *self, const i_type* other) {
    if (self->data == other->data) return;
    _c_MEMB(_drop)(self);
    *self = _c_MEMB(_clone)(*other);
}

STC_INLINE _m_value _c_MEMB(_value_clone)(_m_value val)
    { return i_keyclone(val); }

STC_INLINE i_keyraw _c_MEMB(_value_toraw)(const _m_value* val)
    { return i_keyto(val); }
#endif // !i_no_clone

STC_INLINE _m_iter _c_MEMB(_begin)(const i_type* self) {
    return c_LITERAL(_m_iter){self->_len ? (_m_value*)self->data : NULL,
                              (_m_value*)self->data + self->_len};
}

STC_INLINE _m_iter _c_MEMB(_end)(const i_type* self)
    { return c_LITERAL(_m_iter){NULL, (_m_value*)self->data + self->_len}; }

STC_INLINE void _c_MEMB(_next)(_m_iter* it)
    { if (++it->ref == it->end) it->ref = NULL; }

STC_INLINE _m_iter _c_MEMB(_advance)(_m_iter it, size_t n)
    { if ((it.ref += n) >= it.end) it.ref = NULL ; return it; }

STC_INLINE intptr_t _c_MEMB(_index)(const i_type* self, _m_iter it)
    { return (it.ref - self->data); }

STC_INLINE void _c_MEMB(_adjust_end_)(i_type* self, intptr_t n)
    { self->_len += n; }

#if defined _i_has_eq
STC_INLINE bool
_c_MEMB(_eq)(const i_type* self, const i_type* other) {
    if (self->_len != other->_len) return false;
    for (intptr_t i = 0; i < self->_len; ++i) {
        const _m_raw _rx = i_keyto(self->data+i), _ry = i_keyto(other->data+i);
        if (!(i_eq((&_rx), (&_ry)))) return false;
    }
    return true;
}
#endif
#include "priv/template2.h"
#include "priv/linkage2.h"
