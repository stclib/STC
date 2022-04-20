/* MIT License
 *
 * Copyright (c) 2022 Tyge Løvset, NORCE, www.norceresearch.no
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

#ifndef CSTACK_H_INCLUDED
#define CSTACK_H_INCLUDED
#include <stdlib.h>
#include "forward.h"
#endif // CSTACK_H_INCLUDED

#ifndef _i_prefix
#define _i_prefix cstack_
#endif
#include "template.h"

#if !c_option(c_is_fwd)
_cx_deftypes(_c_cstack_types, _cx_self, i_val);
#endif
typedef i_valraw _cx_raw;

STC_INLINE _cx_self _cx_memb(_init)(void)
    { return c_make(_cx_self){0, 0, 0}; }

STC_INLINE _cx_self _cx_memb(_with_capacity)(size_t cap) {
    _cx_self out = {(_cx_value *) c_malloc(cap*sizeof(i_val)), 0, cap};
    return out;
}

STC_INLINE _cx_self _cx_memb(_with_size)(size_t size, i_val null) {
    _cx_self out = {(_cx_value *) c_malloc(size*sizeof null), size, size};
    while (size) out.data[--size] = null;
    return out;
}

STC_INLINE void _cx_memb(_clear)(_cx_self* self) {
    _cx_value *p = self->data + self->size;
    while (p-- != self->data) { i_valdrop(p); }
    self->size = 0;
}

STC_INLINE void _cx_memb(_drop)(_cx_self* self)
    { _cx_memb(_clear)(self); c_free(self->data); }

STC_INLINE size_t _cx_memb(_size)(_cx_self v)
    { return v.size; }

STC_INLINE bool _cx_memb(_empty)(_cx_self v)
    { return !v.size; }

STC_INLINE size_t _cx_memb(_capacity)(_cx_self v)
    { return v.capacity; }

STC_INLINE bool _cx_memb(_reserve)(_cx_self* self, size_t n) {
    if (n < self->size) return true;
    _cx_value *t = (_cx_value *)c_realloc(self->data, n*sizeof *t);
    return t ? (self->data = t, self->capacity = n) : 0;
}

STC_INLINE void _cx_memb(_shrink_to_fit)(_cx_self* self)
    { _cx_memb(_reserve)(self, self->size); }

STC_INLINE _cx_value* _cx_memb(_top)(const _cx_self* self)
    { return &self->data[self->size - 1]; }

STC_INLINE _cx_value* _cx_memb(_push)(_cx_self* self, _cx_value val) {
    if (self->size == self->capacity)
        _cx_memb(_reserve)(self, self->size*3/2 + 4);
    _cx_value* vp = self->data + self->size++; 
    *vp = val; return vp;
}
STC_INLINE _cx_value* _cx_memb(_push_back)(_cx_self* self, _cx_value val)
    { return _cx_memb(_push)(self, val); }

STC_INLINE void _cx_memb(_pop)(_cx_self* self)
    { _cx_value* p = &self->data[--self->size]; i_valdrop(p); }
STC_INLINE void _cx_memb(_pop_back)(_cx_self* self)
    { _cx_memb(_pop)(self); }

STC_INLINE const _cx_value* _cx_memb(_at)(const _cx_self* self, size_t idx)
    { assert(idx < self->size); return self->data + idx; }
STC_INLINE _cx_value* _cx_memb(_at_mut)(_cx_self* self, size_t idx)
    { assert(idx < self->size); return self->data + idx; }

#if !defined _i_no_clone
#if !defined _i_no_emplace
STC_INLINE _cx_value* _cx_memb(_emplace)(_cx_self* self, _cx_raw raw)
    { return _cx_memb(_push)(self, i_valfrom(raw)); }
STC_INLINE _cx_value* _cx_memb(_emplace_back)(_cx_self* self, _cx_raw raw)
    { return _cx_memb(_push)(self,  i_valfrom(raw)); }
#endif // !_i_no_emplace

STC_INLINE _cx_self _cx_memb(_clone)(_cx_self v) {
    _cx_self out = {(_cx_value *) c_malloc(v.size*sizeof(_cx_value)), v.size, v.size};
    for (size_t i = 0; i < v.size; ++v.data)
        out.data[i++] = i_valclone((*v.data));
    return out;
}

STC_INLINE void _cx_memb(_copy)(_cx_self *self, _cx_self other) {
    if (self->data == other.data) return;
    _cx_memb(_drop)(self); *self = _cx_memb(_clone)(other);
}

STC_INLINE i_val _cx_memb(_value_clone)(_cx_value val)
    { return i_valclone(val); }

STC_INLINE i_valraw _cx_memb(_value_toraw)(_cx_value* val)
    { return i_valto(val); }
#endif // !_i_no_clone

STC_INLINE _cx_iter _cx_memb(_begin)(const _cx_self* self)
    { return c_make(_cx_iter){self->data}; }
STC_INLINE _cx_iter _cx_memb(_end)(const _cx_self* self)
    { return c_make(_cx_iter){self->data + self->size}; }
STC_INLINE void _cx_memb(_next)(_cx_iter* it) { ++it->ref; }
STC_INLINE _cx_iter _cx_memb(_advance)(_cx_iter it, intptr_t offs)
    { it.ref += offs; return it; }

#include "template.h"
