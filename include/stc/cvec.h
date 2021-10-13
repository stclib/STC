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

#define i_fwd // forward declared
#define i_tag i32
#define i_val int
#include <stc/cvec.h>

int main() {
    cvec_i32 vec = cvec_i32_init();
    cvec_i32_push_back(&vec, 123);
    cvec_i32_del(&vec);

    cvec_float fvec = cvec_float_init();
    cvec_float_push_back(&fvec, 123.3);
    cvec_float_del(&fvec);

    cvec_str svec = cvec_str_init();
    cvec_str_emplace_back(&svec, "Hello, friend");
    cvec_str_del(&svec);
}
*/

#ifndef CVEC_H_INCLUDED
#include "ccommon.h"
#include "forward.h"
#include <stdlib.h>
#include <string.h>

struct cvec_rep { size_t size, cap; void* data[]; };
#define cvec_rep_(self) c_container_of((self)->data, struct cvec_rep, data)
#endif // CVEC_H_INCLUDED

#ifndef i_prefix
#define i_prefix cvec_
#endif
#include "template.h"

#if !defined i_fwd
   cx_deftypes(_c_cvec_types, Self, i_val);
#endif
typedef i_valraw cx_rawvalue_t;

STC_API Self            cx_memb(_init)(void);
STC_API Self            cx_memb(_clone)(Self cx);
STC_API void            cx_memb(_del)(Self* self);
STC_API void            cx_memb(_clear)(Self* self);
STC_API void            cx_memb(_reserve)(Self* self, size_t cap);
STC_API void            cx_memb(_resize)(Self* self, size_t size, i_val fill_val);
STC_API int             cx_memb(_value_compare)(const cx_value_t* x, const cx_value_t* y);
STC_API cx_iter_t       cx_memb(_find_in)(cx_iter_t it1, cx_iter_t it2, i_valraw raw);
STC_API cx_iter_t       cx_memb(_bsearch_in)(cx_iter_t it1, cx_iter_t it2, i_valraw raw);
STC_API cx_value_t*     cx_memb(_push_back)(Self* self, i_val value);
STC_API cx_iter_t       cx_memb(_erase_range_p)(Self* self, cx_value_t* p1, cx_value_t* p2);
STC_API cx_iter_t       cx_memb(_insert_range_p)(Self* self, cx_value_t* pos,
                                                 const cx_value_t* p1, const cx_value_t* p2, bool clone);
STC_API cx_iter_t       cx_memb(_emplace_range_p)(Self* self, cx_value_t* pos,
                                                  const cx_rawvalue_t* p1, const cx_rawvalue_t* p2);

STC_INLINE size_t       cx_memb(_size)(Self cx) { return cvec_rep_(&cx)->size; }
STC_INLINE size_t       cx_memb(_capacity)(Self cx) { return cvec_rep_(&cx)->cap; }
STC_INLINE bool         cx_memb(_empty)(Self cx) { return !cvec_rep_(&cx)->size; }
STC_INLINE i_val        cx_memb(_value_fromraw)(i_valraw raw) { return i_valfrom(raw); }
STC_INLINE i_valraw     cx_memb(_value_toraw)(cx_value_t* val) { return i_valto(val); }
STC_INLINE i_val        cx_memb(_value_clone)(cx_value_t val)
                            { return i_valfrom(i_valto(&val)); }
STC_INLINE void         cx_memb(_swap)(Self* a, Self* b) { c_swap(Self, *a, *b); }
STC_INLINE cx_value_t*  cx_memb(_front)(const Self* self) { return self->data; }
STC_INLINE cx_value_t*  cx_memb(_back)(const Self* self)
                            { return self->data + cvec_rep_(self)->size - 1; }
STC_INLINE cx_value_t*  cx_memb(_emplace_back)(Self* self, i_valraw raw)
                            { return cx_memb(_push_back)(self, i_valfrom(raw)); }
STC_INLINE void         cx_memb(_pop_back)(Self* self)
                            { cx_value_t* p = &self->data[--cvec_rep_(self)->size]; i_valdel(p); }
STC_INLINE cx_iter_t    cx_memb(_begin)(const Self* self)
                            { return c_make(cx_iter_t){self->data}; }
STC_INLINE cx_iter_t    cx_memb(_end)(const Self* self)
                            { return c_make(cx_iter_t){self->data + cvec_rep_(self)->size}; }
STC_INLINE void         cx_memb(_next)(cx_iter_t* it) { ++it->ref; }
STC_INLINE cx_iter_t    cx_memb(_advance)(cx_iter_t it, intptr_t offs)
                            { it.ref += offs; return it; }
STC_INLINE size_t       cx_memb(_index)(Self cx, cx_iter_t it) { return it.ref - cx.data; }

STC_INLINE Self
cx_memb(_with_size)(size_t size, i_val null_val) {
    Self cx = cx_memb(_init)();
    cx_memb(_resize)(&cx, size, null_val);
    return cx;
}

STC_INLINE Self
cx_memb(_with_capacity)(size_t size) {
    Self cx = cx_memb(_init)();
    cx_memb(_reserve)(&cx, size);
    return cx;
}

STC_INLINE void
cx_memb(_shrink_to_fit)(Self *self) {
    cx_memb(_reserve)(self, cx_memb(_size)(*self));
}

STC_INLINE void
cx_memb(_copy)(Self *self, Self other) {
    if (self->data == other.data) return;
    cx_memb(_del)(self); *self = cx_memb(_clone)(other);
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

STC_INLINE cx_value_t*
cx_memb(_at)(const Self* self, size_t idx) {
    assert(idx < cvec_rep_(self)->size);
    return self->data + idx;
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

STC_INLINE cx_iter_t
cx_memb(_bsearch)(const Self* self, i_valraw raw) {
    return cx_memb(_bsearch_in)(cx_memb(_begin)(self), cx_memb(_end)(self), raw);
}
STC_INLINE void
cx_memb(_sort_range)(cx_iter_t i1, cx_iter_t i2,
                     int(*_cmp_)(const cx_value_t*, const cx_value_t*)) {
    qsort(i1.ref, i2.ref - i1.ref, sizeof(cx_value_t), (int(*)(const void*, const void*)) _cmp_);
}
STC_INLINE void
cx_memb(_sort)(Self* self) {
    cx_memb(_sort_range)(cx_memb(_begin)(self), cx_memb(_end)(self), cx_memb(_value_compare));
}

/* -------------------------- IMPLEMENTATION ------------------------- */
#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION) || defined(i_imp)

#ifndef CVEC_H_INCLUDED
static struct cvec_rep _cvec_sentinel = {0, 0};
#endif

STC_DEF Self
cx_memb(_init)(void) {
    Self cx = {(cx_value_t *) _cvec_sentinel.data};
    return cx;
}

STC_DEF void
cx_memb(_clear)(Self* self) {
    struct cvec_rep* rep = cvec_rep_(self);
    if (rep->cap) {
        for (cx_value_t *p = self->data, *q = p + rep->size; p != q; ++p)
            i_valdel(p);
        rep->size = 0;
    }
}

STC_DEF void
cx_memb(_del)(Self* self) {
    cx_memb(_clear)(self);
    if (cvec_rep_(self)->cap)
        c_free(cvec_rep_(self));
}

STC_DEF void
cx_memb(_reserve)(Self* self, size_t cap) {
    struct cvec_rep* rep = cvec_rep_(self);
    size_t len = rep->size;
    if (cap >= len && cap != rep->cap) {
        rep = (struct cvec_rep*) c_realloc(rep->cap ? rep : NULL,
                                           offsetof(struct cvec_rep, data) + cap*sizeof(i_val));
        self->data = (cx_value_t*) rep->data;
        rep->size = len;
        rep->cap = cap;
    }
}

STC_DEF void
cx_memb(_resize)(Self* self, size_t len, i_val fill) {
    if (len > cx_memb(_capacity)(*self))
        cx_memb(_reserve)(self, len);
    struct cvec_rep* rep = cvec_rep_(self);
    size_t i, n = rep->size;
    for (i = len; i < n; ++i) i_valdel(&self->data[i]);
    for (i = n; i < len; ++i) self->data[i] = fill;
    if (rep->cap) rep->size = len;
}

STC_DEF cx_value_t*
cx_memb(_push_back)(Self* self, i_val value) {
    size_t len = cvec_rep_(self)->size;
    if (len == cx_memb(_capacity)(*self))
        cx_memb(_reserve)(self, (len*13 >> 3) + 4);
    cx_value_t *v = self->data + cvec_rep_(self)->size++;
    *v = value; return v;
}

STC_DEF Self
cx_memb(_clone)(Self cx) {
    size_t len = cvec_rep_(&cx)->size;
    Self out = cx_memb(_with_capacity)(len);
    cx_memb(_insert_range_p)(&out, out.data, cx.data, cx.data + len, true);
    return out;
}

STC_DEF cx_value_t*
cx_memb(_insert_space_)(Self* self, cx_value_t* pos, size_t len) {
    size_t idx = pos - self->data, size = cvec_rep_(self)->size;
    if (len == 0) return pos;
    if (size + len > cx_memb(_capacity)(*self))
        cx_memb(_reserve)(self, (size*13 >> 3) + len),
        pos = self->data + idx;
    cvec_rep_(self)->size += len;
    memmove(pos + len, pos, (size - idx) * sizeof(i_val));
    return pos;
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
cx_memb(_emplace_range_p)(Self* self, cx_value_t* pos, const cx_rawvalue_t* p1,
                                                       const cx_rawvalue_t* p2) {
    pos = cx_memb(_insert_space_)(self, pos, p2 - p1);
    cx_iter_t it = {pos};
    for (; p1 != p2; ++p1) *pos++ = i_valfrom(*p1);
    return it;
}

STC_DEF cx_iter_t
cx_memb(_erase_range_p)(Self* self, cx_value_t* p1, cx_value_t* p2) {
    intptr_t len = p2 - p1;
    if (len > 0) {
        cx_value_t* p = p1, *end = self->data + cvec_rep_(self)->size;
        for (; p != p2; ++p) i_valdel(p);
        memmove(p1, p2, (end - p2) * sizeof(i_val));
        cvec_rep_(self)->size -= len;
    }
    return c_make(cx_iter_t){.ref = p1};
}

STC_DEF cx_iter_t
cx_memb(_find_in)(cx_iter_t i1, cx_iter_t i2, i_valraw raw) {
    for (; i1.ref != i2.ref; ++i1.ref) {
        i_valraw r = i_valto(i1.ref);
        if (i_cmp(&raw, &r) == 0) return i1;
    }
    return i2;
}

STC_DEF cx_iter_t
cx_memb(_bsearch_in)(cx_iter_t i1, cx_iter_t i2, i_valraw raw) {
    cx_iter_t mid, last = i2;
    while (i1.ref != i2.ref) {
        mid.ref = i1.ref + ((i2.ref - i1.ref)>>1);
        int c; i_valraw m = i_valto(mid.ref);
        if ((c = i_cmp(&raw, &m)) == 0) return mid;
        else if (c < 0) i2.ref = mid.ref;
        else i1.ref = mid.ref + 1;
    }
    return last;
}

STC_DEF int
cx_memb(_value_compare)(const cx_value_t* x, const cx_value_t* y) {
    i_valraw rx = i_valto(x);
    i_valraw ry = i_valto(y);
    return i_cmp(&rx, &ry);
}

#endif
#include "template.h"
#define CVEC_H_INCLUDED
