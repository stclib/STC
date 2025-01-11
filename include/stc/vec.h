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

/*
#define i_implement
#include "stc/cstr.h"
#include "stc/types.h"

declare_vec(vec_i32, int);

typedef struct MyStruct {
    vec_i32 int_vec;
    cstr name;
} MyStruct;

#define i_key float
#include "stc/vec.h"

#define i_keypro cstr // cstr is a "pro"-type
#include "stc/vec.h"

#define i_type vec_i32,int32_t
#define i_declared
#include "stc/vec.h"

int main(void) {
    vec_i32 vec = {0};
    vec_i32_push(&vec, 123);
    vec_i32_drop(&vec);

    vec_float fvec = {0};
    vec_float_push(&fvec, 123.3);
    vec_float_drop(&fvec);

    vec_cstr svec = {0};
    vec_cstr_emplace(&svec, "Hello, friend");
    vec_cstr_drop(&svec);
}
*/
#include "priv/linkage.h"

#ifndef STC_VEC_H_INCLUDED
#define STC_VEC_H_INCLUDED
#include "common.h"
#include "types.h"
#include <stdlib.h>

#define _it2_ptr(it1, it2) (it1.ref && !it2.ref ? it1.end : it2.ref)
#define _it_ptr(it) (it.ref ? it.ref : it.end)
#endif // STC_VEC_H_INCLUDED

#ifndef _i_prefix
  #define _i_prefix vec_
#endif
#include "priv/template.h"

#ifndef i_declared
   _c_DEFTYPES(_c_vec_types, Self, i_key);
#endif
typedef i_keyraw _m_raw;
STC_API void            _c_MEMB(_drop)(const Self* cself);
STC_API void            _c_MEMB(_clear)(Self* self);
STC_API bool            _c_MEMB(_reserve)(Self* self, isize cap);
STC_API bool            _c_MEMB(_resize)(Self* self, isize size, _m_value null);
STC_API _m_iter         _c_MEMB(_erase_n)(Self* self, isize idx, isize n);
STC_API _m_iter         _c_MEMB(_insert_uninit)(Self* self, isize idx, isize n);
#if defined _i_has_eq
STC_API _m_iter         _c_MEMB(_find_in)(_m_iter it1, _m_iter it2, _m_raw raw);
#endif // _i_has_eq
STC_INLINE Self         _c_MEMB(_init)(void) { return c_literal(Self){0}; }
STC_INLINE void         _c_MEMB(_value_drop)(_m_value* val) { i_keydrop(val); }

STC_INLINE Self _c_MEMB(_move)(Self *self) {
    Self m = *self;
    memset(self, 0, sizeof *self);
    return m;
}

STC_INLINE void _c_MEMB(_take)(Self *self, Self unowned) {
    _c_MEMB(_drop)(self);
    *self = unowned;
}

STC_INLINE _m_value* _c_MEMB(_push)(Self* self, _m_value value) {
    if (self->size == self->capacity)
        if (!_c_MEMB(_reserve)(self, self->size*2 + 4))
            return NULL;
    _m_value *v = self->data + self->size++;
    *v = value;
    return v;
}

STC_INLINE void _c_MEMB(_put_n)(Self* self, const _m_raw* raw, isize n)
    { while (n--) _c_MEMB(_push)(self, i_keyfrom((*raw))), ++raw; }

STC_INLINE Self _c_MEMB(_with_n)(const _m_raw* raw, isize n)
    { Self cx = {0}; _c_MEMB(_put_n)(&cx, raw, n); return cx; }

#if !defined i_no_emplace
STC_API _m_iter _c_MEMB(_emplace_n)(Self* self, isize idx, const _m_raw raw[], isize n);

STC_INLINE _m_value* _c_MEMB(_emplace)(Self* self, _m_raw raw)
    { return _c_MEMB(_push)(self, i_keyfrom(raw)); }

STC_INLINE _m_value* _c_MEMB(_emplace_back)(Self* self, _m_raw raw)
    { return _c_MEMB(_push)(self, i_keyfrom(raw)); }

STC_INLINE _m_iter _c_MEMB(_emplace_at)(Self* self, _m_iter it, _m_raw raw)
    { return _c_MEMB(_emplace_n)(self, _it_ptr(it) - self->data, &raw, 1); }
#endif // !i_no_emplace

#if !defined i_no_clone
STC_API Self            _c_MEMB(_clone)(Self cx);
STC_API _m_iter         _c_MEMB(_copy_n)(Self* self, isize idx, const _m_value arr[], isize n);

STC_INLINE _m_value     _c_MEMB(_value_clone)(_m_value val)
                            { return i_keyclone(val); }

STC_INLINE void         _c_MEMB(_copy)(Self* self, const Self other) {
                            if (self->data == other.data) return;
                            _c_MEMB(_clear)(self);
                            _c_MEMB(_copy_n)(self, 0, other.data, other.size);
                        }
#endif // !i_no_clone

STC_INLINE isize        _c_MEMB(_size)(const Self* self) { return self->size; }
STC_INLINE isize        _c_MEMB(_capacity)(const Self* self) { return self->capacity; }
STC_INLINE bool         _c_MEMB(_is_empty)(const Self* self) { return !self->size; }
STC_INLINE _m_raw       _c_MEMB(_value_toraw)(const _m_value* val) { return i_keytoraw(val); }
STC_INLINE const _m_value*  _c_MEMB(_front)(const Self* self) { return self->data; }
STC_INLINE _m_value*        _c_MEMB(_front_mut)(Self* self) { return self->data; }
STC_INLINE const _m_value*  _c_MEMB(_back)(const Self* self) { return &self->data[self->size - 1]; }
STC_INLINE _m_value*        _c_MEMB(_back_mut)(Self* self) { return &self->data[self->size - 1]; }

STC_INLINE void         _c_MEMB(_pop)(Self* self)
                            { c_assert(self->size); _m_value* p = &self->data[--self->size]; i_keydrop(p); }
STC_INLINE _m_value     _c_MEMB(_pull)(Self* self)
                            { c_assert(self->size); return self->data[--self->size]; }
STC_INLINE _m_value*    _c_MEMB(_push_back)(Self* self, _m_value value)
                            { return _c_MEMB(_push)(self, value); }
STC_INLINE void         _c_MEMB(_pop_back)(Self* self) { _c_MEMB(_pop)(self); }

STC_INLINE Self _c_MEMB(_with_size)(const isize size, _m_value null) {
    Self cx = {0};
    _c_MEMB(_resize)(&cx, size, null);
    return cx;
}

STC_INLINE Self _c_MEMB(_with_capacity)(const isize cap) {
    Self cx = {0};
    _c_MEMB(_reserve)(&cx, cap);
    return cx;
}

STC_INLINE void _c_MEMB(_shrink_to_fit)(Self* self) {
    _c_MEMB(_reserve)(self, _c_MEMB(_size)(self));
}

STC_INLINE _m_iter
_c_MEMB(_insert_n)(Self* self, const isize idx, const _m_value arr[], const isize n) {
    _m_iter it = _c_MEMB(_insert_uninit)(self, idx, n);
    if (it.ref)
        c_memcpy(it.ref, arr, n*c_sizeof *arr);
    return it;
}

STC_INLINE _m_iter _c_MEMB(_insert_at)(Self* self, _m_iter it, const _m_value value) {
    return _c_MEMB(_insert_n)(self, _it_ptr(it) - self->data, &value, 1);
}

STC_INLINE _m_iter _c_MEMB(_erase_at)(Self* self, _m_iter it) {
    return _c_MEMB(_erase_n)(self, it.ref - self->data, 1);
}

STC_INLINE _m_iter _c_MEMB(_erase_range)(Self* self, _m_iter i1, _m_iter i2) {
    return _c_MEMB(_erase_n)(self, i1.ref - self->data, _it2_ptr(i1, i2) - i1.ref);
}

STC_INLINE const _m_value* _c_MEMB(_at)(const Self* self, const isize idx) {
    c_assert(idx < self->size); return self->data + idx;
}

STC_INLINE _m_value* _c_MEMB(_at_mut)(Self* self, const isize idx) {
    c_assert(idx < self->size); return self->data + idx;
}

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

STC_INLINE _m_iter _c_MEMB(_advance)(_m_iter it, size_t n) {
    if ((it.ref += n) >= it.end) it.ref = NULL;
    return it;
}

STC_INLINE isize _c_MEMB(_index)(const Self* self, _m_iter it)
    { return (it.ref - self->data); }

STC_INLINE void _c_MEMB(_adjust_end_)(Self* self, isize n)
    { self->size += n; }

#if defined _i_has_eq
STC_INLINE _m_iter _c_MEMB(_find)(const Self* self, _m_raw raw) {
    return _c_MEMB(_find_in)(_c_MEMB(_begin)(self), _c_MEMB(_end)(self), raw);
}

STC_INLINE bool _c_MEMB(_eq)(const Self* self, const Self* other) {
    if (self->size != other->size) return false;
    for (isize i = 0; i < self->size; ++i) {
        const _m_raw _rx = i_keytoraw((self->data+i)), _ry = i_keytoraw((other->data+i));
        if (!(i_eq((&_rx), (&_ry)))) return false;
    }
    return true;
}
#endif // _i_has_eq

#if defined _i_has_cmp
#include "priv/sort_prv.h"
#endif // _i_has_cmp

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined i_implement

STC_DEF void
_c_MEMB(_clear)(Self* self) {
    if (self->size == 0) return;
    _m_value *p = self->data + self->size;
    while (p-- != self->data) { i_keydrop(p); }
    self->size = 0;
}

STC_DEF void
_c_MEMB(_drop)(const Self* cself) {
    Self* self = (Self*)cself;
    if (self->capacity == 0)
        return;
    _c_MEMB(_clear)(self);
    i_free(self->data, self->capacity*c_sizeof(*self->data));
}

STC_DEF bool
_c_MEMB(_reserve)(Self* self, const isize cap) {
    if (cap > self->capacity || (cap && cap == self->size)) {
        _m_value* d = (_m_value*)i_realloc(self->data, self->capacity*c_sizeof *d,
                                           cap*c_sizeof *d);
        if (d == NULL)
            return false;
        self->data = d;
        self->capacity = cap;
    }
    return true;
}

STC_DEF bool
_c_MEMB(_resize)(Self* self, const isize len, _m_value null) {
    if (!_c_MEMB(_reserve)(self, len))
        return false;
    const isize n = self->size;
    for (isize i = len; i < n; ++i)
        { i_keydrop((self->data + i)); }
    for (isize i = n; i < len; ++i)
        self->data[i] = null;
    self->size = len;
    return true;
}

STC_DEF _m_iter
_c_MEMB(_insert_uninit)(Self* self, const isize idx, const isize n) {
    if (self->size + n > self->capacity)
        if (!_c_MEMB(_reserve)(self, self->size*3/2 + n))
            return _c_MEMB(_end)(self);

    _m_value* pos = self->data + idx;
    c_memmove(pos + n, pos, (self->size - idx)*c_sizeof *pos);
    self->size += n;
    return c_literal(_m_iter){pos, self->data + self->size};
}

STC_DEF _m_iter
_c_MEMB(_erase_n)(Self* self, const isize idx, const isize len) {
    c_assert(idx + len <= self->size);
    _m_value* d = self->data + idx, *p = d, *end = self->data + self->size;
    for (isize i = 0; i < len; ++i, ++p)
        { i_keydrop(p); }
    memmove(d, p, (size_t)(end - p)*sizeof *d);
    self->size -= len;
    return c_literal(_m_iter){p == end ? NULL : d, end - len};
}

#if !defined i_no_clone
STC_DEF Self
_c_MEMB(_clone)(Self vec) {
    Self tmp = {0};
    _c_MEMB(_copy_n)(&tmp, 0, vec.data, vec.size);
    vec.data = tmp.data;
    vec.capacity = tmp.capacity;
    return vec;
}

STC_DEF _m_iter
_c_MEMB(_copy_n)(Self* self, const isize idx,
                 const _m_value arr[], const isize n) {
    _m_iter it = _c_MEMB(_insert_uninit)(self, idx, n);
    if (it.ref)
        for (_m_value* p = it.ref, *q = p + n; p != q; ++arr)
            *p++ = i_keyclone((*arr));
    return it;
}
#endif // !i_no_clone

#if !defined i_no_emplace
STC_DEF _m_iter
_c_MEMB(_emplace_n)(Self* self, const isize idx, const _m_raw raw[], isize n) {
    _m_iter it = _c_MEMB(_insert_uninit)(self, idx, n);
    if (it.ref)
        for (_m_value* p = it.ref; n--; ++raw, ++p)
            *p = i_keyfrom((*raw));
    return it;
}
#endif // !i_no_emplace

#if defined _i_has_eq
STC_DEF _m_iter
_c_MEMB(_find_in)(_m_iter i1, _m_iter i2, _m_raw raw) {
    const _m_value* p2 = _it2_ptr(i1, i2);
    for (; i1.ref != p2; ++i1.ref) {
        const _m_raw r = i_keytoraw(i1.ref);
        if (i_eq((&raw), (&r)))
            return i1;
    }
    i2.ref = NULL;
    return i2;
}
#endif //  _i_has_eq
#endif // i_implement
#include "priv/linkage2.h"
#include "priv/template2.h"
