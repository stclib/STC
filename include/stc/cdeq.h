/* MIT License
 *
 * Copyright (c) 2021 Tyge Løvset, NORCE, www.norceresearch.no
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

#ifndef CDEQ_H_INCLUDED
#include "ccommon.h"
#include "forward.h"
#include <stdlib.h>
#include <string.h>

struct cdeq_rep { size_t size, cap; void* base[]; };
#define cdeq_rep_(self) c_container_of((self)->_base, struct cdeq_rep, base)
#endif // CDEQ_H_INCLUDED

#ifndef i_prefix
#define i_prefix cdeq_
#endif
#include "template.h"

#if !defined i_fwd
cx_deftypes(_c_cdeq_types, Self, i_val);
#endif
typedef i_valraw cx_rawvalue_t;

STC_API Self            cx_memb(_init)(void);
STC_API Self            cx_memb(_clone)(Self cx);
STC_API void            cx_memb(_clear)(Self* self);
STC_API void            cx_memb(_del)(Self* self);
STC_API cx_value_t*     cx_memb(_push_back)(Self* self, i_val value);
STC_API void            cx_memb(_expand_right_half_)(Self* self, size_t idx, size_t n);

#ifndef i_queue
STC_API cx_iter_t       cx_memb(_find_in)(cx_iter_t p1, cx_iter_t p2, i_valraw raw);
STC_API int             cx_memb(_value_compare)(const cx_value_t* x, const cx_value_t* y);
STC_API cx_value_t*     cx_memb(_push_front)(Self* self, i_val value);
STC_API cx_iter_t       cx_memb(_erase_range_p)(Self* self, cx_value_t* p1, cx_value_t* p2);
STC_API cx_iter_t       cx_memb(_insert_range_p)(Self* self, cx_value_t* pos,
                                                const cx_value_t* p1, const cx_value_t* p2, bool clone);
STC_API cx_iter_t       cx_memb(_emplace_range_p)(Self* self, cx_value_t* pos,
                                            const cx_rawvalue_t* p1, const cx_rawvalue_t* p2);
#endif // i_queue

STC_INLINE bool         cx_memb(_empty)(Self cx) { return !cdeq_rep_(&cx)->size; }
STC_INLINE size_t       cx_memb(_size)(Self cx) { return cdeq_rep_(&cx)->size; }
STC_INLINE size_t       cx_memb(_capacity)(Self cx) { return cdeq_rep_(&cx)->cap; }
STC_INLINE void         cx_memb(_swap)(Self* a, Self* b) {c_swap(Self, *a, *b); }
STC_INLINE i_val        cx_memb(_value_fromraw)(i_valraw raw) { return i_valfrom(raw); }
STC_INLINE i_valraw     cx_memb(_value_toraw)(cx_value_t* pval) { return i_valto(pval); }
STC_INLINE i_val        cx_memb(_value_clone)(i_val val)
                            { return i_valfrom(i_valto(&val)); }
STC_INLINE void         cx_memb(_copy)(Self *self, Self other) {
                            if (self->data == other.data) return;
                            cx_memb(_del)(self); *self = cx_memb(_clone)(other);
                        }
STC_INLINE cx_value_t*  cx_memb(_emplace_back)(Self* self, i_valraw raw)
                            { return cx_memb(_push_back)(self, i_valfrom(raw)); }
STC_INLINE void         cx_memb(_pop_front)(Self* self)
                            { i_valdel(self->data); ++self->data; --cdeq_rep_(self)->size; }
STC_INLINE cx_value_t*  cx_memb(_back)(const Self* self)
                            { return self->data + cdeq_rep_(self)->size - 1; }
STC_INLINE cx_value_t*  cx_memb(_front)(const Self* self) { return self->data; }
STC_INLINE cx_iter_t    cx_memb(_begin)(const Self* self)
                            { return c_make(cx_iter_t){self->data}; }
STC_INLINE cx_iter_t    cx_memb(_end)(const Self* self)
                            { return c_make(cx_iter_t){self->data + cdeq_rep_(self)->size}; }
STC_INLINE void         cx_memb(_next)(cx_iter_t* it) { ++it->ref; }
STC_INLINE cx_iter_t    cx_memb(_advance)(cx_iter_t it, intptr_t offs)
                            { it.ref += offs; return it; }

STC_INLINE Self
cx_memb(_with_capacity)(size_t n) {
    Self cx = cx_memb(_init)();
    cx_memb(_expand_right_half_)(&cx, 0, n);
    return cx;
}

STC_INLINE void
cx_memb(_shrink_to_fit)(Self *self) {
    if (cx_memb(_size)(*self) != cx_memb(_capacity)(*self)) {
        Self cx = cx_memb(_clone)(*self);
        cx_memb(_del)(self); *self = cx;
    }
}

#ifndef i_queue

STC_INLINE cx_value_t* cx_memb(_emplace_front)(Self* self, i_valraw raw) {
    return cx_memb(_push_front)(self, i_valfrom(raw));
}

STC_INLINE void cx_memb(_pop_back)(Self* self) {
    cx_value_t* p = &self->data[--cdeq_rep_(self)->size];
    i_valdel(p);
}

STC_INLINE cx_value_t* cx_memb(_at)(const Self* self, size_t idx) {
    assert(idx < cdeq_rep_(self)->size); 
    return self->data + idx;
}

STC_INLINE size_t cx_memb(_index)(Self cx, cx_iter_t it) {
    return it.ref - cx.data;
}

STC_INLINE void
cx_memb(_reserve)(Self* self, size_t n) {
    size_t sz = cdeq_rep_(self)->size;
    if (n > sz) cx_memb(_expand_right_half_)(self, sz, n - sz);
}

STC_INLINE cx_iter_t
cx_memb(_insert)(Self* self, size_t idx, i_val value) {
    return cx_memb(_insert_range_p)(self, self->data + idx, &value, &value + 1, false);
}
STC_INLINE cx_iter_t
cx_memb(_insert_n)(Self* self, size_t idx, const cx_value_t arr[], size_t n) {
    return cx_memb(_insert_range_p)(self, self->data + idx, arr, arr + n, false);
}
STC_INLINE cx_iter_t
cx_memb(_insert_at)(Self* self, cx_iter_t it, i_val value) {
    return cx_memb(_insert_range_p)(self, it.ref, &value, &value + 1, false);
}

STC_INLINE cx_iter_t
cx_memb(_emplace)(Self* self, size_t idx, i_valraw raw) {
    return cx_memb(_emplace_range_p)(self, self->data + idx, &raw, &raw + 1);
}
STC_INLINE cx_iter_t
cx_memb(_emplace_n)(Self* self, size_t idx, const cx_rawvalue_t arr[], size_t n) {
    return cx_memb(_emplace_range_p)(self, self->data + idx, arr, arr + n);
}
STC_INLINE cx_iter_t
cx_memb(_emplace_at)(Self* self, cx_iter_t it, i_valraw raw) {
    return cx_memb(_emplace_range_p)(self, it.ref, &raw, &raw + 1);
}
STC_INLINE cx_iter_t
cx_memb(_emplace_range)(Self* self, cx_iter_t it, cx_iter_t it1, cx_iter_t it2) {
    return cx_memb(_insert_range_p)(self, it.ref, it1.ref, it2.ref, true);
}

STC_INLINE cx_iter_t
cx_memb(_erase)(Self* self, size_t idx) {
    return cx_memb(_erase_range_p)(self, self->data + idx, self->data + idx + 1);
}
STC_INLINE cx_iter_t
cx_memb(_erase_n)(Self* self, size_t idx, size_t n) {
    return cx_memb(_erase_range_p)(self, self->data + idx, self->data + idx + n);
}
STC_INLINE cx_iter_t
cx_memb(_erase_at)(Self* self, cx_iter_t it) {
    return cx_memb(_erase_range_p)(self, it.ref, it.ref + 1);
}
STC_INLINE cx_iter_t
cx_memb(_erase_range)(Self* self, cx_iter_t it1, cx_iter_t it2) {
    return cx_memb(_erase_range_p)(self, it1.ref, it2.ref);
}

STC_INLINE cx_iter_t
cx_memb(_find)(const Self* self, i_valraw raw) {
    return cx_memb(_find_in)(cx_memb(_begin)(self), cx_memb(_end)(self), raw);
}

STC_INLINE cx_value_t*
cx_memb(_get)(const Self* self, i_valraw raw) {
    cx_iter_t end = cx_memb(_end)(self);
    cx_value_t* val = cx_memb(_find_in)(cx_memb(_begin)(self), end, raw).ref;
    return val == end.ref ? NULL : val;
}

STC_INLINE void
cx_memb(_sort_range)(cx_iter_t i1, cx_iter_t i2,
                int(*_cmp_)(const cx_value_t*, const cx_value_t*)) {
    qsort(i1.ref, i2.ref - i1.ref, sizeof *i1.ref, (int(*)(const void*, const void*)) _cmp_);
}

STC_INLINE void
cx_memb(_sort)(Self* self) {
    cx_memb(_sort_range)(cx_memb(_begin)(self), cx_memb(_end)(self), cx_memb(_value_compare));
}
#endif // i_queue

/* -------------------------- IMPLEMENTATION ------------------------- */
#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION) || defined(i_imp)

#ifndef CDEQ_H_INCLUDED
static struct cdeq_rep _cdeq_sentinel = {0, 0};
#define _cdeq_nfront(self) ((self)->data - (self)->_base)
#endif

STC_DEF Self
cx_memb(_init)(void) {
    cx_value_t *b = (cx_value_t *) _cdeq_sentinel.base;
    return c_make(Self){b, b};
}

STC_DEF void
cx_memb(_clear)(Self* self) {
    struct cdeq_rep* rep = cdeq_rep_(self);
    if (rep->cap) {
        for (cx_value_t *p = self->data, *q = p + rep->size; p != q; ++p)
            i_valdel(p);
        rep->size = 0;
    }
}

STC_DEF void
cx_memb(_del)(Self* self) {
    cx_memb(_clear)(self);
    if (cdeq_rep_(self)->cap)
        c_free(cdeq_rep_(self));
}

STC_DEF size_t
cx_memb(_realloc_)(Self* self, size_t n) {
    struct cdeq_rep* rep = cdeq_rep_(self);
    size_t sz = rep->size, cap = (size_t) (sz*1.7) + n + 7;
    size_t nfront = _cdeq_nfront(self);
    rep = (struct cdeq_rep*) c_realloc(rep->cap ? rep : NULL,
                                       offsetof(struct cdeq_rep, base) + cap*sizeof(i_val));
    rep->size = sz, rep->cap = cap;
    self->_base = (cx_value_t *) rep->base;
    self->data = self->_base + nfront;
    return cap;
}

STC_DEF void
cx_memb(_expand_right_half_)(Self* self, size_t idx, size_t n) {
    struct cdeq_rep* rep = cdeq_rep_(self);
    size_t sz = rep->size, cap = rep->cap;
    size_t nfront = _cdeq_nfront(self), nback = cap - sz - nfront;
    if (nback >= n || sz*1.3 + n > cap && cx_memb(_realloc_)(self, n)) {
        memmove(self->data + idx + n, self->data + idx, (sz - idx)*sizeof(i_val));
    } else {
#ifdef i_queue
        size_t pos = 0;
#else
        size_t unused = cap - (sz + n);
        size_t pos = (nfront*2 < unused) ? nfront : unused/2;
#endif
        memmove(self->_base + pos, self->data, idx*sizeof(i_val));
        memmove(self->data + pos + idx + n, self->data + idx, (sz - idx)*sizeof(i_val));
        self->data = self->_base + pos;
    }
}

STC_DEF cx_value_t*
cx_memb(_push_back)(Self* self, i_val value) {
    struct cdeq_rep* rep = cdeq_rep_(self);
    if (_cdeq_nfront(self) + rep->size == rep->cap)
        cx_memb(_expand_right_half_)(self, rep->size, 1);
    cx_value_t *v = self->data + cdeq_rep_(self)->size++;
    *v = value; return v;
}

STC_DEF Self
cx_memb(_clone)(Self cx) {
    size_t sz = cdeq_rep_(&cx)->size;
    Self out = cx_memb(_with_capacity)(sz);
    cdeq_rep_(&out)->size = sz;
    for (size_t i = 0; i < sz; ++i) out.data[i] = i_valfrom(i_valto(&cx.data[i]));
    return out;
}

#ifndef i_queue

STC_DEF void
cx_memb(_expand_left_half_)(Self* self, size_t idx, size_t n) {
    struct cdeq_rep* rep = cdeq_rep_(self);
    size_t sz = rep->size, cap = rep->cap;
    size_t nfront = _cdeq_nfront(self), nback = cap - sz - nfront;
    if (nfront >= n) {
        self->data = (cx_value_t *) memmove(self->data - n, self->data, idx*sizeof(i_val));
    } else {
        if (sz*1.3 + n > cap) cap = cx_memb(_realloc_)(self, n);
        size_t unused = cap - (sz + n);
        size_t pos = (nback*2 < unused) ? unused - nback : unused/2;
        memmove(self->_base + pos + idx + n, self->data + idx, (sz - idx)*sizeof(i_val));
        self->data = (cx_value_t *) memmove(self->_base + pos, self->data, idx*sizeof(i_val));
    }
}

STC_DEF cx_value_t*
cx_memb(_insert_space_)(Self* self, cx_value_t* pos, size_t n) {
    size_t idx = pos - self->data;
    if (idx*2 < cdeq_rep_(self)->size) cx_memb(_expand_left_half_)(self, idx, n);
    else                               cx_memb(_expand_right_half_)(self, idx, n);
    if (n) cdeq_rep_(self)->size += n; /* do only if size > 0 */
    return self->data + idx;
}

STC_DEF cx_value_t*
cx_memb(_push_front)(Self* self, i_val value) {
    if (self->data == self->_base)
        cx_memb(_expand_left_half_)(self, 0, 1);
    else
        --self->data;
    ++cdeq_rep_(self)->size;
    *self->data = value;
    return self->data;
}

STC_DEF cx_iter_t
cx_memb(_insert_range_p)(Self* self, cx_value_t* pos,
                         const cx_value_t* p1, const cx_value_t* p2, bool clone) {
    pos = cx_memb(_insert_space_)(self, pos, p2 - p1);
    cx_iter_t it = {pos};
    if (clone) for (; p1 != p2; ++p1) *pos++ = i_valfrom(i_valto(p1));
    else memcpy(pos, p1, (p2 - p1)*sizeof *p1);
    return it;
}

STC_DEF cx_iter_t
cx_memb(_emplace_range_p)(Self* self, cx_value_t* pos, const cx_rawvalue_t* p1, const cx_rawvalue_t* p2) {
    pos = cx_memb(_insert_space_)(self, pos, p2 - p1);
    cx_iter_t it = {pos};
    for (; p1 != p2; ++p1) *pos++ = i_valfrom(*p1);
    return it;
}

STC_DEF cx_iter_t
cx_memb(_erase_range_p)(Self* self, cx_value_t* p1, cx_value_t* p2) {
    size_t n = p2 - p1;
    if (n > 0) {
        cx_value_t* p = p1, *end = self->data + cdeq_rep_(self)->size;
        for (; p != p2; ++p) i_valdel(p);
        if (p1 == self->data) self->data += n;
        else memmove(p1, p2, (end - p2) * sizeof(i_val));
        cdeq_rep_(self)->size -= n;
    }
    return c_make(cx_iter_t){p1};
}

STC_DEF cx_iter_t
cx_memb(_find_in)(cx_iter_t i1, cx_iter_t i2, i_valraw raw) {
    for (; i1.ref != i2.ref; ++i1.ref) {
        i_valraw r = i_valto(i1.ref);
        if (i_cmp(&raw, &r) == 0) return i1;
    }
    return i2;
}

STC_DEF int
cx_memb(_value_compare)(const cx_value_t* x, const cx_value_t* y) {
    i_valraw rx = i_valto(x);
    i_valraw ry = i_valto(y);
    return i_cmp(&rx, &ry);
}

#endif // i_queue
#endif // IMPLEMENTATION
#include "template.h"
#define CDEQ_H_INCLUDED
