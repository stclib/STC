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

#include "template.h"
#ifndef i_is_forward
_cx_DEFTYPES(_c_cdeq_types, _cx_Self, i_key);
#endif
typedef i_keyraw _cx_raw;

STC_API _cx_Self        _cx_MEMB(_with_capacity)(const intptr_t n);
STC_API bool            _cx_MEMB(_reserve)(_cx_Self* self, const intptr_t n);
STC_API void            _cx_MEMB(_clear)(_cx_Self* self);
STC_API void            _cx_MEMB(_drop)(_cx_Self* self);
STC_API _cx_value*      _cx_MEMB(_push)(_cx_Self* self, i_key value); // push_back
STC_API void            _cx_MEMB(_shrink_to_fit)(_cx_Self *self);
STC_API _cx_iter        _cx_MEMB(_advance)(_cx_iter it, intptr_t n);

#define _cdeq_toidx(self, pos) (((pos) - (self)->start) & (self)->capmask)
#define _cdeq_topos(self, idx) (((self)->start + (idx)) & (self)->capmask)

STC_INLINE _cx_Self     _cx_MEMB(_init)(void)
                            { _cx_Self cx = {0}; return cx; }
STC_INLINE void         _cx_MEMB(_put_n)(_cx_Self* self, const _cx_raw* raw, intptr_t n)
                            { while (n--) _cx_MEMB(_push)(self, i_keyfrom(*raw++)); }
STC_INLINE _cx_Self     _cx_MEMB(_from_n)(const _cx_raw* raw, intptr_t n)
                            { _cx_Self cx = {0}; _cx_MEMB(_put_n)(&cx, raw, n); return cx; }
STC_INLINE void         _cx_MEMB(_value_drop)(_cx_value* val) { i_keydrop(val); }

#if !defined i_no_emplace
STC_INLINE _cx_value*   _cx_MEMB(_emplace)(_cx_Self* self, _cx_raw raw)
                            { return _cx_MEMB(_push)(self, i_keyfrom(raw)); }
#endif

#if defined _i_has_eq || defined _i_has_cmp
STC_API bool            _cx_MEMB(_eq)(const _cx_Self* self, const _cx_Self* other);
#endif

#if !defined i_no_clone
STC_API _cx_Self        _cx_MEMB(_clone)(_cx_Self cx);
STC_INLINE i_key        _cx_MEMB(_value_clone)(i_key val)
                            { return i_keyclone(val); }
STC_INLINE void         _cx_MEMB(_copy)(_cx_Self* self, const _cx_Self* other) {
                            if (self->data == other->data) return;
                            _cx_MEMB(_drop)(self);
                            *self = _cx_MEMB(_clone)(*other);
                        }
#endif // !i_no_clone
STC_INLINE intptr_t     _cx_MEMB(_size)(const _cx_Self* self) 
                            { return _cdeq_toidx(self, self->end); }
STC_INLINE intptr_t     _cx_MEMB(_capacity)(const _cx_Self* self)
                            { return self->capmask; }
STC_INLINE bool         _cx_MEMB(_empty)(const _cx_Self* self)
                            { return self->start == self->end; }
STC_INLINE _cx_raw      _cx_MEMB(_value_toraw)(const _cx_value* pval)
                            { return i_keyto(pval); }

STC_INLINE _cx_value*   _cx_MEMB(_front)(const _cx_Self* self) 
                            { return self->data + self->start; }

STC_INLINE _cx_value*   _cx_MEMB(_back)(const _cx_Self* self)
                            { return self->data + ((self->end - 1) & self->capmask); }

STC_INLINE void _cx_MEMB(_pop)(_cx_Self* self) { // pop_front
    c_assert(!_cx_MEMB(_empty)(self));
    i_keydrop((self->data + self->start));
    self->start = (self->start + 1) & self->capmask;
}

STC_INLINE _cx_value _cx_MEMB(_pull)(_cx_Self* self) { // move front out of queue
    c_assert(!_cx_MEMB(_empty)(self));
    intptr_t s = self->start;
    self->start = (s + 1) & self->capmask;
    return self->data[s];
}

STC_INLINE _cx_iter _cx_MEMB(_begin)(const _cx_Self* self) {
    return c_LITERAL(_cx_iter){
        .ref=_cx_MEMB(_empty)(self) ? NULL : self->data + self->start,
        .pos=self->start, ._s=self
    };
}

STC_INLINE _cx_iter _cx_MEMB(_end)(const _cx_Self* self)
    { return c_LITERAL(_cx_iter){.pos=self->end, ._s=self}; }

STC_INLINE void _cx_MEMB(_next)(_cx_iter* it) {
    if (it->pos != it->_s->capmask) { ++it->ref; ++it->pos; }
    else { it->ref -= it->pos; it->pos = 0; }
    if (it->pos == it->_s->end) it->ref = NULL;
}

STC_INLINE intptr_t _cx_MEMB(_index)(const _cx_Self* self, _cx_iter it)
    { return _cdeq_toidx(self, it.pos); }

STC_INLINE void _cx_MEMB(_adjust_end_)(_cx_Self* self, intptr_t n)
    { self->end = (self->end + n) & self->capmask; }
