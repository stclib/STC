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

#ifndef CDEQ_H_INCLUDED
#include "forward.h"
#include <stdlib.h>
#include <string.h>

struct cdeq_rep { size_t size, cap; unsigned base[1]; };
#define cdeq_rep_(self) c_unchecked_container_of((self)->_base, struct cdeq_rep, base)
#define _it2_ptr(it1, it2) (it1.ref && !it2.ref ? it2.end : it2.ref)
#define _it_ptr(it) (it.ref ? it.ref : it.end)
#endif // CDEQ_H_INCLUDED

#ifndef _i_prefix
#define _i_prefix cdeq_
#endif
#include "template.h"

#if !c_option(c_is_fwd)
_cx_deftypes(_c_cdeq_types, _cx_self, i_key);
#endif
typedef i_keyraw _cx_raw;

STC_API _cx_self        _cx_memb(_init)(void);
STC_API _cx_self        _cx_memb(_with_capacity)(const size_t n);
STC_API bool            _cx_memb(_reserve)(_cx_self* self, const size_t n);
STC_API void            _cx_memb(_clear)(_cx_self* self);
STC_API void            _cx_memb(_drop)(_cx_self* self);
STC_API _cx_value*      _cx_memb(_push)(_cx_self* self, i_key value);
STC_API void            _cx_memb(_shrink_to_fit)(_cx_self *self);

#if !defined _i_queue
#if !defined _i_no_emplace
STC_API _cx_iter        _cx_memb(_emplace_range)(_cx_self* self, _cx_value* pos,
                                                 const _cx_raw* p1, const _cx_raw* p2);
#endif // _i_no_emplace

#if !defined _i_no_cmp
STC_API _cx_iter        _cx_memb(_find_in)(_cx_iter p1, _cx_iter p2, _cx_raw raw);
STC_API int             _cx_memb(_value_cmp)(const _cx_value* x, const _cx_value* y);
#endif
STC_API _cx_value*      _cx_memb(_push_front)(_cx_self* self, i_key value);
STC_API _cx_iter        _cx_memb(_erase_range_p)(_cx_self* self, _cx_value* p1, _cx_value* p2);
STC_API _cx_iter        _cx_memb(_insert_range)(_cx_self* self, _cx_value* pos,
                                                const _cx_value* p1, const _cx_value* p2);
#endif // !_i_queue

#if !defined _i_no_emplace
STC_INLINE _cx_value*   _cx_memb(_emplace)(_cx_self* self, _cx_raw raw)
                            { return _cx_memb(_push)(self, i_keyfrom(raw)); }
#endif

#if !defined _i_no_clone
#if !defined _i_queue
STC_API _cx_iter        _cx_memb(_copy_range)(_cx_self* self, _cx_value* pos,
                                                const _cx_value* p1, const _cx_value* p2);

STC_INLINE void         _cx_memb(_copy)(_cx_self *self, const _cx_self* other) {
                            if (self->data == other->data) return;
                            _cx_memb(_clear)(self);
                            _cx_memb(_copy_range)(self, self->data, other->data, 
                                                    other->data + cdeq_rep_(other)->size);
                        }
#endif // !_i_queue
STC_API _cx_self        _cx_memb(_clone)(_cx_self cx);
STC_INLINE i_key        _cx_memb(_value_clone)(i_key val)
                            { return i_keyclone(val); }
#endif // !_i_no_clone
STC_INLINE size_t       _cx_memb(_size)(const _cx_self* cx) { return cdeq_rep_(cx)->size; }
STC_INLINE size_t       _cx_memb(_capacity)(const _cx_self* cx) { return cdeq_rep_(cx)->cap; }
STC_INLINE bool         _cx_memb(_empty)(const _cx_self* cx) { return !cdeq_rep_(cx)->size; }
STC_INLINE _cx_raw      _cx_memb(_value_toraw)(const _cx_value* pval) { return i_keyto(pval); }
STC_INLINE void         _cx_memb(_swap)(_cx_self* a, _cx_self* b) { c_swap(_cx_self, *a, *b); }
STC_INLINE _cx_value*   _cx_memb(_front)(const _cx_self* self) { return self->data; }
STC_INLINE _cx_value*   _cx_memb(_back)(const _cx_self* self)
                            { return self->data + cdeq_rep_(self)->size - 1; }
STC_INLINE void         _cx_memb(_pop_front)(_cx_self* self) // == _pop() when _i_queue
                            { assert(!_cx_memb(_empty)(self)); i_keydrop(self->data); ++self->data; --cdeq_rep_(self)->size; }

STC_INLINE _cx_iter _cx_memb(_begin)(const _cx_self* self) {
    size_t n = cdeq_rep_(self)->size;
    return c_make(_cx_iter){n ? self->data : NULL, self->data + n};
}

STC_INLINE _cx_iter _cx_memb(_end)(const _cx_self* self)
    { return c_make(_cx_iter){NULL, self->data + cdeq_rep_(self)->size}; }

STC_INLINE void _cx_memb(_next)(_cx_iter* it)
    { if (++it->ref == it->end) it->ref = NULL; }

STC_INLINE _cx_iter _cx_memb(_advance)(_cx_iter it, intptr_t n)
    { if ((it.ref += n) >= it.end) it.ref = NULL; return it; }

#if !defined _i_queue

STC_INLINE size_t       _cx_memb(_index)(const _cx_self* cx, _cx_iter it)
                            { return it.ref - cx->data; }
STC_INLINE void         _cx_memb(_pop_back)(_cx_self* self)
                            { assert(!_cx_memb(_empty)(self)); _cx_value* p = &self->data[--cdeq_rep_(self)->size]; i_keydrop(p); }

STC_INLINE const _cx_value* _cx_memb(_at)(const _cx_self* self, const size_t idx) {
    assert(idx < cdeq_rep_(self)->size);  return self->data + idx;
}
STC_INLINE _cx_value* _cx_memb(_at_mut)(_cx_self* self, const size_t idx) {
    assert(idx < cdeq_rep_(self)->size);  return self->data + idx;
}

STC_INLINE _cx_value* _cx_memb(_push_back)(_cx_self* self, i_key value) {
    return _cx_memb(_push)(self, value);
}
STC_INLINE _cx_iter
_cx_memb(_insert)(_cx_self* self, const size_t idx, i_key value) {
    return _cx_memb(_insert_range)(self, self->data + idx, &value, &value + 1);
}
STC_INLINE _cx_iter
_cx_memb(_insert_n)(_cx_self* self, const size_t idx, const _cx_value arr[], const size_t n) {
    return _cx_memb(_insert_range)(self, self->data + idx, arr, arr + n);
}
STC_INLINE _cx_iter
_cx_memb(_insert_at)(_cx_self* self, _cx_iter it, i_key value) {
    return _cx_memb(_insert_range)(self, _it_ptr(it), &value, &value + 1);
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
_cx_memb(_erase_range)(_cx_self* self, _cx_iter i1, _cx_iter i2) {
    return _cx_memb(_erase_range_p)(self, i1.ref, _it2_ptr(i1, i2));
}

#if !defined _i_no_emplace
STC_INLINE _cx_value*
_cx_memb(_emplace_front)(_cx_self* self, _cx_raw raw) {
    return _cx_memb(_push_front)(self, i_keyfrom(raw));
}

STC_INLINE _cx_value* _cx_memb(_emplace_back)(_cx_self* self, _cx_raw raw) {
    return _cx_memb(_push)(self, i_keyfrom(raw));
}

STC_INLINE _cx_iter
_cx_memb(_emplace_n)(_cx_self* self, const size_t idx, const _cx_raw arr[], const size_t n) {
    return _cx_memb(_emplace_range)(self, self->data + idx, arr, arr + n);
}
STC_INLINE _cx_iter
_cx_memb(_emplace_at)(_cx_self* self, _cx_iter it, _cx_raw raw) {
    return _cx_memb(_emplace_range)(self, _it_ptr(it), &raw, &raw + 1);
}
#endif // !_i_no_emplace

#if !defined _i_no_cmp

STC_INLINE _cx_iter
_cx_memb(_find)(const _cx_self* self, _cx_raw raw) {
    return _cx_memb(_find_in)(_cx_memb(_begin)(self), _cx_memb(_end)(self), raw);
}

STC_INLINE const _cx_value*
_cx_memb(_get)(const _cx_self* self, _cx_raw raw) {
    return _cx_memb(_find_in)(_cx_memb(_begin)(self), _cx_memb(_end)(self), raw).ref;
}

STC_INLINE _cx_value*
_cx_memb(_get_mut)(_cx_self* self, _cx_raw raw)
    { return (_cx_value *) _cx_memb(_get)(self, raw); }

STC_INLINE void
_cx_memb(_sort_range)(_cx_iter i1, _cx_iter i2, int(*cmp)(const _cx_value*, const _cx_value*)) {
    qsort(i1.ref, _it2_ptr(i1, i2) - i1.ref, sizeof *i1.ref,
          (int(*)(const void*, const void*)) cmp);
}

STC_INLINE void
_cx_memb(_sort)(_cx_self* self) {
    _cx_memb(_sort_range)(_cx_memb(_begin)(self), _cx_memb(_end)(self), _cx_memb(_value_cmp));
}
#endif // !c_no_cmp
#endif // _i_queue

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined(i_implement)

#ifndef CDEQ_H_INCLUDED
static struct cdeq_rep _cdeq_sentinel = {0, 0};
#define _cdeq_nfront(self) ((self)->data - (self)->_base)
#endif

STC_DEF _cx_self
_cx_memb(_init)(void) {
    _cx_value *b = (_cx_value *) _cdeq_sentinel.base;
    return c_make(_cx_self){b, b};
}

STC_DEF void
_cx_memb(_clear)(_cx_self* self) {
    struct cdeq_rep* rep = cdeq_rep_(self);
    if (rep->cap) {
        for (_cx_value *p = self->data, *q = p + rep->size; p != q; ) {
            --q; i_keydrop(q);
        }
        rep->size = 0;
        self->data = self->_base;
    }
}

STC_DEF void
_cx_memb(_shrink_to_fit)(_cx_self *self) {
    if (_cx_memb(_size)(self) != _cx_memb(_capacity)(self)) {
        struct cdeq_rep* rep = cdeq_rep_(self);
        const size_t sz = rep->size;
        memmove(self->_base, self->data, sz*sizeof(i_key));
        rep = (struct cdeq_rep*) c_realloc(rep, offsetof(struct cdeq_rep, base) +
                                                sz*sizeof(i_key));
        if (rep) {
            self->_base = self->data = (_cx_value*)rep->base;
            rep->cap = sz;
        }
    }
}

STC_DEF void
_cx_memb(_drop)(_cx_self* self) {
    struct cdeq_rep* rep = cdeq_rep_(self);
    // second test to supress gcc -O2 warn: -Wfree-nonheap-object
    if (rep->cap == 0 || rep == &_cdeq_sentinel)
        return;
    _cx_memb(_clear)(self);
    c_free(rep);
}

static size_t
_cx_memb(_realloc_)(_cx_self* self, const size_t n) {
    struct cdeq_rep* rep = cdeq_rep_(self);
    const size_t sz = rep->size, cap = (size_t) (sz*1.7) + n + 7;
    const size_t nfront = _cdeq_nfront(self);
    rep = (struct cdeq_rep*) c_realloc(rep->cap ? rep : NULL,
                                       offsetof(struct cdeq_rep, base) + cap*sizeof(i_key));
    if (!rep)
        return 0; 
    rep->size = sz, rep->cap = cap;
    self->_base = (_cx_value *) rep->base;
    self->data = self->_base + nfront;
    return cap;
}

static bool
_cx_memb(_expand_right_half_)(_cx_self* self, const size_t idx, const size_t n) {
    struct cdeq_rep* rep = cdeq_rep_(self);
    const size_t sz = rep->size, cap = rep->cap;
    const size_t nfront = _cdeq_nfront(self), nback = cap - sz - nfront;
    if (nback >= n || sz*1.3 + n > cap) {
        if (!_cx_memb(_realloc_)(self, n))
            return false;
        memmove(self->data + idx + n, self->data + idx, (sz - idx)*sizeof(i_key));
    } else {
#if !defined _i_queue
        const size_t unused = cap - (sz + n);
        const size_t pos = (nfront*2 < unused) ? nfront : unused/2;
#else
        const size_t pos = 0;
#endif
        memmove(self->_base + pos, self->data, idx*sizeof(i_key));
        memmove(self->data + pos + idx + n, self->data + idx, (sz - idx)*sizeof(i_key));
        self->data = self->_base + pos;
    }
    return true;
}

STC_DEF _cx_self
_cx_memb(_with_capacity)(const size_t n) {
    _cx_self cx = _cx_memb(_init)();
    _cx_memb(_expand_right_half_)(&cx, 0, n);
    return cx;
}

STC_DEF bool
_cx_memb(_reserve)(_cx_self* self, const size_t n) {
    const size_t sz = cdeq_rep_(self)->size;
    return n <= sz || _cx_memb(_expand_right_half_)(self, sz, n - sz);
}

STC_DEF _cx_value*
_cx_memb(_push)(_cx_self* self, i_key value) {
    struct cdeq_rep* r = cdeq_rep_(self);
    if (_cdeq_nfront(self) + r->size == r->cap) {
        _cx_memb(_expand_right_half_)(self, r->size, 1);
        r = cdeq_rep_(self);
    }
    _cx_value *v = self->data + r->size++;
    *v = value; return v;
}

#if !c_option(c_no_clone)
STC_DEF _cx_self
_cx_memb(_clone)(_cx_self cx) {
    const size_t sz = cdeq_rep_(&cx)->size;
    _cx_self out = _cx_memb(_with_capacity)(sz);
    struct cdeq_rep* r = cdeq_rep_(&out);
    if (r->cap) {
        for (size_t i = 0; i < sz; ++i) out.data[i] = i_keyclone(cx.data[i]);
        r->size = sz;
    }
    return out;
}
#endif

#if !defined _i_queue

static void
_cx_memb(_expand_left_half_)(_cx_self* self, const size_t idx, const size_t n) {
    struct cdeq_rep* rep = cdeq_rep_(self);
    size_t cap = rep->cap;
    const size_t sz = rep->size;
    const size_t nfront = _cdeq_nfront(self), nback = cap - sz - nfront;
    if (nfront >= n) {
        self->data = (_cx_value *)memmove(self->data - n, self->data, idx*sizeof(i_key));
    } else {
        if (sz*1.3 + n > cap)
            cap = _cx_memb(_realloc_)(self, n);
        const size_t unused = cap - (sz + n);
        const size_t pos = (nback*2 < unused) ? unused - nback : unused/2;
        memmove(self->_base + pos + idx + n, self->data + idx, (sz - idx)*sizeof(i_key));
        self->data = (_cx_value *)memmove(self->_base + pos, self->data, idx*sizeof(i_key));
    }
}

static _cx_iter
_cx_memb(_insert_uninit)(_cx_self* self, _cx_value* pos, const size_t n) {
    struct cdeq_rep* r = cdeq_rep_(self);
    if (n) {
        if (!pos) pos = self->data + r->size;
        const size_t idx = pos - self->data;
        if (idx*2 < r->size)
            _cx_memb(_expand_left_half_)(self, idx, n);
        else
            _cx_memb(_expand_right_half_)(self, idx, n);
        r = cdeq_rep_(self);
        r->size += n;
        pos = self->data + idx;
    }
    return c_make(_cx_iter){pos, self->data + r->size};
}

STC_DEF _cx_value*
_cx_memb(_push_front)(_cx_self* self, i_key value) {
    if (self->data == self->_base)
        _cx_memb(_expand_left_half_)(self, 0, 1);
    else
        --self->data;
    ++cdeq_rep_(self)->size;
    *self->data = value;
    return self->data;
}

STC_DEF _cx_iter
_cx_memb(_insert_range)(_cx_self* self, _cx_value* pos,
                          const _cx_value* p1, const _cx_value* p2) {
    _cx_iter it = _cx_memb(_insert_uninit)(self, pos, p2 - p1);
    if (it.ref)
        memcpy(it.ref, p1, (p2 - p1)*sizeof *p1);
    return it;
}

STC_DEF _cx_iter
_cx_memb(_erase_range_p)(_cx_self* self, _cx_value* p1, _cx_value* p2) {
    assert(p1 && p2);
    intptr_t len = p2 - p1;
    struct cdeq_rep* r = cdeq_rep_(self);
    _cx_value* p = p1, *end = self->data + r->size;
    for (; p != p2; ++p)
        { i_keydrop(p); }
    memmove(p1, p2, (end - p2) * sizeof *p1);
    r->size -= len;
    return c_make(_cx_iter){p2 == end ? NULL : p1, end - len};
}

#if !defined _i_no_clone
STC_DEF _cx_iter
_cx_memb(_copy_range)(_cx_self* self, _cx_value* pos,
                      const _cx_value* p1, const _cx_value* p2) {
    _cx_iter it = _cx_memb(_insert_uninit)(self, pos, p2 - p1);
    if (it.ref)
        for (_cx_value* p = it.ref; p1 != p2; ++p1)
            *p++ = i_keyclone((*p1));
    return it;
}
#endif // !_i_no_clone

#if !defined _i_no_emplace
STC_DEF _cx_iter
_cx_memb(_emplace_range)(_cx_self* self, _cx_value* pos,
                         const _cx_raw* p1, const _cx_raw* p2) {
    _cx_iter it = _cx_memb(_insert_uninit)(self, pos, p2 - p1);
    if (it.ref)
        for (_cx_value* p = it.ref; p1 != p2; ++p1)
            *p++ = i_keyfrom((*p1));
    return it;
}
#endif // !_i_no_emplace

#if !defined _i_no_cmp

STC_DEF _cx_iter
_cx_memb(_find_in)(_cx_iter i1, _cx_iter i2, _cx_raw raw) {
    const _cx_value* p2 = _it2_ptr(i1, i2);
    for (; i1.ref != p2; ++i1.ref) {
        const _cx_raw r = i_keyto(i1.ref);
        if (i_eq((&raw), (&r)))
            return i1;
    }
    i2.ref = NULL; return i2; // NB!
}

STC_DEF int
_cx_memb(_value_cmp)(const _cx_value* x, const _cx_value* y) {
    const _cx_raw rx = i_keyto(x);
    const _cx_raw ry = i_keyto(y);
    return i_cmp((&rx), (&ry));
}
#endif // !c_no_cmp
#endif // !_i_queue
#endif // IMPLEMENTATION
#include "template.h"
#define CDEQ_H_INCLUDED
