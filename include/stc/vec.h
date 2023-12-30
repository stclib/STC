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

/*
#define i_implement
#include "stc/cstr.h"
#include "stc/types.h"

forward_vec(vec_i32, int);

struct MyStruct {
    vec_i32 int_vec;
    cstr name;
} typedef MyStruct;

#define i_key float
#include "stc/vec.h"

#define i_key_str // special for cstr
#include "stc/vec.h"

#define i_TYPE vec_i32,int32_t
#define i_is_forward
#include "stc/vec.h"

int main(void) {
    vec_i32 vec = {0};
    vec_i32_push(&vec, 123);
    vec_i32_drop(&vec);

    vec_float fvec = {0};
    vec_float_push(&fvec, 123.3);
    vec_float_drop(&fvec);

    vec_str svec = {0};
    vec_str_emplace(&svec, "Hello, friend");
    vec_str_drop(&svec);
}
*/
#include "priv/linkage.h"

#ifndef STC_VEC_H_INCLUDED
#define STC_VEC_H_INCLUDED
#include "common.h"
#include "types.h"
#include <stdlib.h>
#include <string.h>

#define _it2_ptr(it1, it2) (it1.ref && !it2.ref ? it2.end : it2.ref)
#define _it_ptr(it) (it.ref ? it.ref : it.end)
#endif // STC_VEC_H_INCLUDED

#ifndef _i_prefix
  #define _i_prefix vec_
#endif
#include "priv/template.h"

#ifndef i_is_forward
   _c_DEFTYPES(_c_vec_types, i_type, i_key);
#endif
typedef i_keyraw _m_raw;
STC_API i_type          _c_MEMB(_init)(void);
STC_API void            _c_MEMB(_drop)(const i_type* cself);
STC_API void            _c_MEMB(_clear)(i_type* self);
STC_API bool            _c_MEMB(_reserve)(i_type* self, intptr_t cap);
STC_API bool            _c_MEMB(_resize)(i_type* self, intptr_t size, _m_value null);
STC_API _m_iter         _c_MEMB(_erase_n)(i_type* self, intptr_t idx, intptr_t n);
STC_API _m_iter         _c_MEMB(_insert_uninit)(i_type* self, intptr_t idx, intptr_t n);
#if defined _i_has_eq
STC_API _m_iter         _c_MEMB(_find_in)(_m_iter it1, _m_iter it2, _m_raw raw);
#endif
STC_INLINE void         _c_MEMB(_value_drop)(_m_value* val) { i_keydrop(val); }

STC_INLINE _m_value*    _c_MEMB(_push)(i_type* self, _m_value value) {
                            if (self->_len == self->_cap)
                            if (!_c_MEMB(_reserve)(self, self->_len*2 + 4))
                                return NULL;
                            _m_value *v = self->data + self->_len++;
                            *v = value;
                            return v;
                        }

#if !defined i_no_emplace
STC_API _m_iter
_c_MEMB(_emplace_n)(i_type* self, intptr_t idx, const _m_raw raw[], intptr_t n);

STC_INLINE _m_value* _c_MEMB(_emplace)(i_type* self, _m_raw raw) {
    return _c_MEMB(_push)(self, i_keyfrom(raw));
}
STC_INLINE _m_value* _c_MEMB(_emplace_back)(i_type* self, _m_raw raw) {
     return _c_MEMB(_push)(self, i_keyfrom(raw));
}
STC_INLINE _m_iter _c_MEMB(_emplace_at)(i_type* self, _m_iter it, _m_raw raw) {
    return _c_MEMB(_emplace_n)(self, _it_ptr(it) - self->data, &raw, 1);
}
#endif // !i_no_emplace

#if !defined i_no_clone
STC_API i_type          _c_MEMB(_clone)(i_type cx);
STC_API _m_iter         _c_MEMB(_copy_n)(i_type* self, intptr_t idx, const _m_value arr[], intptr_t n);
STC_INLINE void         _c_MEMB(_put_n)(i_type* self, const _m_raw* raw, intptr_t n)
                            { while (n--) _c_MEMB(_push)(self, i_keyfrom(*raw++)); }
STC_INLINE i_type       _c_MEMB(_from_n)(const _m_raw* raw, intptr_t n)
                            { i_type cx = {0}; _c_MEMB(_put_n)(&cx, raw, n); return cx; }
STC_INLINE _m_value     _c_MEMB(_value_clone)(_m_value val)
                            { return i_keyclone(val); }
STC_INLINE void         _c_MEMB(_copy)(i_type* self, const i_type* other) {
                            if (self->data == other->data) return;
                            _c_MEMB(_clear)(self);
                            _c_MEMB(_copy_n)(self, 0, other->data, other->_len);
                        }
#endif // !i_no_clone

STC_INLINE intptr_t     _c_MEMB(_size)(const i_type* self) { return self->_len; }
STC_INLINE intptr_t     _c_MEMB(_capacity)(const i_type* self) { return self->_cap; }
STC_INLINE bool         _c_MEMB(_empty)(const i_type* self) { return !self->_len; }
STC_INLINE _m_raw       _c_MEMB(_value_toraw)(const _m_value* val) { return i_keyto(val); }
STC_INLINE _m_value*    _c_MEMB(_front)(const i_type* self) { return self->data; }
STC_INLINE _m_value*    _c_MEMB(_back)(const i_type* self)
                            { return self->data + self->_len - 1; }
STC_INLINE void         _c_MEMB(_pop)(i_type* self)
                            { c_assert(self->_len); _m_value* p = &self->data[--self->_len]; i_keydrop(p); }
STC_INLINE _m_value     _c_MEMB(_pull)(i_type* self)
                            { c_assert(self->_len); return self->data[--self->_len]; }
STC_INLINE _m_value*    _c_MEMB(_push_back)(i_type* self, _m_value value)
                            { return _c_MEMB(_push)(self, value); }
STC_INLINE void         _c_MEMB(_pop_back)(i_type* self) { _c_MEMB(_pop)(self); }

STC_INLINE i_type
_c_MEMB(_with_size)(const intptr_t size, _m_value null) {
    i_type cx = _c_MEMB(_init)();
    _c_MEMB(_resize)(&cx, size, null);
    return cx;
}

STC_INLINE i_type
_c_MEMB(_with_capacity)(const intptr_t cap) {
    i_type cx = _c_MEMB(_init)();
    _c_MEMB(_reserve)(&cx, cap);
    return cx;
}

STC_INLINE void
_c_MEMB(_shrink_to_fit)(i_type* self) {
    _c_MEMB(_reserve)(self, _c_MEMB(_size)(self));
}

STC_INLINE _m_iter
_c_MEMB(_insert_n)(i_type* self, const intptr_t idx, const _m_value arr[], const intptr_t n) {
    _m_iter it = _c_MEMB(_insert_uninit)(self, idx, n);
    if (it.ref)
        c_memcpy(it.ref, arr, n*c_sizeof *arr);
    return it;
}
STC_INLINE _m_iter
_c_MEMB(_insert_at)(i_type* self, _m_iter it, const _m_value value) {
    return _c_MEMB(_insert_n)(self, _it_ptr(it) - self->data, &value, 1);
}

STC_INLINE _m_iter
_c_MEMB(_erase_at)(i_type* self, _m_iter it) {
    return _c_MEMB(_erase_n)(self, it.ref - self->data, 1);
}
STC_INLINE _m_iter
_c_MEMB(_erase_range)(i_type* self, _m_iter i1, _m_iter i2) {
    return _c_MEMB(_erase_n)(self, i1.ref - self->data, _it2_ptr(i1, i2) - i1.ref);
}

STC_INLINE const _m_value*
_c_MEMB(_at)(const i_type* self, const intptr_t idx) {
    c_assert(idx < self->_len); return self->data + idx;
}
STC_INLINE _m_value*
_c_MEMB(_at_mut)(i_type* self, const intptr_t idx) {
    c_assert(idx < self->_len); return self->data + idx;
}


STC_INLINE _m_iter _c_MEMB(_begin)(const i_type* self) {
    intptr_t n = self->_len;
    return c_LITERAL(_m_iter){n ? self->data : NULL, self->data + n};
}

STC_INLINE _m_iter _c_MEMB(_end)(const i_type* self)
    { return c_LITERAL(_m_iter){NULL, self->data + self->_len}; }

STC_INLINE void _c_MEMB(_next)(_m_iter* it)
    { if (++it->ref == it->end) it->ref = NULL; }

STC_INLINE _m_iter _c_MEMB(_advance)(_m_iter it, size_t n)
    { if ((it.ref += n) >= it.end) it.ref = NULL; return it; }

STC_INLINE intptr_t _c_MEMB(_index)(const i_type* self, _m_iter it)
    { return (it.ref - self->data); }

STC_INLINE void _c_MEMB(_adjust_end_)(i_type* self, intptr_t n)
    { self->_len += n; }

#if defined _i_has_eq
STC_INLINE _m_iter
_c_MEMB(_find)(const i_type* self, _m_raw raw) {
    return _c_MEMB(_find_in)(_c_MEMB(_begin)(self), _c_MEMB(_end)(self), raw);
}

STC_INLINE const _m_value*
_c_MEMB(_get)(const i_type* self, _m_raw raw) {
    return _c_MEMB(_find)(self, raw).ref;
}

STC_INLINE _m_value*
_c_MEMB(_get_mut)(const i_type* self, _m_raw raw)
    { return (_m_value*) _c_MEMB(_get)(self, raw); }

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

#if defined _i_has_cmp
STC_API int _c_MEMB(_value_cmp)(const _m_value* x, const _m_value* y);

STC_INLINE void
_c_MEMB(_sort)(i_type* self) {
    qsort(self->data, (size_t)self->_len, sizeof(_m_value),
          (int(*)(const void*, const void*))_c_MEMB(_value_cmp));
}

STC_INLINE _m_value*
_c_MEMB(_bsearch)(const i_type* self, _m_value key) {
    return (_m_value*)bsearch(&key, self->data, (size_t)self->_len, sizeof(_m_value),
                              (int(*)(const void*, const void*))_c_MEMB(_value_cmp));
}
#endif // _i_has_cmp

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined(i_implement) || defined(i_static)

STC_DEF i_type
_c_MEMB(_init)(void) {
    return c_LITERAL(i_type){NULL};
}

STC_DEF void
_c_MEMB(_clear)(i_type* self) {
    if (self->_cap) {
        for (_m_value *p = self->data, *q = p + self->_len; p != q; ) {
            --q; i_keydrop(q);
        }
        self->_len = 0;
    }
}

STC_DEF void
_c_MEMB(_drop)(const i_type* cself) {
    i_type* self = (i_type*)cself;
    if (self->_cap == 0)
        return;
    _c_MEMB(_clear)(self);
    i_free(self->data, self->_cap*c_sizeof(*self->data));
}

STC_DEF bool
_c_MEMB(_reserve)(i_type* self, const intptr_t cap) {
    if (cap > self->_cap || (cap && cap == self->_len)) {
        _m_value* d = (_m_value*)i_realloc(self->data, self->_cap*c_sizeof *d,
                                                       cap*c_sizeof *d);
        if (!d)
            return false;
        self->data = d;
        self->_cap = cap;
    }
    return true;
}

STC_DEF bool
_c_MEMB(_resize)(i_type* self, const intptr_t len, _m_value null) {
    if (!_c_MEMB(_reserve)(self, len))
        return false;
    const intptr_t n = self->_len;
    for (intptr_t i = len; i < n; ++i)
        { i_keydrop((self->data + i)); }
    for (intptr_t i = n; i < len; ++i)
        self->data[i] = null;
    self->_len = len;
    return true;
}

STC_DEF _m_iter
_c_MEMB(_insert_uninit)(i_type* self, const intptr_t idx, const intptr_t n) {
    if (self->_len + n > self->_cap)
        if (!_c_MEMB(_reserve)(self, self->_len*3/2 + n))
            return _c_MEMB(_end)(self);

    _m_value* pos = self->data + idx;
    c_memmove(pos + n, pos, (self->_len - idx)*c_sizeof *pos);
    self->_len += n;
    return c_LITERAL(_m_iter){pos, self->data + self->_len};
}

STC_DEF _m_iter
_c_MEMB(_erase_n)(i_type* self, const intptr_t idx, const intptr_t len) {
    _m_value* d = self->data + idx, *p = d, *end = self->data + self->_len;
    for (intptr_t i = 0; i < len; ++i, ++p)
        { i_keydrop(p); }
    c_memmove(d, p, (end - p)*c_sizeof *d);
    self->_len -= len;
    return c_LITERAL(_m_iter){p == end ? NULL : d, end - len};
}

#if !defined i_no_clone
STC_DEF i_type
_c_MEMB(_clone)(i_type cx) {
    i_type out = _c_MEMB(_init)();
    _c_MEMB(_copy_n)(&out, 0, cx.data, cx._len);
    return out;
}

STC_DEF _m_iter
_c_MEMB(_copy_n)(i_type* self, const intptr_t idx,
                 const _m_value arr[], const intptr_t n) {
    _m_iter it = _c_MEMB(_insert_uninit)(self, idx, n);
    if (it.ref)
        for (_m_value* p = it.ref, *q = p + n; p != q; ++arr)
            *p++ = i_keyclone((*arr));
    return it;
}
#endif // !i_no_clone

#if !defined i_no_emplace
STC_DEF _m_iter
_c_MEMB(_emplace_n)(i_type* self, const intptr_t idx, const _m_raw raw[], intptr_t n) {
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
        const _m_raw r = i_keyto(i1.ref);
        if (i_eq((&raw), (&r)))
            return i1;
    }
    i2.ref = NULL;
    return i2;
}
#endif
#if defined _i_has_cmp
STC_DEF int _c_MEMB(_value_cmp)(const _m_value* x, const _m_value* y) {
    const _m_raw rx = i_keyto(x);
    const _m_raw ry = i_keyto(y);
    return i_cmp((&rx), (&ry));
}
#endif // _i_has_cmp
#endif // i_implement
#include "priv/template2.h"
#include "priv/linkage2.h"
