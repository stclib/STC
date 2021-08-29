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

#define forward_cdeq(X, Value) _c_cdeq_types(cdeq_##X, Value)

#define using_cdeq(...) c_MACRO_OVERLOAD(using_cdeq, __VA_ARGS__)

#define using_cdeq_2(X, Value) \
            using_cdeq_3(X, Value, c_default_compare)
#define using_cdeq_3(X, Value, valueCompare) \
            using_cdeq_5(X, Value, valueCompare, c_default_del, c_default_fromraw)
#define using_cdeq_4(X, Value, valueCompare, valueDel) \
            using_cdeq_5(X, Value, valueCompare, valueDel, c_no_clone)
#define using_cdeq_5(X, Value, valueCompare, valueDel, valueClone) \
            using_cdeq_7(X, Value, valueCompare, valueDel, valueClone, c_default_toraw, Value)
#define using_cdeq_7(X, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue) \
            _c_using_cdeq(cdeq_##X, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue, c_true)
#define using_cdeq_8(X, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue, defTypes) \
            _c_using_cdeq(cdeq_##X, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue, defTypes)

#define using_cdeq_str() \
            using_cdeq_7(str, cstr, c_rawstr_compare, cstr_del, cstr_from, cstr_str, const char*)


struct cdeq_rep { size_t size, cap; void* base[]; };
#define _cdeq_rep(self) c_container_of((self)->_base, struct cdeq_rep, base)

#define _c_cdeq_types(CX, Value) \
    typedef Value CX##_value_t; \
    typedef struct {CX##_value_t *ref; } CX##_iter_t; \
    typedef struct {CX##_value_t *_base, *data;} CX

#define _c_using_cdeq(CX, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue, defTypes) \
\
    defTypes( _c_cdeq_types(CX, Value); ) \
    typedef RawValue CX##_rawvalue_t; \
\
    STC_API CX               CX##_init(void); \
    STC_API CX               CX##_clone(CX cx); \
    STC_API void             CX##_clear(CX* self); \
    STC_API void             CX##_del(CX* self); \
    STC_API CX##_iter_t      CX##_find_in(CX##_iter_t p1, CX##_iter_t p2, RawValue raw); \
    STC_API int              CX##_value_compare(const CX##_value_t* x, const CX##_value_t* y); \
    STC_API void             CX##_push_back(CX* self, Value value); \
    STC_API void             CX##_push_front(CX* self, Value value); \
    STC_API CX##_iter_t      CX##_erase_range_p(CX* self, CX##_value_t* p1, CX##_value_t* p2); \
    STC_API CX##_iter_t      CX##_insert_range_p(CX* self, CX##_value_t* pos, \
                                                 const CX##_value_t* p1, const CX##_value_t* p2, bool clone); \
    STC_API CX##_iter_t      CX##_emplace_range_p(CX* self, CX##_value_t* pos, \
                                             const CX##_rawvalue_t* p1, const CX##_rawvalue_t* p2); \
    STC_API void             CX##_expand_right_(CX* self, size_t idx, size_t n); \
\
    STC_INLINE bool          CX##_empty(CX cx) {return !_cdeq_rep(&cx)->size;} \
    STC_INLINE size_t        CX##_size(CX cx) {return _cdeq_rep(&cx)->size;} \
    STC_INLINE size_t        CX##_capacity(CX cx) {return _cdeq_rep(&cx)->cap;} \
    STC_INLINE void          CX##_swap(CX* a, CX* b) {c_swap(CX, *a, *b);} \
    STC_INLINE Value         CX##_value_fromraw(RawValue raw) {return valueFromRaw(raw);} \
    STC_INLINE RawValue      CX##_value_toraw(CX##_value_t* pval) {return valueToRaw(pval);} \
    STC_INLINE Value         CX##_value_clone(Value val) \
                                {return valueFromRaw(valueToRaw(&val));} \
    STC_INLINE void          CX##_emplace_back(CX* self, RawValue raw) \
                                {CX##_push_back(self, valueFromRaw(raw));} \
    STC_INLINE void          CX##_emplace_front(CX* self, RawValue raw) \
                                {CX##_push_front(self, valueFromRaw(raw));} \
    STC_INLINE void          CX##_pop_back(CX* self) \
                                {valueDel(&self->data[--_cdeq_rep(self)->size]);} \
    STC_INLINE void          CX##_pop_front(CX* self) \
                                {valueDel(self->data++); --_cdeq_rep(self)->size;} \
    STC_INLINE CX##_value_t* CX##_front(const CX* self) {return self->data;} \
    STC_INLINE CX##_value_t* CX##_back(const CX* self) \
                                {return self->data + _cdeq_rep(self)->size - 1;} \
    STC_INLINE CX##_value_t* CX##_at(const CX* self, size_t idx) \
                                {assert(idx < _cdeq_rep(self)->size); return self->data + idx;} \
    STC_INLINE CX##_iter_t   CX##_begin(const CX* self) \
                                {return c_make(CX##_iter_t){self->data};} \
    STC_INLINE CX##_iter_t   CX##_end(const CX* self) \
                                {return c_make(CX##_iter_t){self->data + _cdeq_rep(self)->size};} \
    STC_INLINE void          CX##_next(CX##_iter_t* it) {++it->ref;} \
    STC_INLINE CX##_iter_t   CX##_adv(CX##_iter_t it, intptr_t offs) {it.ref += offs; return it;} \
    STC_INLINE size_t        CX##_idx(CX cx, CX##_iter_t it) {return it.ref - cx.data;} \
\
    STC_INLINE CX \
    CX##_with_capacity(size_t n) { \
        CX cx = CX##_init(); \
        CX##_expand_right_(&cx, 0, n); \
        return cx; \
    } \
\
    STC_INLINE void \
    CX##_reserve(CX* self, size_t n) { \
        size_t sz = _cdeq_rep(self)->size; \
        if (n > sz) CX##_expand_right_(self, sz, n - sz); \
    } \
\
    STC_INLINE void \
    CX##_shrink_to_fit(CX *self) { \
        CX cx = CX##_clone(*self); \
        CX##_del(self); *self = cx; \
    } \
\
    STC_INLINE CX##_iter_t \
    CX##_insert(CX* self, size_t idx, Value value) { \
        return CX##_insert_range_p(self, self->data + idx, &value, &value + 1, false); \
    } \
    STC_INLINE CX##_iter_t \
    CX##_insert_n(CX* self, size_t idx, const CX##_value_t arr[], size_t n) { \
        return CX##_insert_range_p(self, self->data + idx, arr, arr + n, false); \
    } \
    STC_INLINE CX##_iter_t \
    CX##_insert_at(CX* self, CX##_iter_t it, Value value) { \
        return CX##_insert_range_p(self, it.ref, &value, &value + 1, false); \
    } \
\
    STC_INLINE CX##_iter_t \
    CX##_emplace(CX* self, size_t idx, RawValue raw) { \
        return CX##_emplace_range_p(self, self->data + idx, &raw, &raw + 1); \
    } \
    STC_INLINE CX##_iter_t \
    CX##_emplace_n(CX* self, size_t idx, const CX##_rawvalue_t arr[], size_t n) { \
        return CX##_emplace_range_p(self, self->data + idx, arr, arr + n); \
    } \
    STC_INLINE CX##_iter_t \
    CX##_emplace_at(CX* self, CX##_iter_t it, RawValue raw) { \
        return CX##_emplace_range_p(self, it.ref, &raw, &raw + 1); \
    } \
    STC_INLINE CX##_iter_t \
    CX##_emplace_range(CX* self, CX##_iter_t it, CX##_iter_t it1, CX##_iter_t it2) { \
        return CX##_insert_range_p(self, it.ref, it1.ref, it2.ref, true); \
    } \
    STC_INLINE void \
    CX##_emplace_items(CX *self, const CX##_rawvalue_t arr[], size_t n) { \
        CX##_emplace_range_p(self, self->data + _cdeq_rep(self)->size, arr, arr + n); \
    } \
\
    STC_INLINE CX##_iter_t \
    CX##_erase(CX* self, size_t idx) { \
        return CX##_erase_range_p(self, self->data + idx, self->data + idx + 1); \
    } \
    STC_INLINE CX##_iter_t \
    CX##_erase_n(CX* self, size_t idx, size_t n) { \
        return CX##_erase_range_p(self, self->data + idx, self->data + idx + n); \
    } \
    STC_INLINE CX##_iter_t \
    CX##_erase_at(CX* self, CX##_iter_t it) { \
        return CX##_erase_range_p(self, it.ref, it.ref + 1); \
    } \
    STC_INLINE CX##_iter_t \
    CX##_erase_range(CX* self, CX##_iter_t it1, CX##_iter_t it2) { \
        return CX##_erase_range_p(self, it1.ref, it2.ref); \
    } \
\
    STC_INLINE CX##_iter_t \
    CX##_find(const CX* self, RawValue raw) { \
        return CX##_find_in(CX##_begin(self), CX##_end(self), raw); \
    } \
\
    STC_INLINE CX##_value_t* \
    CX##_get(const CX* self, RawValue raw) { \
        CX##_iter_t end = CX##_end(self); \
        CX##_value_t* val = CX##_find_in(CX##_begin(self), end, raw).ref; \
        return val == end.ref ? NULL : val; \
    } \
\
    STC_INLINE void \
    CX##_sort_range(CX##_iter_t i1, CX##_iter_t i2, \
                    int(*_cmp_)(const CX##_value_t*, const CX##_value_t*)) { \
        qsort(i1.ref, i2.ref - i1.ref, sizeof *i1.ref, (int(*)(const void*, const void*)) _cmp_); \
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

static struct cdeq_rep _cdeq_sentinel = {0, 0};
#define _cdeq_nfront(self) ((self)->data - (self)->_base)

#define _c_implement_cdeq(CX, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue) \
\
    STC_DEF CX \
    CX##_init(void) { \
        CX##_value_t *b = (CX##_value_t *) _cdeq_sentinel.base; \
        return c_make(CX){b, b}; \
    } \
\
    STC_DEF void \
    CX##_clear(CX* self) { \
        struct cdeq_rep* rep = _cdeq_rep(self); if (rep->cap) { \
            for (CX##_value_t *p = self->data, *q = p + rep->size; p != q; ++p) \
                valueDel(p); \
            rep->size = 0; \
        } \
    } \
\
    STC_DEF void \
    CX##_del(CX* self) { \
        CX##_clear(self); \
        if (_cdeq_rep(self)->cap) \
            c_free(_cdeq_rep(self)); \
    } \
\
    STC_DEF size_t \
    CX##_realloc_(CX* self, size_t n) { \
        struct cdeq_rep* rep = _cdeq_rep(self); \
        size_t sz = rep->size, cap = (size_t) (sz*1.7) + n + 7; \
        size_t nfront = _cdeq_nfront(self); \
        rep = (struct cdeq_rep*) c_realloc(rep->cap ? rep : NULL, \
                                           offsetof(struct cdeq_rep, base) + cap*sizeof(Value)); \
        rep->size = sz, rep->cap = cap; \
        self->_base = (CX##_value_t *) rep->base; \
        self->data = self->_base + nfront; \
        return cap; \
    } \
\
    STC_DEF void \
    CX##_expand_left_(CX* self, size_t idx, size_t n) { \
        struct cdeq_rep* rep = _cdeq_rep(self); \
        size_t sz = rep->size, cap = rep->cap; \
        size_t nfront = _cdeq_nfront(self), nback = cap - sz - nfront; \
        if (nfront >= n) { \
            self->data = (CX##_value_t *) memmove(self->data - n, self->data, idx*sizeof(Value)); \
        } else { \
            if (sz*1.3 + n > cap) cap = CX##_realloc_(self, n); \
            size_t unused = cap - (sz + n); \
            size_t pos = (nback*2 < unused) ? unused - nback : unused/2; \
            memmove(self->_base + pos + idx + n, self->data + idx, (sz - idx)*sizeof(Value)); \
            self->data = (CX##_value_t *) memmove(self->_base + pos, self->data, idx*sizeof(Value)); \
        } \
    } \
\
    STC_DEF void \
    CX##_expand_right_(CX* self, size_t idx, size_t n) { \
        struct cdeq_rep* rep = _cdeq_rep(self); \
        size_t sz = rep->size, cap = rep->cap; \
        size_t nfront = _cdeq_nfront(self), nback = cap - sz - nfront; \
        if (nback >= n || sz*1.3 + n > cap && CX##_realloc_(self, n)) { \
            memmove(self->data + idx + n, self->data + idx, (sz - idx)*sizeof(Value)); \
        } else { \
            size_t unused = cap - (sz + n); \
            size_t pos = (nfront*2 < unused) ? nfront : unused/2; \
            memmove(self->_base + pos, self->data, idx*sizeof(Value)); \
            memmove(self->data + pos + idx + n, self->data + idx, (sz - idx)*sizeof(Value)); \
            self->data = ((CX##_value_t *) self->_base) + pos; \
        } \
    } \
\
    STC_DEF CX##_value_t* \
    CX##_insert_space_(CX* self, CX##_value_t* pos, size_t n) { \
        size_t idx = pos - self->data; \
        if (idx*2 < _cdeq_rep(self)->size) CX##_expand_left_(self, idx, n); \
        else                               CX##_expand_right_(self, idx, n); \
        if (n) _cdeq_rep(self)->size += n; /* do only if size > 0 */ \
        return self->data + idx; \
    } \
\
    STC_DEF void \
    CX##_push_front(CX* self, Value value) { \
        if (self->data == self->_base) \
            CX##_expand_left_(self, 0, 1); \
        else \
            --self->data; \
        *self->data = value; \
        ++_cdeq_rep(self)->size; \
    } \
\
    STC_DEF void \
    CX##_push_back(CX* self, Value value) { \
        struct cdeq_rep* rep = _cdeq_rep(self); \
        if (_cdeq_nfront(self) + rep->size == rep->cap) \
            CX##_expand_right_(self, rep->size, 1); \
        self->data[_cdeq_rep(self)->size++] = value; \
    } \
\
    STC_DEF CX \
    CX##_clone(CX cx) { \
        size_t sz = _cdeq_rep(&cx)->size; \
        CX out = CX##_with_capacity(sz); \
        CX##_insert_range_p(&out, out.data, cx.data, cx.data + sz, true); \
        return out; \
    } \
\
    STC_DEF CX##_iter_t \
    CX##_insert_range_p(CX* self, CX##_value_t* pos, const CX##_value_t* p1, \
                                                     const CX##_value_t* p2, bool clone) { \
        pos = CX##_insert_space_(self, pos, p2 - p1); \
        CX##_iter_t it = {pos}; \
        if (clone) while (p1 != p2) *pos++ = valueFromRaw(valueToRaw(p1++)); \
        else memcpy(pos, p1, (p2 - p1)*sizeof *p1); \
        return it; \
    } \
\
    STC_DEF CX##_iter_t \
    CX##_emplace_range_p(CX* self, CX##_value_t* pos, const CX##_rawvalue_t* p1, const CX##_rawvalue_t* p2) { \
        pos = CX##_insert_space_(self, pos, p2 - p1); \
        CX##_iter_t it = {pos}; \
        while (p1 != p2) *pos++ = valueFromRaw(*p1++); \
        return it; \
    } \
\
    STC_DEF CX##_iter_t \
    CX##_erase_range_p(CX* self, CX##_value_t* p1, CX##_value_t* p2) { \
        size_t n = p2 - p1; \
        if (n > 0) { \
            CX##_value_t* p = p1, *end = self->data + _cdeq_rep(self)->size; \
            while (p != p2) valueDel(p++); \
            if (p1 == self->data) self->data += n; \
            else memmove(p1, p2, (end - p2) * sizeof(Value)); \
            _cdeq_rep(self)->size -= n; \
        } \
        return c_make(CX##_iter_t){p1}; \
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
