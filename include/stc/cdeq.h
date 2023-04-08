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

#ifndef CDEQ_H_INCLUDED
#include "forward.h"
#include <stdlib.h>
#include <string.h>

#define _it2_ptr(it1, it2) (it1.ref && !it2.ref ? it2.end : it2.ref)
#define _it_ptr(it) (it.ref ? it.ref : it.end)
#endif // CDEQ_H_INCLUDED

#ifndef _i_prefix
#define _i_prefix cdeq_
#endif
#include "priv/template.h"

#ifndef i_is_forward
_cx_deftypes(_c_cdeq_types, _cx_self, i_key);
#endif
typedef i_keyraw _cx_raw;

STC_API _cx_self        _cx_memb(_init)(void);
STC_API _cx_self        _cx_memb(_with_capacity)(const intptr_t n);
STC_API bool            _cx_memb(_reserve)(_cx_self* self, const intptr_t n);
STC_API void            _cx_memb(_clear)(_cx_self* self);
STC_API void            _cx_memb(_drop)(_cx_self* self);
STC_API _cx_value*      _cx_memb(_push)(_cx_self* self, i_key value);
STC_API void            _cx_memb(_shrink_to_fit)(_cx_self *self);
STC_INLINE void         _cx_memb(_put_n)(_cx_self* self, const _cx_raw* raw, intptr_t n)
                            { while (n--) _cx_memb(_push)(self, i_keyfrom(*raw++)); }
STC_INLINE _cx_self     _cx_memb(_from_n)(const _cx_raw* raw, intptr_t n)
                            { _cx_self cx = {0}; _cx_memb(_put_n)(&cx, raw, n); return cx; }
STC_INLINE void         _cx_memb(_value_drop)(_cx_value* val) { i_keydrop(val); }
#if !defined _i_queue
#if !defined i_no_emplace
STC_API _cx_iter        _cx_memb(_emplace_range)(_cx_self* self, _cx_value* pos,
                                                 const _cx_raw* p1, const _cx_raw* p2);
#endif // i_no_emplace

#if !defined i_no_cmp || defined _i_has_eq
STC_API _cx_iter        _cx_memb(_find_in)(_cx_iter p1, _cx_iter p2, _cx_raw raw);
#endif
#ifndef i_no_cmp
STC_API int             _cx_memb(_value_cmp)(const _cx_value* x, const _cx_value* y);
#endif
STC_API _cx_value*      _cx_memb(_push_front)(_cx_self* self, i_key value);
STC_API _cx_iter        _cx_memb(_erase_range_p)(_cx_self* self, _cx_value* p1, _cx_value* p2);
STC_API _cx_iter        _cx_memb(_insert_range)(_cx_self* self, _cx_value* pos,
                                                const _cx_value* p1, const _cx_value* p2);
#endif // !_i_queue

#if !defined i_no_emplace
STC_INLINE _cx_value*   _cx_memb(_emplace)(_cx_self* self, _cx_raw raw)
                            { return _cx_memb(_push)(self, i_keyfrom(raw)); }
#endif

#if !defined i_no_clone
#if !defined _i_queue
STC_API _cx_iter        _cx_memb(_copy_range)(_cx_self* self, _cx_value* pos,
                                                const _cx_value* p1, const _cx_value* p2);

STC_INLINE void         _cx_memb(_copy)(_cx_self *self, const _cx_self* other) {
                            if (self->data == other->data) return;
                            _cx_memb(_clear)(self);
                            _cx_memb(_copy_range)(self, self->data,
                                                  other->data, other->data + other->_len);
                        }
#endif // !_i_queue
STC_API _cx_self        _cx_memb(_clone)(_cx_self cx);
STC_INLINE i_key        _cx_memb(_value_clone)(i_key val)
                            { return i_keyclone(val); }
#endif // !i_no_clone
STC_INLINE intptr_t     _cx_memb(_size)(const _cx_self* self) { return self->_len; }
STC_INLINE intptr_t     _cx_memb(_capacity)(const _cx_self* self) { return self->_cap; }
STC_INLINE bool         _cx_memb(_empty)(const _cx_self* self) { return !self->_len; }
STC_INLINE _cx_raw      _cx_memb(_value_toraw)(const _cx_value* pval) { return i_keyto(pval); }
STC_INLINE _cx_value*   _cx_memb(_front)(const _cx_self* self) { return self->data; }
STC_INLINE _cx_value*   _cx_memb(_back)(const _cx_self* self)
                            { return self->data + self->_len - 1; }
STC_INLINE void         _cx_memb(_pop_front)(_cx_self* self) // == _pop() when _i_queue
                            { i_keydrop(self->data); ++self->data; --self->_len; }

STC_INLINE _cx_iter _cx_memb(_begin)(const _cx_self* self) {
    intptr_t n = self->_len;
    return c_LITERAL(_cx_iter){n ? self->data : NULL, self->data + n};
}

STC_INLINE _cx_iter _cx_memb(_end)(const _cx_self* self)
    { return c_LITERAL(_cx_iter){NULL, self->data + self->_len}; }

STC_INLINE void _cx_memb(_next)(_cx_iter* it)
    { if (++it->ref == it->end) it->ref = NULL; }

STC_INLINE _cx_iter _cx_memb(_advance)(_cx_iter it, size_t n)
    { if ((it.ref += n) >= it.end) it.ref = NULL; return it; }

#if !defined _i_queue

STC_INLINE intptr_t _cx_memb(_index)(const _cx_self* self, _cx_iter it)
    { return (it.ref - self->data); }
STC_INLINE void _cx_memb(_pop_back)(_cx_self* self)
    { _cx_value* p = &self->data[--self->_len]; i_keydrop(p); }

STC_INLINE const _cx_value* _cx_memb(_at)(const _cx_self* self, const intptr_t idx) {
    assert(idx < self->_len); return self->data + idx;
}
STC_INLINE _cx_value* _cx_memb(_at_mut)(_cx_self* self, const intptr_t idx) {
    assert(idx < self->_len); return self->data + idx;
}

STC_INLINE _cx_value* _cx_memb(_push_back)(_cx_self* self, i_key value) {
    return _cx_memb(_push)(self, value);
}
STC_INLINE _cx_iter
_cx_memb(_insert)(_cx_self* self, const intptr_t idx, i_key value) {
    return _cx_memb(_insert_range)(self, self->data + idx, &value, &value + 1);
}
STC_INLINE _cx_iter
_cx_memb(_insert_n)(_cx_self* self, const intptr_t idx, const _cx_value arr[], const intptr_t n) {
    return _cx_memb(_insert_range)(self, self->data + idx, arr, arr + n);
}
STC_INLINE _cx_iter
_cx_memb(_insert_at)(_cx_self* self, _cx_iter it, i_key value) {
    return _cx_memb(_insert_range)(self, _it_ptr(it), &value, &value + 1);
}

STC_INLINE _cx_iter
_cx_memb(_erase_n)(_cx_self* self, const intptr_t idx, const intptr_t n) {
    return _cx_memb(_erase_range_p)(self, self->data + idx, self->data + idx + n);
}
STC_INLINE _cx_iter
_cx_memb(_erase_at)(_cx_self* self, _cx_iter it) {
    return _cx_memb(_erase_range_p)(self, it.ref, it.ref + 1);
}
STC_INLINE _cx_iter
_cx_memb(_erase_range)(_cx_self* self, _cx_iter i1, _cx_iter i2) {
    return _cx_memb(_erase_range_p)(self, i1.ref, _it2_ptr(i1, i2));
}

#if !defined i_no_emplace
STC_INLINE _cx_value*
_cx_memb(_emplace_front)(_cx_self* self, _cx_raw raw) {
    return _cx_memb(_push_front)(self, i_keyfrom(raw));
}

STC_INLINE _cx_value* _cx_memb(_emplace_back)(_cx_self* self, _cx_raw raw) {
    return _cx_memb(_push)(self, i_keyfrom(raw));
}

STC_INLINE _cx_iter
_cx_memb(_emplace_n)(_cx_self* self, const intptr_t idx, const _cx_raw arr[], const intptr_t n) {
    return _cx_memb(_emplace_range)(self, self->data + idx, arr, arr + n);
}
STC_INLINE _cx_iter
_cx_memb(_emplace_at)(_cx_self* self, _cx_iter it, _cx_raw raw) {
    return _cx_memb(_emplace_range)(self, _it_ptr(it), &raw, &raw + 1);
}
#endif // !i_no_emplace

#if !defined i_no_cmp || defined _i_has_eq

STC_INLINE _cx_iter
_cx_memb(_find)(const _cx_self* self, _cx_raw raw) {
    return _cx_memb(_find_in)(_cx_memb(_begin)(self), _cx_memb(_end)(self), raw);
}

STC_INLINE const _cx_value*
_cx_memb(_get)(const _cx_self* self, _cx_raw raw) {
    return _cx_memb(_find_in)(_cx_memb(_begin)(self), _cx_memb(_end)(self), raw).ref;
}

STC_INLINE _cx_value*
_cx_memb(_get_mut)(_cx_self* self, _cx_raw raw)
    { return (_cx_value *) _cx_memb(_get)(self, raw); }

STC_INLINE bool
_cx_memb(_eq)(const _cx_self* self, const _cx_self* other) {
    if (self->_len != other->_len) return false;
    for (intptr_t i = 0; i < self->_len; ++i) {
        const _cx_raw _rx = i_keyto(self->data+i), _ry = i_keyto(other->data+i);
        if (!(i_eq((&_rx), (&_ry)))) return false;
    }
    return true;
}
#endif
#ifndef i_no_cmp

STC_INLINE void
_cx_memb(_sort_range)(_cx_iter i1, _cx_iter i2, int(*cmp)(const _cx_value*, const _cx_value*)) {
    qsort(i1.ref, (size_t)(_it2_ptr(i1, i2) - i1.ref), sizeof *i1.ref,
          (int(*)(const void*, const void*)) cmp);
}

STC_INLINE void
_cx_memb(_sort)(_cx_self* self) {
    _cx_memb(_sort_range)(_cx_memb(_begin)(self), _cx_memb(_end)(self), _cx_memb(_value_cmp));
}
#endif // !c_no_cmp
#endif // _i_queue

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined(i_implement)
#define _cdeq_nfront(self) ((self)->data - (self)->_base)

STC_DEF _cx_self
_cx_memb(_init)(void) {
    _cx_self cx = {NULL};
    return cx;
}

STC_DEF void
_cx_memb(_clear)(_cx_self* self) {
    if (self->_cap) {
        for (_cx_value *p = self->data, *q = p + self->_len; p != q; )
            { --q; i_keydrop(q); }
        self->_len = 0;
        self->data = self->_base;
    }
}

STC_DEF void
_cx_memb(_shrink_to_fit)(_cx_self *self) {
    if (self->_len != self->_cap) {
        c_memmove(self->_base, self->data, self->_len*c_sizeof(i_key));
        _cx_value* d = (_cx_value*)i_realloc(self->_base, self->_len*c_sizeof(i_key));
        if (d) {
            self->_base = d;
            self->_cap = self->_len;
        }
	self->data = self->_base;
    }
}

STC_DEF void
_cx_memb(_drop)(_cx_self* self) {
    if (self->_base) {
        _cx_memb(_clear)(self);
        i_free(self->_base);
    }
}

static intptr_t
_cx_memb(_realloc_)(_cx_self* self, const intptr_t n) {
    const intptr_t cap = (intptr_t)((float)self->_len*1.7f) + n + 7;
    const intptr_t nfront = _cdeq_nfront(self);
    _cx_value* d = (_cx_value*)i_realloc(self->_base, cap*c_sizeof(i_key));
    if (!d)
        return 0; 
    self->_cap = cap;
    self->_base = d;
    self->data = d + nfront;
    return cap;
}

static bool
_cx_memb(_expand_right_half_)(_cx_self* self, const intptr_t idx, const intptr_t n) {
    const intptr_t sz = self->_len, cap = self->_cap;
    const intptr_t nfront = _cdeq_nfront(self), nback = cap - sz - nfront;
    if (nback >= n || (intptr_t)((float)sz*1.3f) + n > cap) {
        if (!_cx_memb(_realloc_)(self, n))
            return false;
        c_memmove(self->data + idx + n, self->data + idx, (sz - idx)*c_sizeof(i_key));
    } else {
#if !defined _i_queue
        const intptr_t unused = cap - (sz + n);
        const intptr_t pos = (nfront*2 < unused) ? nfront : unused/2;
#else
        const intptr_t pos = 0;
#endif
        c_memmove(self->_base + pos, self->data, idx*c_sizeof(i_key));
        c_memmove(self->data + pos + idx + n, self->data + idx, (sz - idx)*c_sizeof(i_key));
        self->data = self->_base + pos;
    }
    return true;
}

STC_DEF _cx_self
_cx_memb(_with_capacity)(const intptr_t n) {
    _cx_self cx = _cx_memb(_init)();
    _cx_memb(_expand_right_half_)(&cx, 0, n);
    return cx;
}

STC_DEF bool
_cx_memb(_reserve)(_cx_self* self, const intptr_t n) {
    const intptr_t sz = self->_len;
    return n <= sz || _cx_memb(_expand_right_half_)(self, sz, n - sz);
}

STC_DEF _cx_value*
_cx_memb(_push)(_cx_self* self, i_key value) {
    if (_cdeq_nfront(self) + self->_len == self->_cap)
        _cx_memb(_expand_right_half_)(self, self->_len, 1);
    _cx_value *v = self->data + self->_len++;
    *v = value;
    return v;
}

#if !defined i_no_clone
STC_DEF _cx_self
_cx_memb(_clone)(_cx_self cx) {
    _cx_self out = _cx_memb(_with_capacity)(cx._len);
    if (out._base)
        for (intptr_t i = 0; i < cx._len; ++i) 
            out.data[i] = i_keyclone(cx.data[i]);
    return out;
}
#endif

#if !defined _i_queue

static void
_cx_memb(_expand_left_half_)(_cx_self* self, const intptr_t idx, const intptr_t n) {
    intptr_t cap = self->_cap;
    const intptr_t sz = self->_len;
    const intptr_t nfront = _cdeq_nfront(self), nback = cap - sz - nfront;
    if (nfront >= n) {
        self->data = (_cx_value *)c_memmove(self->data - n, self->data, idx*c_sizeof(i_key));
    } else {
        if ((intptr_t)((float)sz*1.3f) + n > cap)
            cap = _cx_memb(_realloc_)(self, n);
        const intptr_t unused = cap - (sz + n);
        const intptr_t pos = (nback*2 < unused) ? unused - nback : unused/2;
        c_memmove(self->_base + pos + idx + n, self->data + idx, (sz - idx)*c_sizeof(i_key));
        self->data = (_cx_value *)c_memmove(self->_base + pos, self->data, idx*c_sizeof(i_key));
    }
}

static _cx_iter
_cx_memb(_insert_uninit)(_cx_self* self, _cx_value* pos, const intptr_t n) {
    if (n) {
        if (!pos) pos = self->data + self->_len;
        const intptr_t idx = (pos - self->data);
        if (idx*2 < self->_len)
            _cx_memb(_expand_left_half_)(self, idx, n);
        else
            _cx_memb(_expand_right_half_)(self, idx, n);
        self->_len += n;
        pos = self->data + idx;
    }
    return c_LITERAL(_cx_iter){pos, self->data + self->_len};
}

STC_DEF _cx_value*
_cx_memb(_push_front)(_cx_self* self, i_key value) {
    if (self->data == self->_base)
        _cx_memb(_expand_left_half_)(self, 0, 1);
    else
        --self->data;
    ++self->_len;
    *self->data = value;
    return self->data;
}

STC_DEF _cx_iter
_cx_memb(_insert_range)(_cx_self* self, _cx_value* pos,
                          const _cx_value* p1, const _cx_value* p2) {
    _cx_iter it = _cx_memb(_insert_uninit)(self, pos, (p2 - p1));
    if (it.ref)
        c_memcpy(it.ref, p1, (p2 - p1)*c_sizeof *p1);
    return it;
}

STC_DEF _cx_iter
_cx_memb(_erase_range_p)(_cx_self* self, _cx_value* p1, _cx_value* p2) {
    assert(p1 && p2);
    intptr_t len = p2 - p1;
    _cx_value* p = p1, *end = self->data + self->_len;
    for (; p != p2; ++p)
        { i_keydrop(p); }
    c_memmove(p1, p2, (end - p2)*c_sizeof *p1);
    self->_len -= len;
    return c_LITERAL(_cx_iter){p2 == end ? NULL : p1, end - len};
}

#if !defined i_no_clone
STC_DEF _cx_iter
_cx_memb(_copy_range)(_cx_self* self, _cx_value* pos,
                      const _cx_value* p1, const _cx_value* p2) {
    _cx_iter it = _cx_memb(_insert_uninit)(self, pos, (p2 - p1));
    if (it.ref)
        for (_cx_value* p = it.ref; p1 != p2; ++p1)
            *p++ = i_keyclone((*p1));
    return it;
}
#endif // !i_no_clone

#if !defined i_no_emplace
STC_DEF _cx_iter
_cx_memb(_emplace_range)(_cx_self* self, _cx_value* pos,
                         const _cx_raw* p1, const _cx_raw* p2) {
    _cx_iter it = _cx_memb(_insert_uninit)(self, pos, (p2 - p1));
    if (it.ref)
        for (_cx_value* p = it.ref; p1 != p2; ++p1)
            *p++ = i_keyfrom((*p1));
    return it;
}
#endif // !i_no_emplace

#if !defined i_no_cmp || defined _i_has_eq
STC_DEF _cx_iter
_cx_memb(_find_in)(_cx_iter i1, _cx_iter i2, _cx_raw raw) {
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
#ifndef i_no_cmp
STC_DEF int
_cx_memb(_value_cmp)(const _cx_value* x, const _cx_value* y) {
    const _cx_raw rx = i_keyto(x);
    const _cx_raw ry = i_keyto(y);
    return i_cmp((&rx), (&ry));
}
#endif // !c_no_cmp
#endif // !_i_queue
#endif // IMPLEMENTATION
#define CDEQ_H_INCLUDED
#include "priv/template2.h"
