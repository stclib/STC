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

#ifndef CSTACK_H_INCLUDED
#define CSTACK_H_INCLUDED
#include "ccommon.h"
#include <stdlib.h>
#include "forward.h"
#endif // CSTACK_H_INCLUDED

#define _i_prefix cstack_
#include "priv/template.h"

#ifndef i_is_forward
#ifdef i_capacity
  #define i_no_clone
  _cx_DEFTYPES(_c_cstack_fixed, _cx_Self, i_key, i_capacity);
#else
  _cx_DEFTYPES(_c_cstack_types, _cx_Self, i_key);
#endif
#endif
typedef i_keyraw _cx_raw;

#ifdef i_capacity
STC_INLINE void _cx_MEMB(_init)(_cx_Self* self)
    { self->_len = 0; }
#else
STC_INLINE _cx_Self _cx_MEMB(_init)(void) { 
    _cx_Self out = {0};
    return out;
}

STC_INLINE _cx_Self _cx_MEMB(_with_capacity)(intptr_t cap) {
    _cx_Self out = {(_cx_value *) i_malloc(cap*c_sizeof(i_key)), 0, cap};
    return out;
}

STC_INLINE _cx_Self _cx_MEMB(_with_size)(intptr_t size, i_key null) {
    _cx_Self out = {(_cx_value *) i_malloc(size*c_sizeof null), size, size};
    while (size) out.data[--size] = null;
    return out;
}
#endif // i_capacity

STC_INLINE void _cx_MEMB(_clear)(_cx_Self* self) {
    _cx_value *p = self->data + self->_len;
    while (p-- != self->data) { i_keydrop(p); }
    self->_len = 0;
}

STC_INLINE void _cx_MEMB(_drop)(_cx_Self* self) {
    _cx_MEMB(_clear)(self);
#ifndef i_capacity
    i_free(self->data);
#endif
}

STC_INLINE intptr_t _cx_MEMB(_size)(const _cx_Self* self)
    { return self->_len; }

STC_INLINE bool _cx_MEMB(_empty)(const _cx_Self* self)
    { return !self->_len; }

STC_INLINE intptr_t _cx_MEMB(_capacity)(const _cx_Self* self) { 
#ifndef i_capacity
    return self->_cap; 
#else
    return i_capacity;
#endif
}
STC_INLINE void _cx_MEMB(_value_drop)(_cx_value* val)
    { i_keydrop(val); }

STC_INLINE bool _cx_MEMB(_reserve)(_cx_Self* self, intptr_t n) {
    if (n < self->_len) return true;
#ifndef i_capacity
    _cx_value *t = (_cx_value *)i_realloc(self->data, n*c_sizeof *t);
    if (t) { self->_cap = n, self->data = t; return true; }
#endif
    return false;
}

STC_INLINE _cx_value* _cx_MEMB(_append_uninit)(_cx_Self *self, intptr_t n) {
    intptr_t len = self->_len;
    if (!_cx_MEMB(_reserve)(self, len*3/2 + n)) return NULL;
    self->_len += n;
    return self->data + len;
}

STC_INLINE void _cx_MEMB(_shrink_to_fit)(_cx_Self* self)
    { _cx_MEMB(_reserve)(self, self->_len); }

STC_INLINE const _cx_value* _cx_MEMB(_top)(const _cx_Self* self)
    { return &self->data[self->_len - 1]; }

STC_INLINE _cx_value* _cx_MEMB(_back)(const _cx_Self* self)
    { return (_cx_value*) &self->data[self->_len - 1]; }

STC_INLINE _cx_value* _cx_MEMB(_front)(const _cx_Self* self)
    { return (_cx_value*) &self->data[0]; }

STC_INLINE _cx_value* _cx_MEMB(_push)(_cx_Self* self, _cx_value val) {
    if (self->_len == _cx_MEMB(_capacity)(self))
        if (!_cx_MEMB(_reserve)(self, self->_len*3/2 + 4))
            return NULL;
    _cx_value* vp = self->data + self->_len++;
    *vp = val; return vp;
}

STC_INLINE void _cx_MEMB(_pop)(_cx_Self* self)
    { c_assert(self->_len); _cx_value* p = &self->data[--self->_len]; i_keydrop(p); }

STC_INLINE _cx_value _cx_MEMB(_pull)(_cx_Self* self)
    { c_assert(self->_len); return self->data[--self->_len]; }

STC_INLINE void _cx_MEMB(_put_n)(_cx_Self* self, const _cx_raw* raw, intptr_t n)
    { while (n--) _cx_MEMB(_push)(self, i_keyfrom(*raw++)); }

STC_INLINE _cx_Self _cx_MEMB(_from_n)(const _cx_raw* raw, intptr_t n)
    { _cx_Self cx = {0}; _cx_MEMB(_put_n)(&cx, raw, n); return cx; }

STC_INLINE const _cx_value* _cx_MEMB(_at)(const _cx_Self* self, intptr_t idx)
    { c_assert(idx < self->_len); return self->data + idx; }
STC_INLINE _cx_value* _cx_MEMB(_at_mut)(_cx_Self* self, intptr_t idx)
    { c_assert(idx < self->_len); return self->data + idx; }

#if !defined i_no_emplace
STC_INLINE _cx_value* _cx_MEMB(_emplace)(_cx_Self* self, _cx_raw raw)
    { return _cx_MEMB(_push)(self, i_keyfrom(raw)); }
#endif // !i_no_emplace

#if !defined i_no_clone
STC_INLINE _cx_Self _cx_MEMB(_clone)(_cx_Self v) {
    _cx_Self out = {(_cx_value *)i_malloc(v._len*c_sizeof(_cx_value)), v._len, v._len};
    if (!out.data) out._cap = 0;
    else for (intptr_t i = 0; i < v._len; ++v.data)
        out.data[i++] = i_keyclone((*v.data));
    return out;
}

STC_INLINE void _cx_MEMB(_copy)(_cx_Self *self, const _cx_Self* other) {
    if (self->data == other->data) return;
    _cx_MEMB(_drop)(self);
    *self = _cx_MEMB(_clone)(*other);
}

STC_INLINE i_key _cx_MEMB(_value_clone)(_cx_value val)
    { return i_keyclone(val); }

STC_INLINE i_keyraw _cx_MEMB(_value_toraw)(const _cx_value* val)
    { return i_keyto(val); }
#endif // !i_no_clone

STC_INLINE _cx_iter _cx_MEMB(_begin)(const _cx_Self* self) {
    return c_LITERAL(_cx_iter){self->_len ? (_cx_value*)self->data : NULL,
                               (_cx_value*)self->data + self->_len};
}

STC_INLINE _cx_iter _cx_MEMB(_end)(const _cx_Self* self)
    { return c_LITERAL(_cx_iter){NULL, (_cx_value*)self->data + self->_len}; }

STC_INLINE void _cx_MEMB(_next)(_cx_iter* it)
    { if (++it->ref == it->end) it->ref = NULL; }

STC_INLINE _cx_iter _cx_MEMB(_advance)(_cx_iter it, size_t n)
    { if ((it.ref += n) >= it.end) it.ref = NULL ; return it; }

STC_INLINE intptr_t _cx_MEMB(_index)(const _cx_Self* self, _cx_iter it) 
    { return (it.ref - self->data); }

STC_INLINE void _cx_MEMB(_adjust_end_)(_cx_Self* self, intptr_t n)
    { self->_len += n; }

#if defined _i_has_eq || defined _i_has_cmp
STC_INLINE bool
_cx_MEMB(_eq)(const _cx_Self* self, const _cx_Self* other) {
    if (self->_len != other->_len) return false;
    for (intptr_t i = 0; i < self->_len; ++i) {
        const _cx_raw _rx = i_keyto(self->data+i), _ry = i_keyto(other->data+i);
        if (!(i_eq((&_rx), (&_ry)))) return false;
    }
    return true;
}
#endif
#include "priv/template2.h"
