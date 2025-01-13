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

// IWYU pragma: private
#ifndef i_declared
_c_DEFTYPES(_c_deque_types, Self, i_key);
#endif
typedef i_keyraw _m_raw;

STC_API Self            _c_MEMB(_with_capacity)(const isize cap);
STC_API bool            _c_MEMB(_reserve)(Self* self, const isize cap);
STC_API void            _c_MEMB(_clear)(Self* self);
STC_API void            _c_MEMB(_drop)(const Self* cself);
STC_API _m_value*       _c_MEMB(_push)(Self* self, _m_value value); // push_back
STC_API void            _c_MEMB(_shrink_to_fit)(Self *self);
STC_API _m_iter         _c_MEMB(_advance)(_m_iter it, isize n);

#define _cbuf_toidx(self, pos) (((pos) - (self)->start) & (self)->capmask)
#define _cbuf_topos(self, idx) (((self)->start + (idx)) & (self)->capmask)

STC_INLINE Self         _c_MEMB(_init)(void)
                            { Self cx = {0}; return cx; }

STC_INLINE void         _c_MEMB(_put_n)(Self* self, const _m_raw* raw, isize n)
                            { while (n--) _c_MEMB(_push)(self, i_keyfrom((*raw))), ++raw; }

STC_INLINE Self         _c_MEMB(_with_n)(const _m_raw* raw, isize n)
                            { Self cx = {0}; _c_MEMB(_put_n)(&cx, raw, n); return cx; }

STC_INLINE void         _c_MEMB(_value_drop)(_m_value* val) { i_keydrop(val); }

#if !defined i_no_emplace
STC_INLINE _m_value*    _c_MEMB(_emplace)(Self* self, _m_raw raw)
                            { return _c_MEMB(_push)(self, i_keyfrom(raw)); }
#endif

#if defined _i_has_eq
STC_API bool            _c_MEMB(_eq)(const Self* self, const Self* other);
#endif

#if !defined i_no_clone
STC_API Self            _c_MEMB(_clone)(Self cx);
STC_INLINE _m_value     _c_MEMB(_value_clone)(_m_value val)
                            { return i_keyclone(val); }

STC_INLINE void         _c_MEMB(_copy)(Self* self, const Self other) {
                            if (self->cbuf == other.cbuf) return;
                            _c_MEMB(_drop)(self);
                            *self = _c_MEMB(_clone)(other);
                        }
#endif // !i_no_clone
STC_INLINE isize        _c_MEMB(_size)(const Self* self)
                            { return _cbuf_toidx(self, self->end); }
STC_INLINE isize        _c_MEMB(_capacity)(const Self* self)
                            { return self->capmask; }
STC_INLINE bool         _c_MEMB(_is_empty)(const Self* self)
                            { return self->start == self->end; }
STC_INLINE _m_raw       _c_MEMB(_value_toraw)(const _m_value* pval)
                            { return i_keytoraw(pval); }

STC_INLINE const _m_value* _c_MEMB(_front)(const Self* self)
                            { return self->cbuf + self->start; }
STC_INLINE _m_value*    _c_MEMB(_front_mut)(Self* self)
                            { return self->cbuf + self->start; }

STC_INLINE const _m_value* _c_MEMB(_back)(const Self* self)
                            { return self->cbuf + ((self->end - 1) & self->capmask); }
STC_INLINE _m_value*    _c_MEMB(_back_mut)(Self* self)
                            { return (_m_value*)_c_MEMB(_back)(self); }

STC_INLINE Self _c_MEMB(_move)(Self *self) {
    Self m = *self;
    memset(self, 0, sizeof *self);
    return m;
}

STC_INLINE void _c_MEMB(_take)(Self *self, Self unowned) {
    _c_MEMB(_drop)(self);
    *self = unowned;
}

STC_INLINE void _c_MEMB(_pop)(Self* self) { // pop_front
    c_assert(!_c_MEMB(_is_empty)(self));
    i_keydrop((self->cbuf + self->start));
    self->start = (self->start + 1) & self->capmask;
}

STC_INLINE _m_value _c_MEMB(_pull)(Self* self) { // move front out of queue
    c_assert(!_c_MEMB(_is_empty)(self));
    isize s = self->start;
    self->start = (s + 1) & self->capmask;
    return self->cbuf[s];
}

STC_INLINE _m_iter _c_MEMB(_begin)(const Self* self) {
    return c_literal(_m_iter){
        .ref=_c_MEMB(_is_empty)(self) ? NULL : self->cbuf + self->start,
        .pos=self->start, ._s=self};
}

STC_INLINE _m_iter _c_MEMB(_rbegin)(const Self* self) {
    isize pos = (self->end - 1) & self->capmask;
    return c_literal(_m_iter){
        .ref=_c_MEMB(_is_empty)(self) ? NULL : self->cbuf + pos,
        .pos=pos, ._s=self};
}

STC_INLINE _m_iter _c_MEMB(_end)(const Self* self)
    { (void)self; return c_literal(_m_iter){0}; }

STC_INLINE _m_iter _c_MEMB(_rend)(const Self* self)
    { (void)self; return c_literal(_m_iter){0}; }

STC_INLINE void _c_MEMB(_next)(_m_iter* it) {
    if (it->pos != it->_s->capmask) { ++it->ref; ++it->pos; }
    else { it->ref -= it->pos; it->pos = 0; }
    if (it->pos == it->_s->end) it->ref = NULL;
}

STC_INLINE void _c_MEMB(_rnext)(_m_iter* it) {
    if (it->pos == it->_s->start) it->ref = NULL;
    else if (it->pos != 0) { --it->ref; --it->pos; }
    else it->ref += (it->pos = it->_s->capmask);
}

STC_INLINE isize _c_MEMB(_index)(const Self* self, _m_iter it)
    { return _cbuf_toidx(self, it.pos); }

STC_INLINE void _c_MEMB(_adjust_end_)(Self* self, isize n)
    { self->end = (self->end + n) & self->capmask; }

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined i_implement

STC_DEF _m_iter _c_MEMB(_advance)(_m_iter it, isize n) {
    isize len = _c_MEMB(_size)(it._s);
    isize pos = it.pos, idx = _cbuf_toidx(it._s, pos);
    it.pos = (pos + n) & it._s->capmask;
    it.ref += it.pos - pos;
    if (!c_uless(idx + n, len)) it.ref = NULL;
    return it;
}

STC_DEF void
_c_MEMB(_clear)(Self* self) {
    for (c_each(i, Self, *self))
        { i_keydrop(i.ref); }
    self->start = 0, self->end = 0;
}

STC_DEF void
_c_MEMB(_drop)(const Self* cself) {
    Self* self = (Self*)cself;
    _c_MEMB(_clear)(self);
    i_free(self->cbuf, (self->capmask + 1)*c_sizeof(*self->cbuf));
}

STC_DEF Self
_c_MEMB(_with_capacity)(const isize cap) {
    Self cx = {0};
    _c_MEMB(_reserve)(&cx, cap);
    return cx;
}

STC_DEF bool
_c_MEMB(_reserve)(Self* self, const isize cap) {
    isize oldpow2 = self->capmask + 1, newpow2 = c_next_pow2(cap + 1);
    if (newpow2 <= oldpow2)
        return true;
    _m_value* d = (_m_value *)i_realloc(self->cbuf, oldpow2*c_sizeof *d, newpow2*c_sizeof *d);
    if (d == NULL)
        return false;
    isize head = oldpow2 - self->start;
    if (self->start <= self->end)
        ;
    else if (head < self->end) {
        self->start = newpow2 - head;
        c_memmove(d + self->start, d + oldpow2 - head, head*c_sizeof *d);
    } else {
        c_memmove(d + oldpow2, d, self->end*c_sizeof *d);
        self->end += oldpow2;
    }
    self->capmask = newpow2 - 1;
    self->cbuf = d;
    return true;
}

STC_DEF _m_value*
_c_MEMB(_push)(Self* self, _m_value value) { // push_back
    isize end = (self->end + 1) & self->capmask;
    if (end == self->start) { // full
        _c_MEMB(_reserve)(self, self->capmask + 3); // => 2x expand
        end = (self->end + 1) & self->capmask;
    }
    _m_value *v = self->cbuf + self->end;
    self->end = end;
    *v = value;
    return v;
}

STC_DEF void
_c_MEMB(_shrink_to_fit)(Self *self) {
    isize sz = _c_MEMB(_size)(self), j = 0;
    if (sz > self->capmask/2)
        return;
    Self out = _c_MEMB(_with_capacity)(sz);
    if (out.cbuf == NULL)
        return;
    for (c_each(i, Self, *self))
        out.cbuf[j++] = *i.ref;
    out.end = sz;
    i_free(self->cbuf, (self->capmask + 1)*c_sizeof(*self->cbuf));
    *self = out;
}

#if !defined i_no_clone
STC_DEF Self
_c_MEMB(_clone)(Self q) {
    isize sz = _c_MEMB(_size)(&q), j = 0;
    Self tmp = _c_MEMB(_with_capacity)(sz);
    if (tmp.cbuf)
        for (c_each(i, Self, q))
            tmp.cbuf[j++] = i_keyclone((*i.ref));
    q.cbuf = tmp.cbuf;
    q.capmask = tmp.capmask;
    q.start = 0;
    q.end = sz;
    return q;
}
#endif // i_no_clone

#if defined _i_has_eq
STC_DEF bool
_c_MEMB(_eq)(const Self* self, const Self* other) {
    if (_c_MEMB(_size)(self) != _c_MEMB(_size)(other)) return false;
    for (_m_iter i = _c_MEMB(_begin)(self), j = _c_MEMB(_begin)(other);
         i.ref; _c_MEMB(_next)(&i), _c_MEMB(_next)(&j))
    {
        const _m_raw _rx = i_keytoraw(i.ref), _ry = i_keytoraw(j.ref);
        if (!(i_eq((&_rx), (&_ry)))) return false;
    }
    return true;
}
#endif // _i_has_eq
#endif // IMPLEMENTATION
