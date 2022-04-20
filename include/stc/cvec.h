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

/*
#include <stc/cstr.h>
#include <stc/forward.h>

forward_cvec(cvec_i32, int);

struct MyStruct {
    cvec_i32 int_vec;
    cstr name;
} typedef MyStruct;

#define i_val float
#include <stc/cvec.h>

#define i_val_str // special for cstr
#include <stc/cvec.h>

#define i_val int
#define i_opt c_is_fwd // forward declared
#define i_tag i32
#include <stc/cvec.h>

int main() {
    cvec_i32 vec = cvec_i32_init();
    cvec_i32_push_back(&vec, 123);
    cvec_i32_drop(&vec);

    cvec_float fvec = cvec_float_init();
    cvec_float_push_back(&fvec, 123.3);
    cvec_float_drop(&fvec);

    cvec_str svec = cvec_str_init();
    cvec_str_emplace_back(&svec, "Hello, friend");
    cvec_str_drop(&svec);
}
*/
#include "ccommon.h"

#ifndef CVEC_H_INCLUDED
#include "forward.h"
#include <stdlib.h>
#include <string.h>

struct cvec_rep { size_t size, cap; void* data[]; };
#define cvec_rep_(self) c_container_of((self)->data, struct cvec_rep, data)
#endif // CVEC_H_INCLUDED

#ifndef _i_prefix
#define _i_prefix cvec_
#endif
#include "template.h"

#if !c_option(c_is_fwd)
   _cx_deftypes(_c_cvec_types, _cx_self, i_val);
#endif
typedef i_valraw _cx_raw;
STC_API _cx_self        _cx_memb(_init)(void);
STC_API void            _cx_memb(_drop)(_cx_self* self);
STC_API void            _cx_memb(_clear)(_cx_self* self);
STC_API bool            _cx_memb(_reserve)(_cx_self* self, size_t cap);
STC_API bool            _cx_memb(_resize)(_cx_self* self, size_t size, i_val null);
STC_API _cx_value*      _cx_memb(_push)(_cx_self* self, i_val value);
STC_API _cx_iter        _cx_memb(_erase_range_p)(_cx_self* self, _cx_value* p1, _cx_value* p2);
STC_API _cx_iter        _cx_memb(_insert_range_p)(_cx_self* self, _cx_value* pos,
                                                 const _cx_value* p1, const _cx_value* p2);
#if !c_option(c_no_cmp)
STC_API int             _cx_memb(_value_cmp)(const _cx_value* x, const _cx_value* y);
STC_API _cx_iter        _cx_memb(_find_in)(_cx_iter it1, _cx_iter it2, i_valraw raw);
STC_API _cx_iter        _cx_memb(_bsearch_in)(_cx_iter it1, _cx_iter it2, i_valraw raw, _cx_iter* lower_bound);
#endif

#if !defined _i_no_clone
STC_API _cx_self        _cx_memb(_clone)(_cx_self cx);
STC_API _cx_iter        _cx_memb(_clone_range_p)(_cx_self* self, _cx_value* pos,
                                                 const _cx_value* p1, const _cx_value* p2);
STC_INLINE i_val        _cx_memb(_value_clone)(_cx_value val)
                            { return i_valclone(val); }
STC_INLINE i_val        _cx_memb(_value_fromraw)(i_valraw raw) { return i_valfrom(raw); }
STC_INLINE void         _cx_memb(_copy)(_cx_self *self, _cx_self other) {
                            if (self->data == other.data) return;
                            _cx_memb(_drop)(self);
                            *self = _cx_memb(_clone)(other);
                        }
#if !defined _i_no_emplace
STC_API _cx_iter        _cx_memb(_emplace_range_p)(_cx_self* self, _cx_value* pos,
                                                   const _cx_raw* p1, const _cx_raw* p2);
STC_INLINE _cx_value*   _cx_memb(_emplace)(_cx_self* self, i_valraw raw)
                            { return _cx_memb(_push)(self, i_valfrom(raw)); }
STC_INLINE _cx_value*   _cx_memb(_emplace_back)(_cx_self* self, i_valraw raw)
                            { return _cx_memb(_push)(self, i_valfrom(raw)); }
STC_INLINE _cx_iter
_cx_memb(_emplace_n)(_cx_self* self, const size_t idx, const _cx_raw arr[], const size_t n) {
    return _cx_memb(_emplace_range_p)(self, self->data + idx, arr, arr + n);
}
STC_INLINE _cx_iter
_cx_memb(_emplace_at)(_cx_self* self, _cx_iter it, i_valraw raw) {
    return _cx_memb(_emplace_range_p)(self, it.ref, &raw, &raw + 1);
}
STC_INLINE _cx_iter
_cx_memb(_emplace_range)(_cx_self* self, _cx_iter it, _cx_iter it1, _cx_iter it2) {
    return _cx_memb(_clone_range_p)(self, it.ref, it1.ref, it2.ref);
}
#endif // !_i_no_emplace
#endif // !_i_no_clone

STC_INLINE size_t       _cx_memb(_size)(_cx_self cx) { return cvec_rep_(&cx)->size; }
STC_INLINE size_t       _cx_memb(_capacity)(_cx_self cx) { return cvec_rep_(&cx)->cap; }
STC_INLINE bool         _cx_memb(_empty)(_cx_self cx) { return !cvec_rep_(&cx)->size; }
STC_INLINE i_valraw     _cx_memb(_value_toraw)(_cx_value* val) { return i_valto(val); }
STC_INLINE void         _cx_memb(_swap)(_cx_self* a, _cx_self* b) { c_swap(_cx_self, *a, *b); }
STC_INLINE _cx_value*   _cx_memb(_front)(const _cx_self* self) { return self->data; }
STC_INLINE _cx_value*   _cx_memb(_back)(const _cx_self* self)
                            { return self->data + cvec_rep_(self)->size - 1; }
STC_INLINE void         _cx_memb(_pop)(_cx_self* self)
                            { _cx_value* p = &self->data[--cvec_rep_(self)->size]; i_valdrop(p); }
STC_INLINE _cx_value*   _cx_memb(_push_back)(_cx_self* self, i_val value)
                            { return _cx_memb(_push)(self, value); }
STC_INLINE void         _cx_memb(_pop_back)(_cx_self* self) { _cx_memb(_pop)(self); }
STC_INLINE _cx_iter     _cx_memb(_begin)(const _cx_self* self)
                            { return c_make(_cx_iter){self->data}; }
STC_INLINE _cx_iter     _cx_memb(_end)(const _cx_self* self)
                            { return c_make(_cx_iter){self->data + cvec_rep_(self)->size}; }
STC_INLINE void         _cx_memb(_next)(_cx_iter* it) { ++it->ref; }
STC_INLINE _cx_iter     _cx_memb(_advance)(_cx_iter it, intptr_t offs)
                            { it.ref += offs; return it; }
STC_INLINE size_t       _cx_memb(_index)(_cx_self cx, _cx_iter it) { return it.ref - cx.data; }

STC_INLINE _cx_self
_cx_memb(_with_size)(const size_t size, i_val null) {
    _cx_self cx = _cx_memb(_init)();
    _cx_memb(_resize)(&cx, size, null);
    return cx;
}

STC_INLINE _cx_self
_cx_memb(_with_capacity)(const size_t cap) {
    _cx_self cx = _cx_memb(_init)();
    _cx_memb(_reserve)(&cx, cap);
    return cx;
}

STC_INLINE void
_cx_memb(_shrink_to_fit)(_cx_self *self) {
    _cx_memb(_reserve)(self, _cx_memb(_size)(*self));
}

STC_INLINE _cx_iter
_cx_memb(_insert)(_cx_self* self, const size_t idx, i_val value) {
    return _cx_memb(_insert_range_p)(self, self->data + idx, &value, &value + 1);
}
STC_INLINE _cx_iter
_cx_memb(_insert_n)(_cx_self* self, const size_t idx, const _cx_value arr[], const size_t n) {
    return _cx_memb(_insert_range_p)(self, self->data + idx, arr, arr + n);
}
STC_INLINE _cx_iter
_cx_memb(_insert_at)(_cx_self* self, _cx_iter it, i_val value) {
    return _cx_memb(_insert_range_p)(self, it.ref, &value, &value + 1);
}

STC_INLINE _cx_iter
_cx_memb(_erase_n)(_cx_self* self, const size_t idx, const size_t n) {
    return _cx_memb(_erase_range_p)(self, self->data + idx, self->data + idx + n);
}
STC_INLINE _cx_iter
_cx_memb(_erase_at)(_cx_self* self, _cx_iter it) {
    return _cx_memb(_erase_range_p)(self, it.ref, it.ref + 1);
}
STC_INLINE _cx_iter
_cx_memb(_erase_range)(_cx_self* self, _cx_iter it1, _cx_iter it2) {
    return _cx_memb(_erase_range_p)(self, it1.ref, it2.ref);
}

STC_INLINE const _cx_value*
_cx_memb(_at)(const _cx_self* self, const size_t idx) {
    assert(idx < cvec_rep_(self)->size); return self->data + idx;
}
STC_INLINE _cx_value*
_cx_memb(_at_mut)(_cx_self* self, const size_t idx) {
    assert(idx < cvec_rep_(self)->size); return self->data + idx;
}

#if !c_option(c_no_cmp)

STC_INLINE _cx_iter
_cx_memb(_find)(const _cx_self* self, i_valraw raw) {
    return _cx_memb(_find_in)(_cx_memb(_begin)(self), _cx_memb(_end)(self), raw);
}

STC_INLINE const _cx_value*
_cx_memb(_get)(const _cx_self* self, i_valraw raw) {
    _cx_iter end = _cx_memb(_end)(self);
    _cx_value* val = _cx_memb(_find)(self, raw).ref;
    return val == end.ref ? NULL : val;
}

STC_INLINE _cx_value*
_cx_memb(_get_mut)(const _cx_self* self, i_valraw raw)
    { return (_cx_value*) _cx_memb(_get)(self, raw); }

STC_INLINE _cx_iter
_cx_memb(_bsearch)(const _cx_self* self, i_valraw raw) {
    _cx_iter lower;
    return _cx_memb(_bsearch_in)(_cx_memb(_begin)(self), _cx_memb(_end)(self), raw, &lower);
}

STC_INLINE _cx_iter
_cx_memb(_lower_bound)(const _cx_self* self, i_valraw raw) {
    _cx_iter lower;
    _cx_memb(_bsearch_in)(_cx_memb(_begin)(self), _cx_memb(_end)(self), raw, &lower);
    return lower;
}

STC_INLINE void
_cx_memb(_sort_range)(_cx_iter i1, _cx_iter i2,
                     int(*_cmp_)(const _cx_value*, const _cx_value*)) {
    qsort(i1.ref, i2.ref - i1.ref, sizeof(_cx_value), (int(*)(const void*, const void*)) _cmp_);
}
STC_INLINE void
_cx_memb(_sort)(_cx_self* self) {
    _cx_memb(_sort_range)(_cx_memb(_begin)(self), _cx_memb(_end)(self), _cx_memb(_value_cmp));
}
#endif // !c_no_cmp
/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined(_i_implement)

#ifndef CVEC_H_INCLUDED
static struct cvec_rep _cvec_sentinel = {0, 0};
#endif

STC_DEF _cx_self
_cx_memb(_init)(void) {
    _cx_self cx = {(_cx_value *) _cvec_sentinel.data};
    return cx;
}

STC_DEF void
_cx_memb(_clear)(_cx_self* self) {
    struct cvec_rep* rep = cvec_rep_(self);
    if (rep->cap) {
        for (_cx_value *p = self->data, *q = p + rep->size; p != q; ) {
            --q; i_valdrop(q);
        }
        rep->size = 0;
    }
}

STC_DEF void
_cx_memb(_drop)(_cx_self* self) {
    struct cvec_rep* rep = cvec_rep_(self);
    // second test to supress gcc -O2 warn: -Wfree-nonheap-object
    if (rep->cap == 0 || rep == &_cvec_sentinel) return;
    _cx_memb(_clear)(self);
    c_free(rep);
}

STC_DEF bool
_cx_memb(_reserve)(_cx_self* self, const size_t cap) {
    struct cvec_rep* rep = cvec_rep_(self);
    const size_t len = rep->size;
    if (cap > rep->cap || (cap && cap == len)) {
        rep = (struct cvec_rep*) c_realloc(rep->cap ? rep : NULL,
                                           offsetof(struct cvec_rep, data) + cap*sizeof(i_val));
        if (!rep) return false;
        self->data = (_cx_value*) rep->data;
        rep->size = len;
        rep->cap = cap;
    }
    return true;
}

STC_DEF bool
_cx_memb(_resize)(_cx_self* self, const size_t len, i_val null) {
    if (!_cx_memb(_reserve)(self, len)) return false;
    struct cvec_rep *rep = cvec_rep_(self);
    const size_t n = rep->size;
    for (size_t i = len; i < n; ++i) { i_valdrop((self->data + i)); }
    for (size_t i = n; i < len; ++i) self->data[i] = null;
    if (rep->cap) rep->size = len;
    return true;
}

STC_DEF _cx_value*
_cx_memb(_push)(_cx_self* self, i_val value) {
    struct cvec_rep *r = cvec_rep_(self);
    if (r->size == r->cap) {
        _cx_memb(_reserve)(self, (r->size*3 >> 1) + 4);
        r = cvec_rep_(self);
    }
    _cx_value *v = self->data + r->size++;
    *v = value; return v;
}

static _cx_value*
_cx_memb(_insert_space_)(_cx_self* self, _cx_value* pos, const size_t len) {
    const size_t idx = pos - self->data;
    struct cvec_rep* r = cvec_rep_(self);
    if (!len) return pos;
    if (r->size + len > r->cap) {
        _cx_memb(_reserve)(self, (r->size*3 >> 1) + len);
        r = cvec_rep_(self);
        pos = self->data + idx;
    }
    memmove(pos + len, pos, (r->size - idx) * sizeof *pos);
    r->size += len;
    return pos;
}

STC_DEF _cx_iter
_cx_memb(_insert_range_p)(_cx_self* self, _cx_value* pos,
                          const _cx_value* p1, const _cx_value* p2) {
    pos = _cx_memb(_insert_space_)(self, pos, p2 - p1);
    _cx_iter it = {pos};
    memcpy(pos, p1, (p2 - p1)*sizeof *p1);
    return it;
}

STC_DEF _cx_iter
_cx_memb(_erase_range_p)(_cx_self* self, _cx_value* p1, _cx_value* p2) {
    intptr_t len = p2 - p1;
    if (len > 0) {
        _cx_value* p = p1, *end = self->data + cvec_rep_(self)->size;
        for (; p != p2; ++p) { i_valdrop(p); }
        memmove(p1, p2, (end - p2) * sizeof(i_val));
        cvec_rep_(self)->size -= len;
    }
    return c_make(_cx_iter){.ref = p1};
}

#if !defined _i_no_clone
STC_DEF _cx_self
_cx_memb(_clone)(_cx_self cx) {
    const size_t len = cvec_rep_(&cx)->size;
    _cx_self out = _cx_memb(_with_capacity)(len);
    _cx_memb(_clone_range_p)(&out, out.data, cx.data, cx.data + len);
    return out;
}

STC_DEF _cx_iter
_cx_memb(_clone_range_p)(_cx_self* self, _cx_value* pos,
                         const _cx_value* p1, const _cx_value* p2) {
    pos = _cx_memb(_insert_space_)(self, pos, p2 - p1);
    _cx_iter it = {pos};
    for (; p1 != p2; ++p1)
        *pos++ = i_valclone((*p1));
    return it;
}

#if !defined _i_no_emplace
STC_DEF _cx_iter
_cx_memb(_emplace_range_p)(_cx_self* self, _cx_value* pos,
                           const _cx_raw* p1, const _cx_raw* p2) {
    pos = _cx_memb(_insert_space_)(self, pos, p2 - p1);
    _cx_iter it = {pos};
    for (; p1 != p2; ++p1) *pos++ = i_valfrom((*p1));
    return it;
}
#endif // !_i_no_emplace
#endif // !_i_no_clone

#if !c_option(c_no_cmp)
STC_DEF _cx_iter
_cx_memb(_find_in)(_cx_iter i1, _cx_iter i2, i_valraw raw) {
    for (; i1.ref != i2.ref; ++i1.ref) {
        i_valraw r = i_valto(i1.ref);
        if (i_eq((&raw), (&r))) return i1;
    }
    return i2;
}

STC_DEF _cx_iter
_cx_memb(_bsearch_in)(_cx_iter i1, _cx_iter i2, i_valraw raw, _cx_iter* lower_bound) {
    _cx_iter mid, last = i2;
    while (i1.ref != i2.ref) {
        mid.ref = i1.ref + ((i2.ref - i1.ref) >> 1);
        int c; i_valraw m = i_valto(mid.ref);
        if (!(c = i_cmp((&raw), (&m)))) return *lower_bound = mid;
        else if (c < 0) i2.ref = mid.ref;
        else i1.ref = mid.ref + 1;
    }
    *lower_bound = i1;
    return last;
}

STC_DEF int
_cx_memb(_value_cmp)(const _cx_value* x, const _cx_value* y) {
    i_valraw rx = i_valto(x);
    i_valraw ry = i_valto(y);
    return i_cmp((&rx), (&ry));
}
#endif // !c_no_cmp
#endif // _i_implement
#define CVEC_H_INCLUDED
#include "template.h"
