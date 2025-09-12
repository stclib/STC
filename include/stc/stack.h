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
#include "types.h"

// Stack - a simplified vec type without linear search and insert/erase inside the stack.

#ifndef STC_STACK_H_INCLUDED
#define STC_STACK_H_INCLUDED
#include "common.h"
#include <stdlib.h>
#endif // STC_STACK_H_INCLUDED

#ifndef _i_prefix
  #define _i_prefix stack_
#endif
#include "priv/template.h"
#ifndef i_declared
    #if c_NUMARGS(i_type) == 4
      #define i_capacity i_val
    #endif
    #ifdef i_capacity
      #define i_no_clone
      _c_DEFTYPES(declare_stack_fixed, Self, i_key, i_capacity);
    #else
      _c_DEFTYPES(_declare_stack, Self, i_key, _i_aux_def);
    #endif
#endif
typedef i_keyraw _m_raw;

#ifdef i_capacity
    STC_INLINE void _c_MEMB(_init)(Self* news)
        { news->size = 0; }

    STC_INLINE isize _c_MEMB(_capacity)(const Self* self)
        { (void)self; return i_capacity; }

    STC_INLINE bool _c_MEMB(_reserve)(Self* self, isize n)
        { (void)self; return n <= i_capacity; }
#else
    STC_INLINE Self _c_MEMB(_move)(Self *self) {
        Self m = *self;
        self->capacity = self->size = 0;
        self->data = NULL;
        return m;
    }

    STC_INLINE isize _c_MEMB(_capacity)(const Self* self)
        { return self->capacity; }

    STC_INLINE bool _c_MEMB(_reserve)(Self* self, isize n) {
        if (n > self->capacity || (n && n == self->size)) {
            _m_value *d = (_m_value *)_i_realloc_n(self->data, self->capacity, n);
            if (d == NULL)
                return false;
            self->data = d;
            self->capacity = n;
        }
        return self->data != NULL;
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
    _i_free_n(self->data, self->capacity);
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

STC_INLINE void _c_MEMB(_value_drop)(const Self* self, _m_value* val)
    { (void)self; i_keydrop(val); }

STC_INLINE _m_value* _c_MEMB(_append_uninit)(Self *self, isize n) {
    isize len = self->size;
    if (len + n >= _c_MEMB(_capacity)(self))
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

#ifndef _i_no_put
STC_INLINE void _c_MEMB(_put_n)(Self* self, const _m_raw* raw, isize n)
    { while (n--) _c_MEMB(_push)(self, i_keyfrom((*raw))), ++raw; }
#endif

#if !defined _i_aux_alloc && !defined i_capacity
    STC_INLINE Self _c_MEMB(_init)(void)
        { Self out = {0}; return out; }

    STC_INLINE Self _c_MEMB(_with_capacity)(isize cap)
        { Self out = {_i_new_n(_m_value, cap), 0, cap}; return out; }

    STC_INLINE Self _c_MEMB(_with_size_uninit)(isize size)
        { Self out = {_i_new_n(_m_value, size), size, size}; return out; }

    STC_INLINE Self _c_MEMB(_with_size)(isize size, _m_raw default_raw) {
        Self out = {_i_new_n(_m_value, size), size, size};
        while (size) out.data[--size] = i_keyfrom(default_raw);
        return out;
    }

    #ifndef _i_no_put
    STC_INLINE Self _c_MEMB(_from_n)(const _m_raw* raw, isize n) {
        Self out = _c_MEMB(_with_capacity)(n);
        _c_MEMB(_put_n)(&out, raw, n); return out;
    }
    #endif
#endif

STC_INLINE const _m_value* _c_MEMB(_at)(const Self* self, isize idx)
    { c_assert(c_uless(idx, self->size)); return self->data + idx; }

STC_INLINE _m_value* _c_MEMB(_at_mut)(Self* self, isize idx)
    { c_assert(c_uless(idx, self->size)); return self->data + idx; }

#ifndef i_no_emplace
STC_INLINE _m_value* _c_MEMB(_emplace)(Self* self, _m_raw raw)
    { return _c_MEMB(_push)(self, i_keyfrom(raw)); }
#endif // !i_no_emplace

#ifndef i_no_clone
STC_INLINE Self _c_MEMB(_clone)(Self stk) {
    Self out = stk, *self = &out; (void)self; // i_keyclone may use self via i_aux
    out.data = NULL; out.size = out.capacity = 0;
    _c_MEMB(_reserve)(&out, stk.size);
    out.size = stk.size;
    for (c_range(i, stk.size))
        out.data[i] = i_keyclone(stk.data[i]);
    return out;
}

STC_INLINE void _c_MEMB(_copy)(Self *self, const Self* other) {
    if (self == other) return;
    _c_MEMB(_clear)(self);
    _c_MEMB(_reserve)(self, other->size);
    for (c_range(i, other->size))
        self->data[self->size++] = i_keyclone((other->data[i]));
}

STC_INLINE _m_value _c_MEMB(_value_clone)(const Self* self, _m_value val)
    { (void)self; return i_keyclone(val); }

STC_INLINE _m_raw _c_MEMB(_value_toraw)(const _m_value* val)
    { return i_keytoraw(val); }
#endif // !i_no_clone

// iteration

STC_INLINE _m_iter _c_MEMB(_begin)(const Self* self) {
    _m_iter it = {(_m_value*)self->data, (_m_value*)self->data};
    if (self->size) it.end += self->size;
    else it.ref = NULL;
    return it;
}

STC_INLINE _m_iter _c_MEMB(_rbegin)(const Self* self) {
    _m_iter it = {(_m_value*)self->data, (_m_value*)self->data};
    if (self->size) { it.ref += self->size - 1; it.end -= 1; }
    else it.ref = NULL;
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
STC_INLINE _m_iter _c_MEMB(_find_in)(const Self* self, _m_iter i1, _m_iter i2, _m_raw raw) {
    (void)self;
    const _m_value* p2 = i2.ref ? i2.ref : i1.end;
    for (; i1.ref != p2; ++i1.ref) {
        const _m_raw r = i_keytoraw(i1.ref);
        if (i_eq((&raw), (&r)))
            return i1;
    }
    i2.ref = NULL;
    return i2;
}

STC_INLINE _m_iter _c_MEMB(_find)(const Self* self, _m_raw raw)
    { return _c_MEMB(_find_in)(self, _c_MEMB(_begin)(self), _c_MEMB(_end)(self), raw); }

STC_INLINE bool _c_MEMB(_eq)(const Self* self, const Self* other) {
    if (self->size != other->size) return false;
    for (isize i = 0; i < self->size; ++i) {
        const _m_raw _rx = i_keytoraw((self->data+i)), _ry = i_keytoraw((other->data+i));
        if (!(i_eq((&_rx), (&_ry)))) return false;
    }
    return true;
}
#endif // _i_has_eq
#include "sys/finalize.h"
