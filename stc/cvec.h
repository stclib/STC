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
#ifndef CVEC_H_INCLUDED
#define CVEC_H_INCLUDED

#include "ccommon.h"
#include <stdlib.h>
#include <string.h>

#define using_cvec(...) c_MACRO_OVERLOAD(using_cvec, __VA_ARGS__)
#define using_cvec_2(X, Value) \
                    using_cvec_3(X, Value, c_default_compare)
#define using_cvec_3(X, Value, valueCompare) \
                    using_cvec_5(X, Value, valueCompare, c_trivial_del, c_trivial_fromraw)
#define using_cvec_4(X, Value, valueCompare, valueDel) \
                    using_cvec_5(X, Value, valueCompare, valueDel, c_no_clone)
#define using_cvec_5(X, Value, valueCompare, valueDel, valueClone) \
                    using_cvec_7(X, Value, valueCompare, valueDel, valueClone, c_trivial_toraw, Value)
#define using_cvec_7(X, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue) \
                    _c_using_cvec(cvec_##X, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue)
#define using_cvec_str() \
                    _c_using_cvec(cvec_str, cstr_t, cstr_compare_raw, cstr_del, cstr_from, cstr_c_str, const char*)

#define typedefs_cvec(CX, Value, RawValue) \
    typedef Value CX##_value_t; \
    typedef RawValue CX##_rawvalue_t; \
    typedef struct { CX##_value_t *ref; } CX##_iter_t; \
    typedef struct { \
        CX##_value_t* data; \
    } CX
    
struct cvec_rep { size_t size, cap; void* data[]; };
#define _cvec_rep(self) c_container_of((self)->data, struct cvec_rep, data)
typedef int (*c_cmp_fn)(const void*, const void*);

#define _c_using_cvec(CX, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue) \
    typedefs_cvec(CX, Value, RawValue); \
\
    STC_API CX \
    CX##_init(void); \
    STC_INLINE size_t \
    CX##_size(CX vec) { return _cvec_rep(&vec)->size; } \
    STC_INLINE size_t \
    CX##_capacity(CX vec) { return _cvec_rep(&vec)->cap; } \
    STC_INLINE bool \
    CX##_empty(CX vec) {return !_cvec_rep(&vec)->size;} \
    STC_INLINE Value \
    CX##_value_fromraw(RawValue raw) {return valueFromRaw(raw);} \
    STC_INLINE CX##_value_t \
    CX##_value_clone(CX##_value_t val) {return valueFromRaw(valueToRaw(&val));} \
    STC_INLINE void \
    CX##_clear(CX* self); \
    STC_API void \
    CX##_del(CX* self); \
    STC_API void \
    CX##_reserve(CX* self, size_t cap); \
    STC_API void \
    CX##_resize(CX* self, size_t size, Value fill_val); \
    STC_INLINE void \
    CX##_swap(CX* a, CX* b) {c_swap(CX, *a, *b);} \
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
        CX##_reserve(&x, size); \
        return x; \
    } \
    STC_API CX \
    CX##_clone(CX vec); \
\
    STC_INLINE void \
    CX##_shrink_to_fit(CX *self) { \
        CX x = CX##_clone(*self); \
        CX##_del(self); *self = x; \
    } \
    STC_API void \
    CX##_emplace_n(CX *self, const CX##_rawvalue_t arr[], size_t size); \
    STC_API void \
    CX##_push_back(CX* self, Value value); \
    STC_INLINE void \
    CX##_emplace_back(CX* self, RawValue raw) { \
        CX##_push_back(self, valueFromRaw(raw)); \
    } \
    STC_INLINE void \
    CX##_pop_back(CX* self) { \
        valueDel(&self->data[--_cvec_rep(self)->size]); \
    } \
\
    STC_API CX##_iter_t \
    CX##_insert_range_p(CX* self, CX##_value_t* pos, const CX##_value_t* pfirst, const CX##_value_t* pfinish); \
\
    STC_INLINE CX##_iter_t \
    CX##_insert_range(CX* self, CX##_iter_t pos, CX##_iter_t first, CX##_iter_t finish) { \
        return CX##_insert_range_p(self, pos.ref, first.ref, finish.ref); \
    } \
    STC_INLINE CX##_iter_t \
    CX##_insert_at(CX* self, CX##_iter_t pos, Value value) { \
        return CX##_insert_range_p(self, pos.ref, &value, &value + 1); \
    } \
    STC_INLINE CX##_iter_t \
    CX##_insert(CX* self, size_t idx, Value value) { \
        return CX##_insert_range_p(self, self->data + idx, &value, &value + 1); \
    } \
    STC_INLINE CX##_iter_t \
    CX##_emplace_at(CX* self, CX##_iter_t pos, RawValue raw) { \
        return CX##_insert_at(self, pos, valueFromRaw(raw)); \
    } \
    STC_INLINE CX##_iter_t \
    CX##_emplace(CX* self, size_t idx, RawValue raw) { \
        return CX##_insert(self, idx, valueFromRaw(raw)); \
    } \
\
    STC_API CX##_iter_t \
    CX##_erase_range_p(CX* self, CX##_value_t* first, CX##_value_t* finish); \
\
    STC_INLINE CX##_iter_t \
    CX##_erase_range(CX* self, CX##_iter_t first, CX##_iter_t finish) { \
        return CX##_erase_range_p(self, first.ref, finish.ref); \
    } \
    STC_INLINE CX##_iter_t \
    CX##_erase_at(CX* self, CX##_iter_t pos) { \
        return CX##_erase_range_p(self, pos.ref, pos.ref + 1); \
    } \
    STC_INLINE CX##_iter_t \
    CX##_erase(CX* self, size_t idx, size_t n) { \
        return CX##_erase_range_p(self, self->data + idx, self->data + idx + n); \
    } \
\
    STC_INLINE CX##_value_t* \
    CX##_front(const CX* self) {return self->data;} \
    STC_INLINE CX##_value_t* \
    CX##_back(const CX* self) {return self->data + _cvec_rep(self)->size - 1;} \
    STC_INLINE CX##_value_t* \
     CX##_at(const CX* self, size_t i) { \
        assert(i < _cvec_rep(self)->size); \
        return self->data + i; \
    } \
\
    STC_INLINE CX##_iter_t \
    CX##_begin(const CX* self) { \
        CX##_iter_t it = {self->data}; return it; \
    } \
    STC_INLINE CX##_iter_t \
    CX##_end(const CX* self) { \
        CX##_iter_t it = {self->data + _cvec_rep(self)->size}; return it; \
    } \
    STC_INLINE void \
    CX##_next(CX##_iter_t* it) {++it->ref;} \
    STC_INLINE size_t \
    CX##_index(CX vec, CX##_iter_t it) {return it.ref - vec.data;} \
\
    STC_API CX##_iter_t \
    CX##_find_in_range(CX##_iter_t first, CX##_iter_t finish, RawValue raw); \
    STC_INLINE CX##_iter_t \
    CX##_find(const CX* self, RawValue raw) { \
        return CX##_find_in_range(CX##_begin(self), CX##_end(self), raw); \
    } \
    STC_API int \
    CX##_value_compare(const CX##_value_t* x, const CX##_value_t* y); \
    STC_API CX##_iter_t \
    CX##_bsearch_in_range(CX##_iter_t i1, CX##_iter_t i2, RawValue raw); \
    STC_INLINE CX##_iter_t \
    CX##_bsearch(const CX* self, RawValue raw) { \
        return CX##_bsearch_in_range(CX##_begin(self), CX##_end(self), raw); \
    } \
    STC_INLINE void \
    CX##_sort_range(CX##_iter_t i1, CX##_iter_t i2, \
                          int(*cmp)(const CX##_value_t*, const CX##_value_t*)) { \
        qsort(i1.ref, i2.ref - i1.ref, sizeof(CX##_value_t), (c_cmp_fn) cmp); \
    } \
    STC_INLINE void \
    CX##_sort(CX* self) { \
        CX##_sort_range(CX##_begin(self), CX##_end(self), CX##_value_compare); \
    } \
\
    _c_implement_cvec(CX, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue) \
    typedef CX CX##_t

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
static struct cvec_rep _cvec_inits = {0, 0};

#define _c_implement_cvec(CX, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue) \
\
    STC_DEF CX \
    CX##_init(void) { \
        CX vec = {(CX##_value_t *) _cvec_inits.data}; return vec; \
    } \
\
    STC_DEF void \
    CX##_emplace_n(CX *self, const CX##_rawvalue_t arr[], size_t n) { \
        if (!n) return; \
        CX##_reserve(self, _cvec_rep(self)->size + n); \
        CX##_value_t* p = self->data + _cvec_rep(self)->size; \
        for (size_t i=0; i < n; ++i) *p++ = valueFromRaw(arr[i]); \
        _cvec_rep(self)->size += n; \
    } \
\
    STC_DEF void \
    CX##_clear(CX* self) { \
        struct cvec_rep* rep = _cvec_rep(self); if (rep->cap) { \
            for (CX##_value_t *p = self->data, *q = p + rep->size; p != q; ++p) \
                valueDel(p); \
            rep->size = 0; \
        } \
    } \
    STC_DEF void \
    CX##_del(CX* self) { \
        CX##_clear(self); \
        if (_cvec_rep(self)->cap) \
            c_free(_cvec_rep(self)); \
    } \
\
    STC_DEF void \
    CX##_reserve(CX* self, size_t cap) { \
        struct cvec_rep* rep = _cvec_rep(self); \
        size_t len = rep->size, oldcap = rep->cap; \
        if (cap > oldcap) { \
            rep = (struct cvec_rep*) c_realloc(oldcap ? rep : NULL, \
                                               sizeof(struct cvec_rep) + cap*sizeof(Value)); \
            self->data = (CX##_value_t*) rep->data; \
            rep->size = len; \
            rep->cap = cap; \
        } \
    } \
    STC_DEF void \
    CX##_resize(CX* self, size_t len, Value null_val) { \
        CX##_reserve(self, len); \
        struct cvec_rep* rep = _cvec_rep(self); \
        size_t i, n = rep->size; \
        for (i = len; i < n; ++i) valueDel(self->data + i); \
        for (i = n; i < len; ++i) self->data[i] = null_val; \
        if (rep->cap) rep->size = len; \
    } \
\
    STC_DEF void \
    CX##_push_back(CX* self, Value value) { \
        size_t len = _cvec_rep(self)->size; \
        if (len == CX##_capacity(*self)) \
            CX##_reserve(self, 4 + len*1.5); \
        self->data[_cvec_rep(self)->size++] = value; \
    } \
\
    STC_DEF CX \
    CX##_clone(CX vec) { \
        size_t len = _cvec_rep(&vec)->size; \
        CX out = CX##_with_capacity(len); \
        CX##_insert_range_p(&out, out.data, vec.data, vec.data + len); \
        return out; \
    } \
\
    STC_DEF CX##_iter_t \
    CX##_insert_range_p(CX* self, CX##_value_t* pos, const CX##_value_t* first, const CX##_value_t* finish) { \
        size_t len = finish - first, idx = pos - self->data, size = _cvec_rep(self)->size; \
        CX##_iter_t it = {pos}; \
        if (len == 0) return it; \
        if (size + len > CX##_capacity(*self)) \
            CX##_reserve(self, 4 + (size + len)*1.5), \
            it.ref = pos = self->data + idx; \
        _cvec_rep(self)->size += len; \
        memmove(pos + len, pos, (size - idx) * sizeof(Value)); \
        while (first != finish) \
            *pos++ = valueFromRaw(valueToRaw(first++)); \
        return it; \
    } \
\
    STC_DEF CX##_iter_t \
    CX##_erase_range_p(CX* self, CX##_value_t* first, CX##_value_t* finish) { \
        intptr_t len = finish - first; \
        if (len > 0) { \
            CX##_value_t* p = first, *end = self->data + _cvec_rep(self)->size; \
            while (p != finish) valueDel(p++); \
            memmove(first, finish, (end - finish) * sizeof(Value)); \
            _cvec_rep(self)->size -= len; \
        } \
        CX##_iter_t it = {first}; return it; \
    } \
\
    STC_DEF CX##_iter_t \
    CX##_find_in_range(CX##_iter_t i1, CX##_iter_t i2, RawValue raw) { \
        for (; i1.ref != i2.ref; ++i1.ref) { \
            RawValue r = valueToRaw(i1.ref); \
            if (valueCompareRaw(&raw, &r) == 0) return i1; \
        } \
        return i2; \
    } \
    STC_DEF CX##_iter_t \
    CX##_bsearch_in_range(CX##_iter_t i1, CX##_iter_t i2, RawValue raw) { \
        CX##_iter_t mid, last = i2; \
        while (i1.ref != i2.ref) { \
            mid.ref = i1.ref + ((i2.ref - i1.ref)>>1); \
            int c; RawValue m = valueToRaw(mid.ref); \
            if ((c = valueCompareRaw(&raw, &m)) == 0) return mid; \
            else if (c < 0) i2.ref = mid.ref; \
            else i1.ref = mid.ref + 1; \
        } \
        return last; \
    } \
\
    STC_DEF int \
    CX##_value_compare(const CX##_value_t* x, const CX##_value_t* y) { \
        RawValue rx = valueToRaw(x); \
        RawValue ry = valueToRaw(y); \
        return valueCompareRaw(&rx, &ry); \
    }

#else
#define _c_implement_cvec(CX, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue)
#endif

#endif
