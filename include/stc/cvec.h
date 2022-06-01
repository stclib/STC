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

#define i_key float
#include <stc/cvec.h>

#define i_key_str // special for cstr
#include <stc/cvec.h>

#define i_key int
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

struct cvec_rep { size_t size, cap; unsigned data[1]; };
#define cvec_rep_(self) c_unchecked_container_of((self)->data, struct cvec_rep, data)
#endif // CVEC_H_INCLUDED

#ifndef _i_prefix
#define _i_prefix cvec_
#endif
#include "template.h"

#if !c_option(c_is_fwd)
   _cx_deftypes(_c_cvec_types, _cx_self, i_key);
#endif
typedef i_keyraw _cx_raw;
STC_API _cx_self        _cx_memb(_init)(void);
STC_API void            _cx_memb(_drop)(_cx_self* self);
STC_API void            _cx_memb(_clear)(_cx_self* self);
STC_API bool            _cx_memb(_reserve)(_cx_self* self, size_t cap);
STC_API bool            _cx_memb(_resize)(_cx_self* self, size_t size, i_key null);
STC_API _cx_value*      _cx_memb(_push)(_cx_self* self, i_key value);
STC_API _cx_iter        _cx_memb(_erase_range_p)(_cx_self* self, _cx_value* p1, _cx_value* p2);
STC_API _cx_value*      _cx_memb(_insert_range_p)(_cx_self* self, _cx_value* pos,
                                                  const _cx_value* p1, const _cx_value* p2);
STC_API _cx_value*      _cx_memb(_expand_uninit_p)(_cx_self* self, _cx_value* pos, const size_t n);
#if !c_option(c_no_cmp)
STC_API int             _cx_memb(_value_cmp)(const _cx_value* x, const _cx_value* y);
STC_API _cx_iter        _cx_memb(_find_in)(_cx_iter it1, _cx_iter it2, _cx_raw raw);
STC_API _cx_iter        _cx_memb(_binary_search_in)(_cx_iter it1, _cx_iter it2, _cx_raw raw, _cx_iter* lower_bound);
#endif

#if !defined _i_no_clone
STC_API _cx_self        _cx_memb(_clone)(_cx_self cx);
STC_API _cx_value*      _cx_memb(_clone_range_p)(_cx_self* self, _cx_value* pos,
                                                 const _cx_value* p1, const _cx_value* p2);
STC_INLINE i_key        _cx_memb(_value_clone)(_cx_value val)
                            { return i_keyclone(val); }
STC_INLINE void         _cx_memb(_copy)(_cx_self *self, _cx_self other) {
                            if (self->data == other.data) return;
                            _cx_memb(_drop)(self);
                            *self = _cx_memb(_clone)(other);
                        }
#if !defined _i_no_emplace
STC_API _cx_value*      _cx_memb(_emplace_range_p)(_cx_self* self, _cx_value* pos,
                                                   const _cx_raw* p1, const _cx_raw* p2);
STC_INLINE _cx_value*   _cx_memb(_emplace)(_cx_self* self, _cx_raw raw)
                            { return _cx_memb(_push)(self, i_keyfrom(raw)); }
STC_INLINE _cx_value*   _cx_memb(_emplace_back)(_cx_self* self, _cx_raw raw)
                            { return _cx_memb(_push)(self, i_keyfrom(raw)); }
STC_INLINE _cx_value*
_cx_memb(_emplace_n)(_cx_self* self, const size_t idx, const _cx_raw arr[], const size_t n) {
    return _cx_memb(_emplace_range_p)(self, self->data + idx, arr, arr + n);
}
STC_INLINE _cx_value*
_cx_memb(_emplace_at)(_cx_self* self, _cx_iter it, _cx_raw raw) {
    return _cx_memb(_emplace_range_p)(self, it.ref, &raw, &raw + 1);
}
STC_INLINE _cx_value*
_cx_memb(_emplace_range)(_cx_self* self, _cx_iter it, _cx_iter it1, _cx_iter it2) {
    return _cx_memb(_clone_range_p)(self, it.ref, it1.ref, it2.ref);
}
#endif // !_i_no_emplace
#endif // !_i_no_clone

STC_INLINE size_t       _cx_memb(_size)(_cx_self cx) { return cvec_rep_(&cx)->size; }
STC_INLINE size_t       _cx_memb(_capacity)(_cx_self cx) { return cvec_rep_(&cx)->cap; }
STC_INLINE bool         _cx_memb(_empty)(_cx_self cx) { return !cvec_rep_(&cx)->size; }
STC_INLINE _cx_raw      _cx_memb(_value_toraw)(_cx_value* val) { return i_keyto(val); }
STC_INLINE void         _cx_memb(_swap)(_cx_self* a, _cx_self* b) { c_swap(_cx_self, *a, *b); }
STC_INLINE _cx_value*   _cx_memb(_front)(const _cx_self* self) { return self->data; }
STC_INLINE _cx_value*   _cx_memb(_back)(const _cx_self* self)
                            { return self->data + cvec_rep_(self)->size - 1; }
STC_INLINE void         _cx_memb(_pop)(_cx_self* self)
                            { _cx_value* p = &self->data[--cvec_rep_(self)->size]; i_keydrop(p); }
STC_INLINE _cx_value*   _cx_memb(_push_back)(_cx_self* self, i_key value)
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
_cx_memb(_with_size)(const size_t size, i_key null) {
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

STC_INLINE _cx_value*
_cx_memb(_expand_uninit)(_cx_self *self, const size_t n) {
    return _cx_memb(_expand_uninit_p)(self, self->data + _cx_memb(_size)(*self), n);
}

STC_INLINE _cx_value*
_cx_memb(_insert)(_cx_self* self, const size_t idx, i_key value) {
    return _cx_memb(_insert_range_p)(self, self->data + idx, &value, &value + 1);
}
STC_INLINE _cx_value*
_cx_memb(_insert_n)(_cx_self* self, const size_t idx, const _cx_value arr[], const size_t n) {
    return _cx_memb(_insert_range_p)(self, self->data + idx, arr, arr + n);
}
STC_INLINE _cx_value*
_cx_memb(_insert_at)(_cx_self* self, _cx_iter it, i_key value) {
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
_cx_memb(_find)(const _cx_self* self, _cx_raw raw) {
    return _cx_memb(_find_in)(_cx_memb(_begin)(self), _cx_memb(_end)(self), raw);
}

STC_INLINE const _cx_value*
_cx_memb(_get)(const _cx_self* self, _cx_raw raw) {
    _cx_iter end = _cx_memb(_end)(self);
    _cx_value* val = _cx_memb(_find)(self, raw).ref;
    return val == end.ref ? NULL : val;
}

STC_INLINE _cx_value*
_cx_memb(_get_mut)(const _cx_self* self, _cx_raw raw)
    { return (_cx_value*) _cx_memb(_get)(self, raw); }

STC_INLINE _cx_iter
_cx_memb(_binary_search)(const _cx_self* self, _cx_raw raw) {
    _cx_iter lower;
    return _cx_memb(_binary_search_in)(_cx_memb(_begin)(self), _cx_memb(_end)(self), raw, &lower);
}

STC_INLINE _cx_iter
_cx_memb(_lower_bound)(const _cx_self* self, _cx_raw raw) {
    _cx_iter lower;
    _cx_memb(_binary_search_in)(_cx_memb(_begin)(self), _cx_memb(_end)(self), raw, &lower);
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
#if defined(i_implement)

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
            --q; i_keydrop(q);
        }
        rep->size = 0;
    }
}

STC_DEF void
_cx_memb(_drop)(_cx_self* self) {
    struct cvec_rep* rep = cvec_rep_(self);
    // second test to supress gcc -O2 warn: -Wfree-nonheap-object
    if (rep->cap == 0 || rep == &_cvec_sentinel)
        return;
    _cx_memb(_clear)(self);
    c_free(rep);
}

STC_DEF bool
_cx_memb(_reserve)(_cx_self* self, const size_t cap) {
    struct cvec_rep* rep = cvec_rep_(self);
    const size_t len = rep->size;
    if (cap > rep->cap || (cap && cap == len)) {
        rep = (struct cvec_rep*) c_realloc(rep->cap ? rep : NULL,
                                           offsetof(struct cvec_rep, data) + cap*sizeof(i_key));
        if (!rep)
            return false;
        self->data = (_cx_value*) rep->data;
        rep->size = len;
        rep->cap = cap;
    }
    return true;
}

STC_DEF bool
_cx_memb(_resize)(_cx_self* self, const size_t len, i_key null) {
    if (!_cx_memb(_reserve)(self, len)) return false;
    struct cvec_rep *rep = cvec_rep_(self);
    const size_t n = rep->size;
    for (size_t i = len; i < n; ++i)
        { i_keydrop((self->data + i)); }
    for (size_t i = n; i < len; ++i)
        self->data[i] = null;
    if (rep->cap)
        rep->size = len;
    return true;
}

STC_DEF _cx_value*
_cx_memb(_push)(_cx_self* self, i_key value) {
    struct cvec_rep *r = cvec_rep_(self);
    if (r->size == r->cap) {
        if (!_cx_memb(_reserve)(self, (r->size*3 >> 1) + 4))
            return NULL;
        r = cvec_rep_(self);
    }
    _cx_value *v = self->data + r->size++;
    *v = value; return v;
}

STC_DEF _cx_value*
_cx_memb(_expand_uninit_p)(_cx_self* self, _cx_value* pos, const size_t n) {
    const size_t idx = pos - self->data;
    struct cvec_rep* r = cvec_rep_(self);
    if (!n)
        return pos;
    if (r->size + n > r->cap) {
        if (!_cx_memb(_reserve)(self, r->size*3/2 + n))
            return NULL;
        r = cvec_rep_(self);
        pos = self->data + idx;
    }
    memmove(pos + n, pos, (r->size - idx)*sizeof *pos);
    r->size += n;
    return pos;
}

STC_DEF _cx_value*
_cx_memb(_insert_range_p)(_cx_self* self, _cx_value* pos,
                          const _cx_value* p1, const _cx_value* p2) {
    pos = _cx_memb(_expand_uninit_p)(self, pos, p2 - p1);
    if (pos)
        memcpy(pos, p1, (p2 - p1)*sizeof *p1);
    return pos;
}

STC_DEF _cx_iter
_cx_memb(_erase_range_p)(_cx_self* self, _cx_value* p1, _cx_value* p2) {
    intptr_t len = p2 - p1;
    if (len > 0) {
        _cx_value* p = p1, *end = self->data + cvec_rep_(self)->size;
        for (; p != p2; ++p)
            { i_keydrop(p); }
        memmove(p1, p2, (end - p2) * sizeof(i_key));
        cvec_rep_(self)->size -= len;
    }
    return c_make(_cx_iter){.ref = p1};
}

#if !defined _i_no_clone
STC_DEF _cx_self
_cx_memb(_clone)(_cx_self cx) {
    const size_t len = cvec_rep_(&cx)->size;
    _cx_self out = _cx_memb(_with_capacity)(len);
    if (cvec_rep_(&out)->cap)
        _cx_memb(_clone_range_p)(&out, out.data, cx.data, cx.data + len);
    return out;
}

STC_DEF _cx_value*
_cx_memb(_clone_range_p)(_cx_self* self, _cx_value* pos,
                         const _cx_value* p1, const _cx_value* p2) {
    pos = _cx_memb(_expand_uninit_p)(self, pos, p2 - p1);
    _cx_value* it = pos;
    if (pos) for (; p1 != p2; ++p1)
        *pos++ = i_keyclone((*p1));
    return it;
}

#if !defined _i_no_emplace
STC_DEF _cx_value*
_cx_memb(_emplace_range_p)(_cx_self* self, _cx_value* pos,
                           const _cx_raw* p1, const _cx_raw* p2) {
    pos = _cx_memb(_expand_uninit_p)(self, pos, p2 - p1);
    _cx_value* it = pos;
    if (pos) for (; p1 != p2; ++p1)
        *pos++ = i_keyfrom((*p1));
    return it;
}
#endif // !_i_no_emplace
#endif // !_i_no_clone

#if !c_option(c_no_cmp)
STC_DEF _cx_iter
_cx_memb(_find_in)(_cx_iter i1, _cx_iter i2, _cx_raw raw) {
    for (; i1.ref != i2.ref; ++i1.ref) {
        const _cx_raw r = i_keyto(i1.ref);
        if (i_eq((&raw), (&r)))
            return i1;
    }
    return i2;
}

STC_DEF _cx_iter
_cx_memb(_binary_search_in)(_cx_iter i1, _cx_iter i2, _cx_raw raw, _cx_iter* lower_bound) {
    _cx_iter mid, last = i2;
    while (i1.ref != i2.ref) {
        mid.ref = i1.ref + ((i2.ref - i1.ref) >> 1);
        int c; const _cx_raw m = i_keyto(mid.ref);
        if (!(c = i_cmp((&raw), (&m))))
            return *lower_bound = mid;
        else if (c < 0)
            i2.ref = mid.ref;
        else
            i1.ref = mid.ref + 1;
    }
    *lower_bound = i1;
    return last;
}

STC_DEF int
_cx_memb(_value_cmp)(const _cx_value* x, const _cx_value* y) {
    const _cx_raw rx = i_keyto(x);
    const _cx_raw ry = i_keyto(y);
    return i_cmp((&rx), (&ry));
}
#endif // !c_no_cmp
#endif // i_implement
#define CVEC_H_INCLUDED
#include "template.h"
