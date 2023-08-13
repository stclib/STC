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
#include <stc/cstr.h>
#include <stc/forward.h>

forward_cvec(cvec_i32, int);

struct MyStruct {
    cvec_i32 int_vec;
    cstr name;
} typedef MyStruct;

#define i_key float
#include <stc/cvec.h>

#define i_key_str // special for cstr
#include <stc/cvec.h>

#define i_key int
#define i_is_forward
#define i_tag i32
#include <stc/cvec.h>

int main(void) {
    cvec_i32 vec = {0};
    cvec_i32_push(&vec, 123);
    cvec_i32_drop(&vec);

    cvec_float fvec = {0};
    cvec_float_push(&fvec, 123.3);
    cvec_float_drop(&fvec);

    cvec_str svec = {0};
    cvec_str_emplace(&svec, "Hello, friend");
    cvec_str_drop(&svec);
}
*/
#include "priv/linkage.h"

#ifndef CVEC_H_INCLUDED
#include "ccommon.h"
#include "forward.h"
#include <stdlib.h>
#include <string.h>

#define _it2_ptr(it1, it2) (it1.ref && !it2.ref ? it2.end : it2.ref)
#define _it_ptr(it) (it.ref ? it.ref : it.end)
#endif // CVEC_H_INCLUDED

#define _i_prefix cvec_
#include "priv/template.h"

#ifndef i_is_forward
   _cx_DEFTYPES(_c_cvec_types, _cx_Self, i_key);
#endif
typedef i_keyraw _cx_raw;
STC_API _cx_Self        _cx_MEMB(_init)(void);
STC_API void            _cx_MEMB(_drop)(_cx_Self* self);
STC_API void            _cx_MEMB(_clear)(_cx_Self* self);
STC_API bool            _cx_MEMB(_reserve)(_cx_Self* self, intptr_t cap);
STC_API bool            _cx_MEMB(_resize)(_cx_Self* self, intptr_t size, i_key null);
STC_API _cx_iter        _cx_MEMB(_erase_n)(_cx_Self* self, intptr_t idx, intptr_t n);
STC_API _cx_iter        _cx_MEMB(_insert_uninit)(_cx_Self* self, intptr_t idx, intptr_t n);
#if defined _i_has_eq || defined _i_has_cmp
STC_API _cx_iter        _cx_MEMB(_find_in)(_cx_iter it1, _cx_iter it2, _cx_raw raw);
#endif
#if defined _i_has_cmp
STC_API int             _cx_MEMB(_value_cmp)(const _cx_value* x, const _cx_value* y);
STC_API _cx_iter        _cx_MEMB(_binary_search_in)(_cx_iter it1, _cx_iter it2, _cx_raw raw, _cx_iter* lower_bound);
#endif
STC_INLINE void         _cx_MEMB(_value_drop)(_cx_value* val) { i_keydrop(val); }

STC_INLINE _cx_value*   _cx_MEMB(_push)(_cx_Self* self, i_key value) {
                            if (self->_len == self->_cap)
                            if (!_cx_MEMB(_reserve)(self, self->_len*2 + 4))
                                return NULL;
                            _cx_value *v = self->data + self->_len++;
                            *v = value;
                            return v;
                        }

#if !defined i_no_emplace
STC_API _cx_iter
_cx_MEMB(_emplace_n)(_cx_Self* self, intptr_t idx, const _cx_raw raw[], intptr_t n);

STC_INLINE _cx_value* _cx_MEMB(_emplace)(_cx_Self* self, _cx_raw raw) {
    return _cx_MEMB(_push)(self, i_keyfrom(raw));
}
STC_INLINE _cx_value* _cx_MEMB(_emplace_back)(_cx_Self* self, _cx_raw raw) {
     return _cx_MEMB(_push)(self, i_keyfrom(raw));
}
STC_INLINE _cx_iter _cx_MEMB(_emplace_at)(_cx_Self* self, _cx_iter it, _cx_raw raw) {
    return _cx_MEMB(_emplace_n)(self, _it_ptr(it) - self->data, &raw, 1);
}
#endif // !i_no_emplace

#if !defined i_no_clone
STC_API _cx_Self        _cx_MEMB(_clone)(_cx_Self cx);
STC_API _cx_iter        _cx_MEMB(_copy_n)(_cx_Self* self, intptr_t idx, const _cx_value arr[], intptr_t n);
STC_INLINE void         _cx_MEMB(_put_n)(_cx_Self* self, const _cx_raw* raw, intptr_t n)
                            { while (n--) _cx_MEMB(_push)(self, i_keyfrom(*raw++)); }
STC_INLINE _cx_Self     _cx_MEMB(_from_n)(const _cx_raw* raw, intptr_t n)
                            { _cx_Self cx = {0}; _cx_MEMB(_put_n)(&cx, raw, n); return cx; }
STC_INLINE i_key        _cx_MEMB(_value_clone)(_cx_value val)
                            { return i_keyclone(val); }
STC_INLINE void         _cx_MEMB(_copy)(_cx_Self* self, const _cx_Self* other) {
                            if (self->data == other->data) return;
                            _cx_MEMB(_clear)(self);
                            _cx_MEMB(_copy_n)(self, 0, other->data, other->_len);
                        }
#endif // !i_no_clone

STC_INLINE intptr_t     _cx_MEMB(_size)(const _cx_Self* self) { return self->_len; }
STC_INLINE intptr_t     _cx_MEMB(_capacity)(const _cx_Self* self) { return self->_cap; }
STC_INLINE bool         _cx_MEMB(_empty)(const _cx_Self* self) { return !self->_len; }
STC_INLINE _cx_raw      _cx_MEMB(_value_toraw)(const _cx_value* val) { return i_keyto(val); }
STC_INLINE _cx_value*   _cx_MEMB(_front)(const _cx_Self* self) { return self->data; }
STC_INLINE _cx_value*   _cx_MEMB(_back)(const _cx_Self* self)
                            { return self->data + self->_len - 1; }
STC_INLINE void         _cx_MEMB(_pop)(_cx_Self* self)
                            { c_assert(self->_len); _cx_value* p = &self->data[--self->_len]; i_keydrop(p); }
STC_INLINE _cx_value    _cx_MEMB(_pull)(_cx_Self* self)
                            { c_assert(self->_len); return self->data[--self->_len]; }
STC_INLINE _cx_value*   _cx_MEMB(_push_back)(_cx_Self* self, i_key value)
                            { return _cx_MEMB(_push)(self, value); }
STC_INLINE void         _cx_MEMB(_pop_back)(_cx_Self* self) { _cx_MEMB(_pop)(self); }

STC_INLINE _cx_Self
_cx_MEMB(_with_size)(const intptr_t size, i_key null) {
    _cx_Self cx = _cx_MEMB(_init)();
    _cx_MEMB(_resize)(&cx, size, null);
    return cx;
}

STC_INLINE _cx_Self
_cx_MEMB(_with_capacity)(const intptr_t cap) {
    _cx_Self cx = _cx_MEMB(_init)();
    _cx_MEMB(_reserve)(&cx, cap);
    return cx;
}

STC_INLINE void
_cx_MEMB(_shrink_to_fit)(_cx_Self* self) {
    _cx_MEMB(_reserve)(self, _cx_MEMB(_size)(self));
}

STC_INLINE _cx_iter
_cx_MEMB(_insert_n)(_cx_Self* self, const intptr_t idx, const _cx_value arr[], const intptr_t n) {
    _cx_iter it = _cx_MEMB(_insert_uninit)(self, idx, n);
    if (it.ref)
        c_memcpy(it.ref, arr, n*c_sizeof *arr);
    return it;
}
STC_INLINE _cx_iter
_cx_MEMB(_insert_at)(_cx_Self* self, _cx_iter it, const _cx_value value) {
    return _cx_MEMB(_insert_n)(self, _it_ptr(it) - self->data, &value, 1);
}

STC_INLINE _cx_iter
_cx_MEMB(_erase_at)(_cx_Self* self, _cx_iter it) {
    return _cx_MEMB(_erase_n)(self, it.ref - self->data, 1);
}
STC_INLINE _cx_iter
_cx_MEMB(_erase_range)(_cx_Self* self, _cx_iter i1, _cx_iter i2) {
    return _cx_MEMB(_erase_n)(self, i1.ref - self->data, _it2_ptr(i1, i2) - i1.ref);
}

STC_INLINE const _cx_value*
_cx_MEMB(_at)(const _cx_Self* self, const intptr_t idx) {
    c_assert(idx < self->_len); return self->data + idx;
}
STC_INLINE _cx_value*
_cx_MEMB(_at_mut)(_cx_Self* self, const intptr_t idx) {
    c_assert(idx < self->_len); return self->data + idx;
}


STC_INLINE _cx_iter _cx_MEMB(_begin)(const _cx_Self* self) { 
    intptr_t n = self->_len; 
    return c_LITERAL(_cx_iter){n ? self->data : NULL, self->data + n};
}

STC_INLINE _cx_iter _cx_MEMB(_end)(const _cx_Self* self) 
    { return c_LITERAL(_cx_iter){NULL, self->data + self->_len}; }

STC_INLINE void _cx_MEMB(_next)(_cx_iter* it) 
    { if (++it->ref == it->end) it->ref = NULL; }

STC_INLINE _cx_iter _cx_MEMB(_advance)(_cx_iter it, size_t n)
    { if ((it.ref += n) >= it.end) it.ref = NULL; return it; }

STC_INLINE intptr_t _cx_MEMB(_index)(const _cx_Self* self, _cx_iter it) 
    { return (it.ref - self->data); }

STC_INLINE void _cx_MEMB(_adjust_end_)(_cx_Self* self, intptr_t n)
    { self->_len += n; }

#if defined _i_has_eq || defined _i_has_cmp

STC_INLINE _cx_iter
_cx_MEMB(_find)(const _cx_Self* self, _cx_raw raw) {
    return _cx_MEMB(_find_in)(_cx_MEMB(_begin)(self), _cx_MEMB(_end)(self), raw);
}

STC_INLINE const _cx_value*
_cx_MEMB(_get)(const _cx_Self* self, _cx_raw raw) {
    return _cx_MEMB(_find)(self, raw).ref;
}

STC_INLINE _cx_value*
_cx_MEMB(_get_mut)(const _cx_Self* self, _cx_raw raw)
    { return (_cx_value*) _cx_MEMB(_get)(self, raw); }

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
#if defined _i_has_cmp

STC_INLINE _cx_iter
_cx_MEMB(_binary_search)(const _cx_Self* self, _cx_raw raw) {
    _cx_iter lower;
    return _cx_MEMB(_binary_search_in)(_cx_MEMB(_begin)(self), _cx_MEMB(_end)(self), raw, &lower);
}

STC_INLINE _cx_iter
_cx_MEMB(_lower_bound)(const _cx_Self* self, _cx_raw raw) {
    _cx_iter lower;
    _cx_MEMB(_binary_search_in)(_cx_MEMB(_begin)(self), _cx_MEMB(_end)(self), raw, &lower);
    return lower;
}

STC_INLINE void
_cx_MEMB(_sort_range)(_cx_iter i1, _cx_iter i2, int(*cmp)(const _cx_value*, const _cx_value*)) {
    qsort(i1.ref, (size_t)(_it2_ptr(i1, i2) - i1.ref), sizeof(_cx_value),
          (int(*)(const void*, const void*)) cmp);
}

STC_INLINE void
_cx_MEMB(_sort)(_cx_Self* self) {
    _cx_MEMB(_sort_range)(_cx_MEMB(_begin)(self), _cx_MEMB(_end)(self), _cx_MEMB(_value_cmp));
}
#endif // _i_has_cmp
/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined(i_implement) || defined(i_static)

STC_DEF _cx_Self
_cx_MEMB(_init)(void) {
    return c_LITERAL(_cx_Self){NULL};
}

STC_DEF void
_cx_MEMB(_clear)(_cx_Self* self) {
    if (self->_cap) {
        for (_cx_value *p = self->data, *q = p + self->_len; p != q; ) {
            --q; i_keydrop(q);
        }
        self->_len = 0;
    }
}

STC_DEF void
_cx_MEMB(_drop)(_cx_Self* self) {
    if (self->_cap == 0)
        return;
    _cx_MEMB(_clear)(self);
    i_free(self->data);
}

STC_DEF bool
_cx_MEMB(_reserve)(_cx_Self* self, const intptr_t cap) {
    if (cap > self->_cap || (cap && cap == self->_len)) {
        _cx_value* d = (_cx_value*)i_realloc(self->data, cap*c_sizeof(i_key));
        if (!d)
            return false;
        self->data = d;
        self->_cap = cap;
    }
    return true;
}

STC_DEF bool
_cx_MEMB(_resize)(_cx_Self* self, const intptr_t len, i_key null) {
    if (!_cx_MEMB(_reserve)(self, len))
        return false;
    const intptr_t n = self->_len;
    for (intptr_t i = len; i < n; ++i)
        { i_keydrop((self->data + i)); }
    for (intptr_t i = n; i < len; ++i)
        self->data[i] = null;
    self->_len = len;
    return true;
}

STC_DEF _cx_iter
_cx_MEMB(_insert_uninit)(_cx_Self* self, const intptr_t idx, const intptr_t n) {
    if (self->_len + n > self->_cap)
        if (!_cx_MEMB(_reserve)(self, self->_len*5/4 + n))
            return _cx_MEMB(_end)(self);

    _cx_value* pos = self->data + idx;
    c_memmove(pos + n, pos, (self->_len - idx)*c_sizeof *pos);
    self->_len += n;
    return c_LITERAL(_cx_iter){pos, self->data + self->_len};
}

STC_DEF _cx_iter
_cx_MEMB(_erase_n)(_cx_Self* self, const intptr_t idx, const intptr_t len) {
    _cx_value* d = self->data + idx, *p = d, *end = self->data + self->_len;
    for (intptr_t i = 0; i < len; ++i, ++p)
        { i_keydrop(p); }
    c_memmove(d, p, (end - p)*c_sizeof *d);
    self->_len -= len;
    return c_LITERAL(_cx_iter){p == end ? NULL : d, end - len};
}

#if !defined i_no_clone
STC_DEF _cx_Self
_cx_MEMB(_clone)(_cx_Self cx) {
    _cx_Self out = _cx_MEMB(_init)();
    _cx_MEMB(_copy_n)(&out, 0, cx.data, cx._len);
    return out;
}

STC_DEF _cx_iter
_cx_MEMB(_copy_n)(_cx_Self* self, const intptr_t idx,
                  const _cx_value arr[], const intptr_t n) {
    _cx_iter it = _cx_MEMB(_insert_uninit)(self, idx, n);
    if (it.ref)
        for (_cx_value* p = it.ref, *q = p + n; p != q; ++arr)
            *p++ = i_keyclone((*arr));
    return it;
}
#endif // !i_no_clone

#if !defined i_no_emplace
STC_DEF _cx_iter
_cx_MEMB(_emplace_n)(_cx_Self* self, const intptr_t idx, const _cx_raw raw[], intptr_t n) {
    _cx_iter it = _cx_MEMB(_insert_uninit)(self, idx, n);
    if (it.ref)
        for (_cx_value* p = it.ref; n--; ++raw, ++p)
            *p = i_keyfrom((*raw));
    return it;
}
#endif // !i_no_emplace
#if defined _i_has_eq || defined _i_has_cmp

STC_DEF _cx_iter
_cx_MEMB(_find_in)(_cx_iter i1, _cx_iter i2, _cx_raw raw) {
    const _cx_value* p2 = _it2_ptr(i1, i2);
    for (; i1.ref != p2; ++i1.ref) {
        const _cx_raw r = i_keyto(i1.ref);
        if (i_eq((&raw), (&r)))
            return i1;
    }
    i2.ref = NULL;
    return i2;
}
#endif
#if defined _i_has_cmp

STC_DEF _cx_iter
_cx_MEMB(_binary_search_in)(_cx_iter i1, _cx_iter i2, const _cx_raw raw,
                            _cx_iter* lower_bound) {
    _cx_value* w[2] = {i1.ref, _it2_ptr(i1, i2)};
    _cx_iter mid = i1;
    while (w[0] != w[1]) {
        mid.ref = w[0] + (w[1] - w[0])/2;
        const _cx_raw m = i_keyto(mid.ref);
        const int c = i_cmp((&raw), (&m));

        if (!c) return *lower_bound = mid;
        w[c < 0] = mid.ref + (c > 0);
    }
    i1.ref = w[0] == i2.end ? NULL : w[0];
    *lower_bound = i1;
    i1.ref = NULL; return i1;
}

STC_DEF int _cx_MEMB(_value_cmp)(const _cx_value* x, const _cx_value* y) {
    const _cx_raw rx = i_keyto(x);
    const _cx_raw ry = i_keyto(y);
    return i_cmp((&rx), (&ry));
}
#endif // _i_has_cmp
#endif // i_implement
#define CVEC_H_INCLUDED
#include "priv/template2.h"
