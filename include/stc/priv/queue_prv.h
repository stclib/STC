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

#ifndef i_is_forward
_c_DEFTYPES(_c_deq_types, i_type, i_key);
#endif
typedef i_keyraw _m_raw;

STC_API i_type          _c_MEMB(_with_capacity)(const intptr_t n);
STC_API bool            _c_MEMB(_reserve)(i_type* self, const intptr_t n);
STC_API void            _c_MEMB(_clear)(i_type* self);
STC_API void            _c_MEMB(_drop)(const i_type* cself);
STC_API _m_value*       _c_MEMB(_push)(i_type* self, _m_value value); // push_back
STC_API void            _c_MEMB(_shrink_to_fit)(i_type *self);
STC_API _m_iter         _c_MEMB(_advance)(_m_iter it, intptr_t n);

#define _cbuf_toidx(self, pos) (((pos) - (self)->start) & (self)->capmask)
#define _cbuf_topos(self, idx) (((self)->start + (idx)) & (self)->capmask)

STC_INLINE i_type       _c_MEMB(_init)(void)
                            { i_type cx = {0}; return cx; }
STC_INLINE void         _c_MEMB(_put_n)(i_type* self, const _m_raw* raw, intptr_t n)
                            { while (n--) _c_MEMB(_push)(self, i_keyfrom(*raw++)); }
STC_INLINE i_type       _c_MEMB(_from_n)(const _m_raw* raw, intptr_t n)
                            { i_type cx = {0}; _c_MEMB(_put_n)(&cx, raw, n); return cx; }
STC_INLINE void         _c_MEMB(_value_drop)(_m_value* val) { i_keydrop(val); }

#if !defined i_no_emplace
STC_INLINE _m_value*    _c_MEMB(_emplace)(i_type* self, _m_raw raw)
                            { return _c_MEMB(_push)(self, i_keyfrom(raw)); }
#endif

#if defined _i_has_eq
STC_API bool            _c_MEMB(_eq)(const i_type* self, const i_type* other);
#endif

#if !defined i_no_clone
STC_API i_type          _c_MEMB(_clone)(i_type cx);
STC_INLINE _m_value        _c_MEMB(_value_clone)(_m_value val)
                            { return i_keyclone(val); }
STC_INLINE void         _c_MEMB(_copy)(i_type* self, const i_type* other) {
                            if (self->cbuf == other->cbuf) return;
                            _c_MEMB(_drop)(self);
                            *self = _c_MEMB(_clone)(*other);
                        }
#endif // !i_no_clone
STC_INLINE intptr_t     _c_MEMB(_size)(const i_type* self)
                            { return _cbuf_toidx(self, self->end); }
STC_INLINE intptr_t     _c_MEMB(_capacity)(const i_type* self)
                            { return self->capmask; }
STC_INLINE bool         _c_MEMB(_empty)(const i_type* self)
                            { return self->start == self->end; }
STC_INLINE _m_raw       _c_MEMB(_value_toraw)(const _m_value* pval)
                            { return i_keyto(pval); }

STC_INLINE _m_value*    _c_MEMB(_front)(const i_type* self)
                            { return self->cbuf + self->start; }

STC_INLINE _m_value*    _c_MEMB(_back)(const i_type* self)
                            { return self->cbuf + ((self->end - 1) & self->capmask); }

STC_INLINE void _c_MEMB(_pop)(i_type* self) { // pop_front
    c_assert(!_c_MEMB(_empty)(self));
    i_keydrop((self->cbuf + self->start));
    self->start = (self->start + 1) & self->capmask;
}

STC_INLINE _m_value _c_MEMB(_pull)(i_type* self) { // move front out of queue
    c_assert(!_c_MEMB(_empty)(self));
    intptr_t s = self->start;
    self->start = (s + 1) & self->capmask;
    return self->cbuf[s];
}

STC_INLINE _m_iter _c_MEMB(_begin)(const i_type* self) {
    return c_LITERAL(_m_iter){
        .ref=_c_MEMB(_empty)(self) ? NULL : self->cbuf + self->start,
        .pos=self->start, ._s=self
    };
}

STC_INLINE _m_iter _c_MEMB(_end)(const i_type* self)
    { return c_LITERAL(_m_iter){.pos=self->end, ._s=self}; }

STC_INLINE void _c_MEMB(_next)(_m_iter* it) {
    if (it->pos != it->_s->capmask) { ++it->ref; ++it->pos; }
    else { it->ref -= it->pos; it->pos = 0; }
    if (it->pos == it->_s->end) it->ref = NULL;
}

STC_INLINE intptr_t _c_MEMB(_index)(const i_type* self, _m_iter it)
    { return _cbuf_toidx(self, it.pos); }

STC_INLINE void _c_MEMB(_adjust_end_)(i_type* self, intptr_t n)
    { self->end = (self->end + n) & self->capmask; }
