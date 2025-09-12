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
_c_DEFTYPES(_declare_queue, Self, i_key, _i_aux_def);
#endif
typedef i_keyraw _m_raw;

STC_API bool            _c_MEMB(_reserve)(Self* self, const isize cap);
STC_API void            _c_MEMB(_clear)(Self* self);
STC_API void            _c_MEMB(_drop)(const Self* cself);
STC_API _m_value*       _c_MEMB(_push)(Self* self, _m_value value); // push_back
STC_API void            _c_MEMB(_shrink_to_fit)(Self *self);
STC_API _m_iter         _c_MEMB(_advance)(_m_iter it, isize n);

#define _cbuf_toidx(self, pos) (((pos) - (self)->start) & (self)->capmask)
#define _cbuf_topos(self, idx) (((self)->start + (idx)) & (self)->capmask)

#ifndef _i_no_put
STC_INLINE void _c_MEMB(_put_n)(Self* self, const _m_raw* raw, isize n)
    { while (n--) _c_MEMB(_push)(self, i_keyfrom((*raw))), ++raw; }
#endif

STC_INLINE void _c_MEMB(_value_drop)(const Self* self, _m_value* val)
    { (void)self; i_keydrop(val); }

#ifndef _i_aux_alloc
    STC_INLINE Self _c_MEMB(_init)(void)
        { Self out = {0}; return out; }

    STC_INLINE Self _c_MEMB(_with_capacity)(isize cap) {
        cap = c_next_pow2(cap + 1);
        Self out = {_i_new_n(_m_value, cap), 0, 0, cap - 1};
        return out;
    }

    STC_INLINE Self _c_MEMB(_with_size_uninit)(isize size)
        { Self out = _c_MEMB(_with_capacity)(size); out.end = size; return out; }

    STC_INLINE Self _c_MEMB(_with_size)(isize size, _m_raw default_raw) {
        Self out = _c_MEMB(_with_capacity)(size);
        while (out.end < size) out.cbuf[out.end++] = i_keyfrom(default_raw);
        return out;
    }

    #ifndef _i_no_put
    STC_INLINE Self _c_MEMB(_from_n)(const _m_raw* raw, isize n) {
        Self out = _c_MEMB(_with_capacity)(n);
        _c_MEMB(_put_n)(&out, raw, n); return out;
    }
    #endif
#endif

#ifndef i_no_emplace
STC_INLINE _m_value* _c_MEMB(_emplace)(Self* self, _m_raw raw)
    { return _c_MEMB(_push)(self, i_keyfrom(raw)); }
#endif

#if defined _i_has_eq
STC_API bool _c_MEMB(_eq)(const Self* self, const Self* other);
#endif

#ifndef i_no_clone
STC_API Self _c_MEMB(_clone)(Self q);

STC_INLINE _m_value _c_MEMB(_value_clone)(const Self* self, _m_value val)
    { (void)self; return i_keyclone(val); }

STC_INLINE void _c_MEMB(_copy)(Self* self, const Self* other) {
    if (self == other) return;
    _c_MEMB(_drop)(self);
    *self = _c_MEMB(_clone)(*other);
}
#endif // !i_no_clone

STC_INLINE isize _c_MEMB(_size)(const Self* self)
    { return _cbuf_toidx(self, self->end); }

STC_INLINE isize _c_MEMB(_capacity)(const Self* self)
    { return self->capmask; }

STC_INLINE bool _c_MEMB(_is_empty)(const Self* self)
    { return self->start == self->end; }

STC_INLINE _m_raw _c_MEMB(_value_toraw)(const _m_value* pval)
    { return i_keytoraw(pval); }

STC_INLINE const _m_value* _c_MEMB(_front)(const Self* self)
    { return self->cbuf + self->start; }

STC_INLINE _m_value* _c_MEMB(_front_mut)(Self* self)
    { return self->cbuf + self->start; }

STC_INLINE const _m_value* _c_MEMB(_back)(const Self* self)
    { return self->cbuf + ((self->end - 1) & self->capmask); }

STC_INLINE _m_value* _c_MEMB(_back_mut)(Self* self)
    { return (_m_value*)_c_MEMB(_back)(self); }

STC_INLINE Self _c_MEMB(_move)(Self *self) {
    Self m = *self;
    self->capmask = self->start = self->end = 0;
    self->cbuf = NULL;
    return m;
}

STC_INLINE void _c_MEMB(_take)(Self *self, Self unowned)
    { _c_MEMB(_drop)(self); *self = unowned; }

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
    _i_free_n(self->cbuf, self->capmask + 1);
}

STC_DEF bool
_c_MEMB(_reserve)(Self* self, const isize cap) {
    isize oldpow2 = self->capmask + (self->capmask & 1); // handle capmask = 0
    isize newpow2 = c_next_pow2(cap + 1);
    if (newpow2 <= oldpow2)
        return self->cbuf != NULL;
    _m_value* d = (_m_value *)_i_realloc_n(self->cbuf, oldpow2, newpow2);
    if (d == NULL)
        return false;
    isize head = oldpow2 - self->start;
    if (self->start <= self->end)       // [..S########E....|................]
        ;
    else if (head < self->end) {        // [#######E.....S##|.............s!!]
        c_memcpy(d + newpow2 - head, d + self->start, head*c_sizeof *d);
        self->start = newpow2 - head;
    } else {                            // [##E.....S#######|!!e.............]
        c_memcpy(d + oldpow2, d, self->end*c_sizeof *d);
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
        if (!_c_MEMB(_reserve)(self, self->capmask + 3)) // => 2x expand
            return NULL;
        end = (self->end + 1) & self->capmask;
    }
    _m_value *v = self->cbuf + self->end;
    self->end = end;
    *v = value;
    return v;
}

STC_DEF void
_c_MEMB(_shrink_to_fit)(Self *self) {
    isize sz = _c_MEMB(_size)(self);
    isize newpow2 = c_next_pow2(sz + 1);
    if (newpow2 > self->capmask)
        return;
    if (self->start <= self->end) {
        c_memmove(self->cbuf, self->cbuf + self->start, sz*c_sizeof *self->cbuf);
        self->start = 0, self->end = sz;
    } else {
        isize n = self->capmask - self->start + 1;
        c_memmove(self->cbuf + (newpow2 - n), self->cbuf + self->start, n*c_sizeof *self->cbuf);
        self->start = newpow2 - n;
    }
    self->cbuf = (_m_value *)_i_realloc_n(self->cbuf, self->capmask + 1, newpow2);
    self->capmask = newpow2 - 1;
}

#ifndef i_no_clone
STC_DEF Self
_c_MEMB(_clone)(Self q) {
    Self out = q, *self = &out; (void)self; // may be used by _i_new_n/i_keyclone via i_aux.
    out.start = 0; out.end = _c_MEMB(_size)(&q);
    out.capmask = c_next_pow2(out.end + 1) - 1;
    out.cbuf = _i_new_n(_m_value, out.capmask + 1);
    isize i = 0;
    if (out.cbuf)
        for (c_each(it, Self, q))
            out.cbuf[i++] = i_keyclone((*it.ref));
    return out;
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
