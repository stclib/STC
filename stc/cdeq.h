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
#define CDEQ_H_INCLUDED

#include "ccommon.h"
#include <stdlib.h>
#include <string.h>

#define using_cdeq(...) c_MACRO_OVERLOAD(using_cdeq, __VA_ARGS__)

#define using_cdeq_2(X, Value) \
            using_cdeq_3(X, Value, c_default_compare)
#define using_cdeq_3(X, Value, valueCompare) \
            using_cdeq_5(X, Value, valueCompare, c_trivial_del, c_trivial_fromraw)
#define using_cdeq_4(X, Value, valueCompare, valueDel) \
            using_cdeq_5(X, Value, valueCompare, valueDel, c_no_clone)
#define using_cdeq_5(X, Value, valueCompare, valueDel, valueClone) \
            using_cdeq_7(X, Value, valueCompare, valueDel, valueClone, c_trivial_toraw, Value)
#define using_cdeq_7(X, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue) \
            _c_using_cdeq(cdeq_##X, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue)

#define using_cdeq_str() \
            _c_using_cdeq(cdeq_str, cstr_t, cstr_compare_raw, cstr_del, cstr_from, cstr_c_str, const char*)


struct cdeq_rep { size_t size, cap; void* base[]; };
#define cdeq_rep_(self) c_container_of((self)->_base, struct cdeq_rep, base)


#define _c_using_cdeq(CX, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue) \
\
    typedef Value       CX##_value_t; \
    typedef RawValue    CX##_rawvalue_t; \
    typedef struct      {CX##_value_t *ref; } CX##_iter_t; \
    typedef struct      {CX##_value_t *_base, *data;} CX; \
\
    STC_API CX          CX##_init(void); \
    STC_API CX          CX##_clone(CX deq); \
    STC_API void        CX##_clear(CX* self); \
    STC_API void        CX##_del(CX* self); \
    STC_API void        CX##_expand_(CX* self, size_t n, bool at_front); \
    STC_API void        CX##_resize(CX* self, size_t size, Value fill_val); \
    STC_API CX##_iter_t CX##_find_in(CX##_iter_t p1, CX##_iter_t p2, RawValue raw); \
    STC_API int         CX##_value_compare(const CX##_value_t* x, const CX##_value_t* y); \
    STC_API void        CX##_emplace_n(CX *self, const CX##_rawvalue_t arr[], size_t n); \
    STC_API void        CX##_push_back(CX* self, Value value); \
    STC_API void        CX##_push_front(CX* self, Value value); \
    STC_API CX##_iter_t CX##_erase_range_p(CX* self, CX##_value_t* p1, CX##_value_t* p2); \
    STC_API CX##_iter_t CX##_insert_range_p(CX* self, CX##_value_t* pos, const CX##_value_t* p1, \
                                                                         const CX##_value_t* p2, bool clone); \
    STC_INLINE bool     CX##_empty(CX deq) {return !cdeq_rep_(&deq)->size;} \
    STC_INLINE size_t   CX##_size(CX deq) {return cdeq_rep_(&deq)->size;} \
    STC_INLINE size_t   CX##_capacity(CX deq) {return cdeq_rep_(&deq)->cap;} \
    STC_INLINE void     CX##_swap(CX* a, CX* b) {c_swap(CX, *a, *b);} \
    STC_INLINE Value    CX##_value_fromraw(RawValue raw) {return valueFromRaw(raw);} \
    STC_INLINE Value    CX##_value_clone(Value val) \
                            {return valueFromRaw(valueToRaw(&val));} \
    STC_INLINE void     CX##_reserve(CX* self, size_t n) \
                            {CX##_expand_(self, (n - cdeq_rep_(self)->size)*0.65, false);} \
    STC_INLINE void     CX##_emplace_back(CX* self, RawValue raw) \
                            {CX##_push_back(self, valueFromRaw(raw));} \
    STC_INLINE void     CX##_emplace_front(CX* self, RawValue raw) \
                            {CX##_push_front(self, valueFromRaw(raw));} \
    STC_INLINE void     CX##_pop_back(CX* self) \
                            {valueDel(&self->data[--cdeq_rep_(self)->size]);} \
    STC_INLINE void     CX##_pop_front(CX* self) \
                            {valueDel(self->data++); --cdeq_rep_(self)->size;} \
    STC_INLINE \
    CX##_value_t*       CX##_front(const CX* self) {return self->data;} \
    STC_INLINE \
    CX##_value_t*       CX##_back(const CX* self) \
                            {return self->data + cdeq_rep_(self)->size - 1;} \
    STC_INLINE \
    CX##_value_t*       CX##_at(const CX* self, size_t idx) \
                            {assert(idx < cdeq_rep_(self)->size); return self->data + idx;} \
\
    STC_INLINE CX \
    CX##_with_size(size_t size, Value null_val) { \
        CX x = CX##_init(); \
        CX##_resize(&x, size, null_val); \
        return x; \
    } \
    STC_INLINE CX \
    CX##_with_capacity(size_t size) { \
        CX x = CX##_init(); \
        CX##_expand_(&x, size, false); \
        return x; \
    } \
\
    STC_INLINE void \
    CX##_shrink_to_fit(CX *self) { \
        CX x = CX##_clone(*self); \
        CX##_del(self); *self = x; \
    } \
\
    STC_INLINE CX##_iter_t \
    CX##_insert(CX* self, CX##_iter_t it, Value value) { \
        it = CX##_insert_range_p(self, it.ref, &value, &value + 1, false); \
        *it.ref = value; return it; \
    } \
    STC_INLINE CX##_iter_t \
    CX##_emplace(CX* self, CX##_iter_t it, RawValue raw) { \
        return CX##_insert(self, it, valueFromRaw(raw)); \
    } \
    STC_INLINE CX##_iter_t \
    CX##_insert_range(CX* self, CX##_iter_t it, CX##_iter_t it1, CX##_iter_t it2) { \
        return CX##_insert_range_p(self, it.ref, it1.ref, it2.ref, true); \
    } \
    STC_INLINE CX##_iter_t \
    CX##_insert_at(CX* self, size_t idx, const CX##_value_t arr[], size_t n) { \
        return CX##_insert_range_p(self, self->data + idx, arr, arr + n, true); \
    } \
\
    STC_INLINE CX##_iter_t \
    CX##_erase_range(CX* self, CX##_iter_t it1, CX##_iter_t it2) { \
        return CX##_erase_range_p(self, it1.ref, it2.ref); \
    } \
    STC_INLINE CX##_iter_t \
    CX##_erase_at(CX* self, CX##_iter_t it) { \
        return CX##_erase_range_p(self, it.ref, it.ref + 1); \
    } \
    STC_INLINE CX##_iter_t \
    CX##_erase_n(CX* self, size_t idx, size_t n) { \
        return CX##_erase_range_p(self, self->data + idx, self->data + idx + n); \
    } \
\
    STC_INLINE CX##_iter_t \
    CX##_begin(const CX* self) { \
        CX##_iter_t it = {self->data}; return it; \
    } \
    STC_INLINE CX##_iter_t \
    CX##_end(const CX* self) { \
        CX##_iter_t it = {self->data + cdeq_rep_(self)->size}; return it; \
    } \
\
    STC_INLINE void \
    CX##_next(CX##_iter_t* it) {++it->ref;} \
    STC_INLINE size_t \
    CX##_index(CX deq, CX##_iter_t it) {return it.ref - deq.data;} \
\
    STC_INLINE CX##_iter_t \
    CX##_find(const CX* self, RawValue raw) { \
        return CX##_find_in(CX##_begin(self), CX##_end(self), raw); \
    } \
\
    STC_INLINE void \
    CX##_sort_range(CX##_iter_t i1, CX##_iter_t i2, \
                    int(*cmp)(const CX##_value_t*, const CX##_value_t*)) { \
        qsort(i1.ref, i2.ref - i1.ref, sizeof *i1.ref, (int(*)(const void*, const void*)) cmp); \
    } \
\
    STC_INLINE void \
    CX##_sort(CX* self) { \
        CX##_sort_range(CX##_begin(self), CX##_end(self), CX##_value_compare); \
    } \
\
    _c_implement_cdeq(CX, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue) \
    struct stc_trailing_semicolon

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)

static struct cdeq_rep _cdeq_inits = {0, 0};
#define _cdeq_nfront(self) ((self)->data - (self)->_base)

#define _c_implement_cdeq(CX, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue) \
\
    STC_DEF CX \
    CX##_init(void) { \
        CX##_value_t *b = (CX##_value_t *) _cdeq_inits.base; \
        CX deq = {b, b}; return deq; \
    } \
\
    STC_DEF void \
    CX##_emplace_n(CX *self, const CX##_rawvalue_t arr[], size_t n) { \
        if (!n) return; \
        CX##_expand_(self, n, false); \
        CX##_value_t* p = self->data + cdeq_rep_(self)->size; \
        for (size_t i=0; i < n; ++i) *p++ = valueFromRaw(arr[i]); \
        cdeq_rep_(self)->size += n; \
    } \
\
    STC_DEF void \
    CX##_clear(CX* self) { \
        struct cdeq_rep* rep = cdeq_rep_(self); if (rep->cap) { \
            for (CX##_value_t *p = self->data, *q = p + rep->size; p != q; ++p) \
                valueDel(p); \
            rep->size = 0; \
        } \
    } \
    STC_DEF void \
    CX##_del(CX* self) { \
        CX##_clear(self); \
        if (cdeq_rep_(self)->cap) \
            c_free(cdeq_rep_(self)); \
    } \
\
    STC_DEF void \
    CX##_expand_(CX* self, size_t n, bool at_front) { \
        struct cdeq_rep* rep = cdeq_rep_(self); \
        size_t len = rep->size, cap = rep->cap; \
        size_t nfront = _cdeq_nfront(self), nback = cap - len - nfront; \
        if (at_front && nfront >= n || !at_front && nback >= n) \
            return; \
        if (len*1.2 + n > cap) { \
            cap = (size_t) (len*1.8) + n + 6; \
            rep = (struct cdeq_rep*) c_realloc(rep->cap ? rep : NULL, \
                                               offsetof(struct cdeq_rep, base) + cap*sizeof(Value)); \
            rep->size = len, rep->cap = cap; \
            self->_base = (CX##_value_t *) rep->base; \
            self->data = self->_base + nfront; \
        } \
        size_t unused = cap - (len + n), pos = unused/2; \
        if (!at_front && nfront < pos) return; \
        if (at_front && nback < pos) pos = unused - nback; \
        self->data = (CX##_value_t *) memmove(self->_base + pos, self->data, len*sizeof(Value)); \
    } \
\
    STC_DEF void \
    CX##_resize(CX* self, size_t size, Value null_val) { \
        CX##_expand_(self, size, false); \
        size_t i, n = cdeq_rep_(self)->size; \
        for (i=size; i<n; ++i) valueDel(self->data + i); \
        for (i=n; i<size; ++i) self->data[i] = null_val; \
        if (self->data) cdeq_rep_(self)->size = size; \
    } \
\
    STC_DEF void \
    CX##_push_front(CX* self, Value value) { \
        if (self->data == self->_base) \
            CX##_expand_(self, 1, true); \
        *--self->data = value; \
        ++cdeq_rep_(self)->size; \
    } \
\
    STC_DEF void \
    CX##_push_back(CX* self, Value value) { \
        if (_cdeq_nfront(self) + cdeq_rep_(self)->size == cdeq_rep_(self)->cap) \
            CX##_expand_(self, 1, false); \
        self->data[cdeq_rep_(self)->size++] = value; \
    } \
\
    STC_DEF CX \
    CX##_clone(CX deq) { \
        size_t len = cdeq_rep_(&deq)->size; \
        CX out = CX##_with_capacity(len); \
        CX##_insert_range_p(&out, out.data, deq.data, deq.data + len, true); \
        return out; \
    } \
\
    STC_DEF CX##_iter_t \
    CX##_insert_range_p(CX* self, CX##_value_t* pos, \
                        const CX##_value_t* p1, const CX##_value_t* p2, bool clone) { \
        size_t n = p2 - p1, idx = pos - self->data, size = cdeq_rep_(self)->size; \
        bool at_front = (idx*2 < size); \
        CX##_expand_(self, n, at_front); \
        if (at_front) { \
            memmove(self->data - n, self->data, idx*sizeof(Value)); \
            pos = (self->data -= n) + idx; \
        } else { \
            pos = self->data + idx; \
            memmove(pos + n, pos, (size - idx)*sizeof(Value)); \
        } \
        CX##_iter_t it = {pos}; \
        if (n) cdeq_rep_(self)->size += n; \
        if (clone) while (p1 != p2) *pos++ = valueFromRaw(valueToRaw(p1++)); \
        return it; \
    } \
\
    STC_DEF CX##_iter_t \
    CX##_erase_range_p(CX* self, CX##_value_t* p1, CX##_value_t* p2) { \
        intptr_t len = p2 - p1; \
        if (len > 0) { \
            CX##_value_t* p = p1, *end = self->data + cdeq_rep_(self)->size; \
            while (p != p2) valueDel(p++); \
            if (p1 == self->data) self->data += len; \
            else memmove(p1, p2, (end - p2) * sizeof(Value)); \
            cdeq_rep_(self)->size -= len; \
        } \
        CX##_iter_t it = {p1}; return it; \
    } \
\
    STC_DEF CX##_iter_t \
    CX##_find_in(CX##_iter_t i1, CX##_iter_t i2, RawValue raw) { \
        for (; i1.ref != i2.ref; ++i1.ref) { \
            RawValue r = valueToRaw(i1.ref); \
            if (valueCompareRaw(&raw, &r) == 0) return i1; \
        } \
        return i2; \
    } \
\
    STC_DEF int \
    CX##_value_compare(const CX##_value_t* x, const CX##_value_t* y) { \
        RawValue rx = valueToRaw(x); \
        RawValue ry = valueToRaw(y); \
        return valueCompareRaw(&rx, &ry); \
    }

#else
#define _c_implement_cdeq(CX, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue)
#endif

#endif
