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

#ifndef CQUEUE_H_INCLUDED
#include "forward.h"
#include <stdlib.h>
#include <string.h>
#endif // CQUEUE_H_INCLUDED

#ifndef _i_prefix
#define _i_prefix cqueue_
#endif
#include "priv/template.h"

#ifndef i_is_forward
_cx_deftypes(_c_cdeq_types, _cx_self, i_key);
#endif
typedef i_keyraw _cx_raw;

STC_API _cx_self        _cx_memb(_with_capacity)(const intptr_t n);
STC_API bool            _cx_memb(_reserve)(_cx_self* self, const intptr_t n);
STC_API void            _cx_memb(_clear)(_cx_self* self);
STC_API void            _cx_memb(_drop)(_cx_self* self);
STC_API _cx_value*      _cx_memb(_push)(_cx_self* self, i_key value); // push_back
STC_API void            _cx_memb(_shrink_to_fit)(_cx_self *self);
STC_API _cx_iter        _cx_memb(_advance)(_cx_iter it, intptr_t n);

#define _cdeq_toidx(self, pos) (((pos) - (self)->start) & (self)->capmask)
#define _cdeq_topos(self, idx) (((self)->start + (idx)) & (self)->capmask)

STC_INLINE _cx_self     _cx_memb(_init)(void)
                            { _cx_self cx = {0}; return cx; }
STC_INLINE void         _cx_memb(_put_n)(_cx_self* self, const _cx_raw* raw, intptr_t n)
                            { while (n--) _cx_memb(_push)(self, i_keyfrom(*raw++)); }
STC_INLINE _cx_self     _cx_memb(_from_n)(const _cx_raw* raw, intptr_t n)
                            { _cx_self cx = {0}; _cx_memb(_put_n)(&cx, raw, n); return cx; }
STC_INLINE void         _cx_memb(_value_drop)(_cx_value* val) { i_keydrop(val); }

#if !defined i_no_emplace
STC_INLINE _cx_value*   _cx_memb(_emplace)(_cx_self* self, _cx_raw raw)
                            { return _cx_memb(_push)(self, i_keyfrom(raw)); }
#endif

#if !defined i_no_clone
STC_API _cx_self        _cx_memb(_clone)(_cx_self cx);
STC_INLINE i_key        _cx_memb(_value_clone)(i_key val)
                            { return i_keyclone(val); }
#endif // !i_no_clone
STC_INLINE intptr_t     _cx_memb(_size)(const _cx_self* self) 
                            { return _cdeq_toidx(self, self->end); }
STC_INLINE intptr_t     _cx_memb(_capacity)(const _cx_self* self)
                            { return self->capmask; }
STC_INLINE bool         _cx_memb(_empty)(const _cx_self* self)
                            { return self->start == self->end; }
STC_INLINE _cx_raw      _cx_memb(_value_toraw)(const _cx_value* pval)
                            { return i_keyto(pval); }

STC_INLINE _cx_value*
_cx_memb(_front)(const _cx_self* self) 
    { return self->data + self->start; }

STC_INLINE _cx_value*
_cx_memb(_back)(const _cx_self* self)
    { return self->data + ((self->end - 1) & self->capmask); }

STC_INLINE void
_cx_memb(_pop)(_cx_self* self) { // pop_front
    c_ASSERT(!_cx_memb(_empty)(self));
    i_keydrop((self->data + self->start));
    self->start = (self->start + 1) & self->capmask;
}

STC_INLINE void _cx_memb(_copy)(_cx_self* self, const _cx_self* other) {
    if (self->data == other->data) return;
    _cx_memb(_drop)(self);
    *self = _cx_memb(_clone)(*other);
}

STC_INLINE _cx_iter
_cx_memb(_begin)(const _cx_self* self) {
    return c_LITERAL(_cx_iter){
        _cx_memb(_empty)(self) ? NULL : self->data + self->start,
        self->start, self
    };
}

STC_INLINE _cx_iter
_cx_memb(_end)(const _cx_self* self)
    { return c_LITERAL(_cx_iter){NULL, self->end, self}; }

STC_INLINE void
_cx_memb(_next)(_cx_iter* it) {
    if (it->pos != it->_s->capmask) { ++it->ref; ++it->pos; }
    else { it->ref -= it->pos; it->pos = 0; }
    if (it->pos == it->_s->end) it->ref = NULL;
}

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined(i_implement)

STC_DEF _cx_iter _cx_memb(_advance)(_cx_iter it, intptr_t n) {
    intptr_t len = _cx_memb(_size)(it._s);
    intptr_t pos = it.pos, idx = _cdeq_toidx(it._s, pos);
    it.pos = (pos + n) & it._s->capmask; 
    it.ref += it.pos - pos;
    if (!c_LTu(idx + n, len)) it.ref = NULL;
    return it;
}

STC_DEF void
_cx_memb(_clear)(_cx_self* self) {
    c_foreach (i, _cx_self, *self)
        { i_keydrop(i.ref); }
    self->start = 0, self->end = 0;
}

STC_DEF void
_cx_memb(_drop)(_cx_self* self) {
    _cx_memb(_clear)(self);
    i_free(self->data);
}

STC_DEF _cx_self
_cx_memb(_with_capacity)(const intptr_t n) {
    _cx_self cx = {0};
    _cx_memb(_reserve)(&cx, n);
    return cx;
}

STC_DEF bool
_cx_memb(_reserve)(_cx_self* self, const intptr_t n) {
    if (n <= self->capmask)
        return true;
    intptr_t oldcap = self->capmask + 1, newcap = cnextpow2(n + 1);
    _cx_value* data = (_cx_value *)i_realloc(self->data, newcap*c_sizeof *self->data);
    if (!data)
        return false;
    intptr_t head = oldcap - self->start;
    if (self->start < self->end || self->start == 0)
        ;
    else if (head < self->end) {
        self->start = newcap - head;
        c_memmove(data + self->start, data + oldcap - head, head*c_sizeof *data);
    } else {
        c_memmove(data + oldcap, data, self->end*c_sizeof *data);
        self->end += oldcap;
    }
    self->capmask = newcap - 1;
    self->data = data;
    return true;
}

STC_DEF _cx_value*
_cx_memb(_push)(_cx_self* self, i_key value) { // push_back
    intptr_t end = (self->end + 1) & self->capmask;
    if (end == self->start) { // full
        _cx_memb(_reserve)(self, self->capmask + 3); // => 2x expand
        end = (self->end + 1) & self->capmask;
    }
    _cx_value *v = self->data + self->end;
    self->end = end;
    *v = value;
    return v;
}

STC_DEF void
_cx_memb(_shrink_to_fit)(_cx_self *self) {
    intptr_t sz = _cx_memb(_size)(self), j = 0;
    if (sz > self->capmask/2)
        return;
    _cx_self out = _cx_memb(_with_capacity)(sz);
    if (!out.data)
        return;
    c_foreach (i, _cx_self, *self)
        out.data[j++] = *i.ref;
    out.end = sz;
    i_free(self->data);
    *self = out;
}

#if !defined i_no_clone
STC_DEF _cx_self
_cx_memb(_clone)(_cx_self cx) {
    intptr_t sz = _cx_memb(_size)(&cx), j = 0;
    _cx_self out = _cx_memb(_with_capacity)(sz);
    if (out.data)
        c_foreach (i, _cx_self, cx)
            out.data[j++] = i_keyclone((*i.ref));
    out.end = sz;
    return out;
}

#endif // i_no_clone
#endif // IMPLEMENTATION
#include "priv/template2.h"
#define CQUEUE_H_INCLUDED
