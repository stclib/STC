/* MIT License
 *
 * Copyright (c) 2025 Tyge Løvset
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

// Stack - a simplified vec type without linear search and insert/erase inside the stack.

#ifndef STC_STACK_H_INCLUDED
#define STC_STACK_H_INCLUDED
#include "common.h"
#include "types.h"
#include <stdlib.h>
#endif // STC_STACK_H_INCLUDED

#ifndef _i_prefix
  #define _i_prefix stack_
#endif
#include "priv/template.h"

#ifndef i_declared
#ifdef i_capacity
  #define i_no_clone
  _c_DEFTYPES(_c_stack_fixed, Self, i_key, i_capacity);
#else
  _c_DEFTYPES(_c_vec_types, Self, i_key);
#endif
#endif
typedef i_keyraw _m_raw;

STC_INLINE Self _c_MEMB(_init)(void)
    { Self s={0}; return s; }

#ifdef i_capacity
STC_INLINE Self _c_MEMB(_move)(Self *self)
    { return *self; }
#else

STC_INLINE Self _c_MEMB(_move)(Self *self) {
    Self m = *self;
    memset(self, 0, sizeof *self);
    return m;
}

STC_INLINE Self _c_MEMB(_with_capacity)(isize cap) {
    Self out = {_i_malloc(_m_value, cap), 0, cap};
    return out;
}

STC_INLINE Self _c_MEMB(_with_size)(isize size, _m_value null) {
    Self out = {_i_malloc(_m_value, size), size, size};
    while (size) out.data[--size] = null;
    return out;
}
#endif // i_capacity

STC_INLINE void _c_MEMB(_clear)(Self* self) {
    if (self->size == 0) return;
    _m_value *p = self->data + self->size;
    while (p-- != self->data) { i_keydrop(p); }
    self->size = 0;
}

STC_INLINE void _c_MEMB(_drop)(const Self* cself) {
    Self* self = (Self*)cself;
    _c_MEMB(_clear)(self);
#ifndef i_capacity
    i_free(self->data, self->capacity*c_sizeof(*self->data));
#endif
}

STC_INLINE void _c_MEMB(_take)(Self *self, Self unowned) {
    _c_MEMB(_drop)(self);
    *self = unowned;
}

STC_INLINE isize _c_MEMB(_size)(const Self* self)
    { return self->size; }

STC_INLINE bool _c_MEMB(_is_empty)(const Self* self)
    { return !self->size; }

STC_INLINE isize _c_MEMB(_capacity)(const Self* self) {
#ifndef i_capacity
    return self->capacity;
#else
    (void)self; return i_capacity;
#endif
}

STC_INLINE void _c_MEMB(_value_drop)(_m_value* val)
    { i_keydrop(val); }

STC_INLINE bool _c_MEMB(_reserve)(Self* self, isize n) {
    if (n < self->size) return true;
#ifndef i_capacity
    _m_value *d = (_m_value *)i_realloc(self->data, self->capacity*c_sizeof *d, n*c_sizeof *d);
    if (d) { self->capacity = n, self->data = d; return true; }
#endif
    return false;
}

STC_INLINE _m_value* _c_MEMB(_append_uninit)(Self *self, isize n) {
    isize len = self->size;
    if (len + n > _c_MEMB(_capacity)(self))
        if (!_c_MEMB(_reserve)(self, len*3/2 + n))
            return NULL;
    self->size += n;
    return self->data + len;
}

STC_INLINE void _c_MEMB(_shrink_to_fit)(Self* self)
    { _c_MEMB(_reserve)(self, self->size); }

STC_INLINE const _m_value* _c_MEMB(_front)(const Self* self)
    { return &self->data[0]; }
STC_INLINE _m_value* _c_MEMB(_front_mut)(Self* self)
    { return &self->data[0]; }

STC_INLINE const _m_value* _c_MEMB(_back)(const Self* self)
    { return &self->data[self->size - 1]; }
STC_INLINE _m_value* _c_MEMB(_back_mut)(Self* self)
    { return &self->data[self->size - 1]; }

STC_INLINE const _m_value* _c_MEMB(_top)(const Self* self)
    { return _c_MEMB(_back)(self); }
STC_INLINE _m_value* _c_MEMB(_top_mut)(Self* self)
    { return _c_MEMB(_back_mut)(self); }

STC_INLINE _m_value* _c_MEMB(_push)(Self* self, _m_value val) {
    if (self->size == _c_MEMB(_capacity)(self))
        if (!_c_MEMB(_reserve)(self, self->size*3/2 + 4))
            return NULL;
    _m_value* vp = self->data + self->size++;
    *vp = val; return vp;
}

STC_INLINE void _c_MEMB(_pop)(Self* self)
    { c_assert(self->size); _m_value* p = &self->data[--self->size]; i_keydrop(p); }

STC_INLINE _m_value _c_MEMB(_pull)(Self* self)
    { c_assert(self->size); return self->data[--self->size]; }

STC_INLINE void _c_MEMB(_put_n)(Self* self, const _m_raw* raw, isize n)
    { while (n--) _c_MEMB(_push)(self, i_keyfrom((*raw))), ++raw; }

STC_INLINE Self _c_MEMB(_with_n)(const _m_raw* raw, isize n)
    { Self cx = {0}; _c_MEMB(_put_n)(&cx, raw, n); return cx; }

STC_INLINE const _m_value* _c_MEMB(_at)(const Self* self, isize idx)
    { c_assert(idx < self->size); return self->data + idx; }

STC_INLINE _m_value* _c_MEMB(_at_mut)(Self* self, isize idx)
    { c_assert(idx < self->size); return self->data + idx; }

#if !defined i_no_emplace
STC_INLINE _m_value* _c_MEMB(_emplace)(Self* self, _m_raw raw)
    { return _c_MEMB(_push)(self, i_keyfrom(raw)); }
#endif // !i_no_emplace

#if !defined i_no_clone
STC_INLINE Self _c_MEMB(_clone)(Self s) {
    Self tmp = {_i_malloc(_m_value, s.size), s.size, s.size};
    if (tmp.data == NULL) tmp.capacity = 0;
    else for (isize i = 0; i < s.size; ++s.data)
        tmp.data[i++] = i_keyclone((*s.data));
    s.data = tmp.data;
    s.capacity = tmp.capacity;
    return s;
}

STC_INLINE void _c_MEMB(_copy)(Self *self, const Self other) {
    if (self->data == other.data) return;
    _c_MEMB(_drop)(self);
    *self = _c_MEMB(_clone)(other);
}

STC_INLINE _m_value _c_MEMB(_value_clone)(_m_value val)
    { return i_keyclone(val); }

STC_INLINE i_keyraw _c_MEMB(_value_toraw)(const _m_value* val)
    { return i_keytoraw(val); }
#endif // !i_no_clone

// iteration

STC_INLINE _m_iter _c_MEMB(_begin)(const Self* self) {
    _m_iter it = {(_m_value*)self->data, (_m_value*)self->data};
    if (it.ref != NULL) it.end += self->size;
    return it;
}

STC_INLINE _m_iter _c_MEMB(_rbegin)(const Self* self) {
    _m_iter it = {(_m_value*)self->data, (_m_value*)self->data};
    if (it.ref != NULL) { it.ref += self->size - 1; it.end -= 1; }
    return it;
}

STC_INLINE _m_iter _c_MEMB(_end)(const Self* self)
    { (void)self; _m_iter it = {0}; return it; }

STC_INLINE _m_iter _c_MEMB(_rend)(const Self* self)
    { (void)self; _m_iter it = {0}; return it; }

STC_INLINE void _c_MEMB(_next)(_m_iter* it)
    { if (++it->ref == it->end) it->ref = NULL; }

STC_INLINE void _c_MEMB(_rnext)(_m_iter* it)
    { if (--it->ref == it->end) it->ref = NULL; }

STC_INLINE _m_iter _c_MEMB(_advance)(_m_iter it, size_t n)
    { if ((it.ref += n) >= it.end) it.ref = NULL ; return it; }

STC_INLINE isize _c_MEMB(_index)(const Self* self, _m_iter it)
    { return (it.ref - self->data); }

STC_INLINE void _c_MEMB(_adjust_end_)(Self* self, isize n)
    { self->size += n; }

#if defined _i_has_cmp
#include "priv/sort_prv.h"
#endif // _i_has_cmp

#if defined _i_has_eq
STC_INLINE bool
_c_MEMB(_eq)(const Self* self, const Self* other) {
    if (self->size != other->size) return false;
    for (isize i = 0; i < self->size; ++i) {
        const _m_raw _rx = i_keytoraw((self->data+i)), _ry = i_keytoraw((other->data+i));
        if (!(i_eq((&_rx), (&_ry)))) return false;
    }
    return true;
}
#endif
#include "priv/linkage2.h"
#include "priv/template2.h"
