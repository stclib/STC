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
#include "ccommon.h"

#ifndef CSTACK_H_INCLUDED
#define CSTACK_H_INCLUDED
#include <stdlib.h>
#include "forward.h"
#endif // CSTACK_H_INCLUDED

#ifndef _i_prefix
#define _i_prefix cstack_
#endif
#include "priv/template.h"

#ifndef i_is_forward
#ifdef i_capacity
  #define i_no_clone
  _cx_deftypes(_c_cstack_fixed, _cx_self, i_key, i_capacity);
#else
  _cx_deftypes(_c_cstack_types, _cx_self, i_key);
#endif
#endif
typedef i_keyraw _cx_raw;

STC_INLINE _cx_self _cx_memb(_init)(void) { 
    _cx_self cx; cx._len = 0;
#ifndef i_capacity
    cx._cap = 0; cx.data = NULL;
#endif
    return cx;
}

#ifdef i_capacity
STC_INLINE void _cx_memb(_create)(_cx_self* self)
    { self->_len = 0; }
#else
STC_INLINE void _cx_memb(_create)(_cx_self* self)
    { self->_len = 0; self->_cap = 0; self->data = NULL; }

STC_INLINE _cx_self _cx_memb(_with_capacity)(intptr_t cap) {
    _cx_self out = {(_cx_value *) i_malloc(cap*c_sizeof(i_key)), 0, cap};
    return out;
}

STC_INLINE _cx_self _cx_memb(_with_size)(intptr_t size, i_key null) {
    _cx_self out = {(_cx_value *) i_malloc(size*c_sizeof null), size, size};
    while (size) out.data[--size] = null;
    return out;
}
#endif // i_capacity

STC_INLINE void _cx_memb(_clear)(_cx_self* self) {
    _cx_value *p = self->data + self->_len;
    while (p-- != self->data) { i_keydrop(p); }
    self->_len = 0;
}

STC_INLINE void _cx_memb(_drop)(_cx_self* self) {
    _cx_memb(_clear)(self);
#ifndef i_capacity
    i_free(self->data);
#endif
}
STC_INLINE intptr_t _cx_memb(_size)(const _cx_self* self)
    { return self->_len; }

STC_INLINE bool _cx_memb(_empty)(const _cx_self* self)
    { return !self->_len; }

STC_INLINE intptr_t _cx_memb(_capacity)(const _cx_self* self) { 
#ifndef i_capacity
    return self->_cap; 
#else
    return i_capacity;
#endif
}
STC_INLINE void _cx_memb(_value_drop)(_cx_value* val)
    { i_keydrop(val); }

STC_INLINE bool _cx_memb(_reserve)(_cx_self* self, intptr_t n) {
    if (n < self->_len) return true;
#ifndef i_capacity
    _cx_value *t = (_cx_value *)i_realloc(self->data, n*c_sizeof *t);
    if (t) { self->_cap = n, self->data = t; return true; }
#endif
    return false;
}

STC_INLINE _cx_value* _cx_memb(_append_uninit)(_cx_self *self, intptr_t n) {
    intptr_t len = self->_len;
    if (!_cx_memb(_reserve)(self, len + n)) return NULL;
    self->_len += n;
    return self->data + len;
}

STC_INLINE void _cx_memb(_shrink_to_fit)(_cx_self* self)
    { _cx_memb(_reserve)(self, self->_len); }

STC_INLINE const _cx_value* _cx_memb(_top)(const _cx_self* self)
    { return &self->data[self->_len - 1]; }

STC_INLINE _cx_value* _cx_memb(_back)(const _cx_self* self)
    { return (_cx_value*) &self->data[self->_len - 1]; }

STC_INLINE _cx_value* _cx_memb(_front)(const _cx_self* self)
    { return (_cx_value*) &self->data[0]; }

STC_INLINE _cx_value* _cx_memb(_push)(_cx_self* self, _cx_value val) {
    if (self->_len == _cx_memb(_capacity)(self))
        if (!_cx_memb(_reserve)(self, self->_len*3/2 + 4))
            return NULL;
    _cx_value* vp = self->data + self->_len++;
    *vp = val; return vp;
}

STC_INLINE void _cx_memb(_pop)(_cx_self* self)
    { assert(!_cx_memb(_empty)(self)); _cx_value* p = &self->data[--self->_len]; i_keydrop(p); }

STC_INLINE void _cx_memb(_put_n)(_cx_self* self, const _cx_raw* raw, intptr_t n)
    { while (n--) _cx_memb(_push)(self, i_keyfrom(*raw++)); }

STC_INLINE _cx_self _cx_memb(_from_n)(const _cx_raw* raw, intptr_t n)
    { _cx_self cx = {0}; _cx_memb(_put_n)(&cx, raw, n); return cx; }

STC_INLINE const _cx_value* _cx_memb(_at)(const _cx_self* self, intptr_t idx)
    { assert(idx < self->_len); return self->data + idx; }
STC_INLINE _cx_value* _cx_memb(_at_mut)(_cx_self* self, intptr_t idx)
    { assert(idx < self->_len); return self->data + idx; }

#if !defined i_no_emplace
STC_INLINE _cx_value* _cx_memb(_emplace)(_cx_self* self, _cx_raw raw)
    { return _cx_memb(_push)(self, i_keyfrom(raw)); }
#endif // !i_no_emplace

#if !defined i_no_clone
STC_INLINE _cx_self _cx_memb(_clone)(_cx_self v) {
    _cx_self out = {(_cx_value *)i_malloc(v._len*c_sizeof(_cx_value)), v._len, v._len};
    if (!out.data) out._cap = 0;
    else for (intptr_t i = 0; i < v._len; ++v.data)
        out.data[i++] = i_keyclone((*v.data));
    return out;
}

STC_INLINE void _cx_memb(_copy)(_cx_self *self, const _cx_self* other) {
    if (self->data == other->data) return;
    _cx_memb(_drop)(self);
    *self = _cx_memb(_clone)(*other);
}

STC_INLINE i_key _cx_memb(_value_clone)(_cx_value val)
    { return i_keyclone(val); }

STC_INLINE i_keyraw _cx_memb(_value_toraw)(const _cx_value* val)
    { return i_keyto(val); }
#endif // !i_no_clone

STC_INLINE _cx_iter _cx_memb(_begin)(const _cx_self* self) {
    return c_LITERAL(_cx_iter){self->_len ? (_cx_value*)self->data : NULL,
                            (_cx_value*)self->data + self->_len};
}

STC_INLINE _cx_iter _cx_memb(_end)(const _cx_self* self)
    { return c_LITERAL(_cx_iter){NULL, (_cx_value*)self->data + self->_len}; }

STC_INLINE void _cx_memb(_next)(_cx_iter* it)
    { if (++it->ref == it->end) it->ref = NULL; }

STC_INLINE _cx_iter _cx_memb(_advance)(_cx_iter it, size_t n)
    { if ((it.ref += n) >= it.end) it.ref = NULL ; return it; }

#include "priv/template2.h"
