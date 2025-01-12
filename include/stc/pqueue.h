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
#include "priv/linkage.h"

#ifndef STC_PQUEUE_H_INCLUDED
#define STC_PQUEUE_H_INCLUDED
#include "common.h"
#include "types.h"
#include <stdlib.h>
#endif // STC_PQUEUIE_H_INCLUDED

#ifndef _i_prefix
  #define _i_prefix pqueue_
#endif
#define _i_is_pqueue
#include "priv/template.h"
#ifndef i_declared
  _c_DEFTYPES(_c_vec_types, Self, i_key);
#endif
typedef i_keyraw _m_raw;

STC_API void        _c_MEMB(_make_heap)(Self* self);
STC_API void        _c_MEMB(_erase_at)(Self* self, isize idx);
STC_API _m_value*   _c_MEMB(_push)(Self* self, _m_value value);

STC_INLINE Self _c_MEMB(_init)(void)
    { return c_literal(Self){NULL}; }

STC_INLINE void _c_MEMB(_put_n)(Self* self, const _m_raw* raw, isize n)
    { while (n--) _c_MEMB(_push)(self, i_keyfrom(*raw++)); }

STC_INLINE Self _c_MEMB(_with_n)(const _m_raw* raw, isize n)
    { Self cx = {0}; _c_MEMB(_put_n)(&cx, raw, n); return cx; }

STC_INLINE bool _c_MEMB(_reserve)(Self* self, const isize cap) {
    if (cap != self->size && cap <= self->capacity) return true;
    _m_value *d = (_m_value *)i_realloc(self->data, self->capacity*c_sizeof *d, cap*c_sizeof *d);
    return d ? (self->data = d, self->capacity = cap, true) : false;
}

STC_INLINE void _c_MEMB(_shrink_to_fit)(Self* self)
    { _c_MEMB(_reserve)(self, self->size); }

STC_INLINE Self _c_MEMB(_with_capacity)(const isize cap) {
    Self out = {NULL}; _c_MEMB(_reserve)(&out, cap);
    return out;
}

STC_INLINE Self _c_MEMB(_with_size)(const isize size, _m_value null) {
    Self out = {NULL}; _c_MEMB(_reserve)(&out, size);
    while (out.size < size) out.data[out.size++] = null;
    return out;
}

STC_INLINE void _c_MEMB(_clear)(Self* self) {
    isize i = self->size; self->size = 0;
    while (i--) { i_keydrop((self->data + i)); }
}

STC_INLINE void _c_MEMB(_drop)(const Self* cself) {
    Self* self = (Self*)cself;
    _c_MEMB(_clear)(self);
    i_free(self->data, self->capacity*c_sizeof(*self->data));
}

STC_INLINE Self _c_MEMB(_move)(Self *self) {
    Self m = *self;
    memset(self, 0, sizeof *self);
    return m;
}

STC_INLINE void _c_MEMB(_take)(Self *self, Self unowned) {
    _c_MEMB(_drop)(self);
    *self = unowned;
}

STC_INLINE isize _c_MEMB(_size)(const Self* q)
    { return q->size; }

STC_INLINE bool _c_MEMB(_is_empty)(const Self* q)
    { return !q->size; }

STC_INLINE isize _c_MEMB(_capacity)(const Self* q)
    { return q->capacity; }

STC_INLINE const _m_value* _c_MEMB(_top)(const Self* self)
    { return &self->data[0]; }

STC_INLINE void _c_MEMB(_pop)(Self* self)
    { c_assert(!_c_MEMB(_is_empty)(self)); _c_MEMB(_erase_at)(self, 0); }

STC_INLINE _m_value _c_MEMB(_pull)(Self* self)
    { _m_value v = self->data[0]; _c_MEMB(_erase_at)(self, 0); return v; }

#if !defined i_no_clone
STC_API Self _c_MEMB(_clone)(Self q);

STC_INLINE void _c_MEMB(_copy)(Self *self, const Self other) {
    if (self->data == other.data) return;
    _c_MEMB(_drop)(self);
    *self = _c_MEMB(_clone)(other);
}
STC_INLINE _m_value _c_MEMB(_value_clone)(_m_value val)
    { return i_keyclone(val); }
#endif // !i_no_clone

#if !defined i_no_emplace
STC_INLINE void _c_MEMB(_emplace)(Self* self, _m_raw raw)
    { _c_MEMB(_push)(self, i_keyfrom(raw)); }
#endif // !i_no_emplace

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined i_implement

STC_DEF void
_c_MEMB(_sift_down_)(Self* self, const isize idx, const isize n) {
    _m_value t, *arr = self->data - 1;
    for (isize r = idx, c = idx*2; c <= n; c *= 2) {
        c += i_less((&arr[c]), (&arr[c + (c < n)]));
        if (!(i_less((&arr[r]), (&arr[c])))) return;
        t = arr[r], arr[r] = arr[c], arr[r = c] = t;
    }
}

STC_DEF void
_c_MEMB(_make_heap)(Self* self) {
    isize n = self->size;
    for (isize k = n/2; k != 0; --k)
        _c_MEMB(_sift_down_)(self, k, n);
}

#if !defined i_no_clone
STC_DEF Self _c_MEMB(_clone)(Self q) {
    Self tmp = _c_MEMB(_with_capacity)(q.size);
    for (; tmp.size < q.size; ++q.data)
        tmp.data[tmp.size++] = i_keyclone((*q.data));
    q.data = tmp.data;
    q.capacity = tmp.capacity;
    return q;
}
#endif

STC_DEF void
_c_MEMB(_erase_at)(Self* self, const isize idx) {
    i_keydrop((self->data + idx));
    const isize n = --self->size;
    self->data[idx] = self->data[n];
    _c_MEMB(_sift_down_)(self, idx + 1, n);
}

STC_DEF _m_value*
_c_MEMB(_push)(Self* self, _m_value value) {
    if (self->size == self->capacity)
        _c_MEMB(_reserve)(self, self->size*3/2 + 4);
    _m_value *arr = self->data - 1; /* base 1 */
    isize c = ++self->size;
    for (; c > 1 && (i_less((&arr[c/2]), (&value))); c /= 2)
        arr[c] = arr[c/2];
    arr[c] = value;
    return arr + c;
}

#endif
#undef _i_is_pqueue
#include "priv/linkage2.h"
#include "priv/template2.h"
