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

#ifndef STC_DEQ_H_INCLUDED
#define STC_DEQ_H_INCLUDED
#include "common.h"
#include "types.h"
#include <stdlib.h>
#include <string.h>
#endif // STC_DEQ_H_INCLUDED

#ifndef _i_prefix
  #define _i_prefix deq_
#endif
#define _pop _pop_front
#define _pull _pull_front
#include "priv/template.h"
#include "priv/queue_prv.h"
#undef _pop
#undef _pull

STC_API _m_value*   _c_MEMB(_push_front)(i_type* self, _m_value value);
STC_API _m_iter     _c_MEMB(_insert_n)(i_type* self, intptr_t idx, const _m_value* arr, intptr_t n);
STC_API _m_iter     _c_MEMB(_insert_uninit)(i_type* self, intptr_t idx, intptr_t n);
STC_API void        _c_MEMB(_erase_n)(i_type* self, intptr_t idx, intptr_t n);

STC_INLINE const _m_value*
_c_MEMB(_at)(const i_type* self, intptr_t idx)
    { return self->cbuf + _cbuf_topos(self, idx); }

STC_INLINE _m_value*
_c_MEMB(_at_mut)(i_type* self, intptr_t idx)
    { return self->cbuf + _cbuf_topos(self, idx); }

STC_INLINE _m_value*
_c_MEMB(_push_back)(i_type* self, _m_value val)
    { return  _c_MEMB(_push)(self, val); }

STC_INLINE void
_c_MEMB(_pop_back)(i_type* self) {
    c_assert(!_c_MEMB(_empty)(self));
    self->end = (self->end - 1) & self->capmask;
    i_keydrop((self->cbuf + self->end));
}

STC_INLINE _m_value _c_MEMB(_pull_back)(i_type* self) { // move back out of deq
    c_assert(!_c_MEMB(_empty)(self));
    self->end = (self->end - 1) & self->capmask;
    return self->cbuf[self->end];
}

STC_INLINE _m_iter
_c_MEMB(_insert_at)(i_type* self, _m_iter it, const _m_value val) {
    intptr_t idx = _cbuf_toidx(self, it.pos);
    return _c_MEMB(_insert_n)(self, idx, &val, 1);
}

STC_INLINE _m_iter
_c_MEMB(_erase_at)(i_type* self, _m_iter it) {
    _c_MEMB(_erase_n)(self, _cbuf_toidx(self, it.pos), 1);
    if (it.pos == self->end) it.ref = NULL;
    return it;
}

STC_INLINE _m_iter
_c_MEMB(_erase_range)(i_type* self, _m_iter it1, _m_iter it2) {
    intptr_t idx1 = _cbuf_toidx(self, it1.pos);
    intptr_t idx2 = _cbuf_toidx(self, it2.pos);
    _c_MEMB(_erase_n)(self, idx1, idx2 - idx1);
    if (it1.pos == self->end) it1.ref = NULL;
    return it1;
}

#if !defined i_no_emplace
STC_API _m_iter
_c_MEMB(_emplace_n)(i_type* self, intptr_t idx, const _m_raw* raw, intptr_t n);

STC_INLINE _m_value*
_c_MEMB(_emplace_front)(i_type* self, const _m_raw raw)
    { return _c_MEMB(_push_front)(self, i_keyfrom(raw)); }

STC_INLINE _m_value*
_c_MEMB(_emplace_back)(i_type* self, const _m_raw raw)
    { return _c_MEMB(_push)(self, i_keyfrom(raw)); }

STC_INLINE _m_iter
_c_MEMB(_emplace_at)(i_type* self, _m_iter it, const _m_raw raw)
    { return _c_MEMB(_insert_at)(self, it, i_keyfrom(raw)); }
#endif

#if defined _i_has_eq
STC_API _m_iter _c_MEMB(_find_in)(_m_iter p1, _m_iter p2, _m_raw raw);

STC_INLINE _m_iter
_c_MEMB(_find)(const i_type* self, _m_raw raw) {
    return _c_MEMB(_find_in)(_c_MEMB(_begin)(self), _c_MEMB(_end)(self), raw);
}

STC_INLINE const _m_value*
_c_MEMB(_get)(const i_type* self, _m_raw raw) {
    return _c_MEMB(_find_in)(_c_MEMB(_begin)(self), _c_MEMB(_end)(self), raw).ref;
}

STC_INLINE _m_value*
_c_MEMB(_get_mut)(i_type* self, _m_raw raw)
    { return (_m_value *) _c_MEMB(_get)(self, raw); }
#endif

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined(i_implement) || defined(i_static)

#include "priv/queue_prv.c"

STC_DEF _m_value*
_c_MEMB(_push_front)(i_type* self, _m_value value) {
    intptr_t start = (self->start - 1) & self->capmask;
    if (start == self->end) { // full
        _c_MEMB(_reserve)(self, self->capmask + 3); // => 2x expand
        start = (self->start - 1) & self->capmask;
    }
    _m_value *v = self->cbuf + start;
    self->start = start;
    *v = value;
    return v;
}

STC_DEF void
_c_MEMB(_erase_n)(i_type* self, const intptr_t idx, const intptr_t n) {
    const intptr_t len = _c_MEMB(_size)(self);
    for (intptr_t i = idx + n - 1; i >= idx; --i)
        i_keydrop(_c_MEMB(_at_mut)(self, i));
    for (intptr_t i = idx, j = i + n; j < len; ++i, ++j)
        *_c_MEMB(_at_mut)(self, i) = *_c_MEMB(_at)(self, j);
    self->end = (self->end - n) & self->capmask;
}

STC_DEF _m_iter
_c_MEMB(_insert_uninit)(i_type* self, const intptr_t idx, const intptr_t n) {
    const intptr_t len = _c_MEMB(_size)(self);
    _m_iter it = {._s=self};
    if (len + n > self->capmask)
        if (!_c_MEMB(_reserve)(self, len + n + 3)) // minimum 2x expand
            return it;
    it.pos = _cbuf_topos(self, idx);
    it.ref = self->cbuf + it.pos;
    self->end = (self->end + n) & self->capmask;

    if (it.pos < self->end) // common case because of reserve policy
        c_memmove(it.ref + n, it.ref, (len - idx)*c_sizeof *it.ref);
    else for (intptr_t i = len - 1, j = i + n; i >= idx; --i, --j)
        *_c_MEMB(_at_mut)(self, j) = *_c_MEMB(_at)(self, i);
    return it;
}

STC_DEF _m_iter
_c_MEMB(_insert_n)(i_type* self, const intptr_t idx, const _m_value* arr, const intptr_t n) {
    _m_iter it = _c_MEMB(_insert_uninit)(self, idx, n);
    for (intptr_t i = idx, j = 0; j < n; ++i, ++j)
        *_c_MEMB(_at_mut)(self, i) = arr[j];
    return it;
}

#if !defined i_no_emplace
STC_DEF _m_iter
_c_MEMB(_emplace_n)(i_type* self, const intptr_t idx, const _m_raw* raw, const intptr_t n) {
    _m_iter it = _c_MEMB(_insert_uninit)(self, idx, n);
    for (intptr_t i = idx, j = 0; j < n; ++i, ++j)
        *_c_MEMB(_at_mut)(self, i) = i_keyfrom(raw[j]);
    return it;
}
#endif

#if defined _i_has_eq
STC_DEF _m_iter
_c_MEMB(_find_in)(_m_iter i1, _m_iter i2, _m_raw raw) {
    for (; i1.pos != i2.pos; _c_MEMB(_next)(&i1)) {
        const _m_raw r = i_keyto(i1.ref);
        if (i_eq((&raw), (&r)))
            break;
    }
    return i1;
}
#endif
#endif // IMPLEMENTATION
#include "priv/template2.h"
#include "priv/linkage2.h"
