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
#ifndef CVEC__H__
#define CVEC__H__

#include "ccommon.h"
#include <stdlib.h>
#include <string.h>

#define using_cvec(...) c_MACRO_OVERLOAD(using_cvec, __VA_ARGS__)
#define using_cvec_2(X, Value) \
                    using_cvec_3(X, Value, c_default_compare)
#define using_cvec_3(X, Value, valueCompare) \
                    using_cvec_7(X, Value, valueCompare, c_default_del, c_default_fromraw, c_default_toraw, Value)
#define using_cvec_4(X, Value, valueCompare, valueDestroy) \
                    using_cvec_7(X, Value, valueCompare, valueDestroy, c_no_fromraw, c_default_toraw, Value)
#define using_cvec_str() \
                    using_cvec_7(str, cstr_t, cstr_compare_raw, cstr_del, cstr_from, cstr_c_str, const char*)

#define typedefs_cvec(X, Value, RawValue) \
    typedef Value cvec_##X##_value_t; \
    typedef RawValue cvec_##X##_rawvalue_t; \
    typedef struct { cvec_##X##_value_t *ref; } cvec_##X##_iter_t; \
    typedef struct { \
        cvec_##X##_value_t* data; \
    } cvec_##X
    
struct cvec_rep { size_t size, cap; void* data[]; };
#define _cvec_rep(self) c_container_of((self)->data, struct cvec_rep, data)

#define using_cvec_7(X, Value, valueCompareRaw, valueDestroy, valueFromRaw, valueToRaw, RawValue) \
    typedefs_cvec(X, Value, RawValue); \
\
    STC_API cvec_##X \
    cvec_##X##_init(void); \
    STC_INLINE size_t \
    cvec_##X##_size(cvec_##X vec) { return _cvec_rep(&vec)->size; } \
    STC_INLINE size_t \
    cvec_##X##_capacity(cvec_##X vec) { return _cvec_rep(&vec)->cap; } \
    STC_INLINE bool \
    cvec_##X##_empty(cvec_##X vec) {return !_cvec_rep(&vec)->size;} \
    STC_INLINE Value \
    cvec_##X##_value_from_raw(RawValue raw) {return valueFromRaw(raw);} \
    STC_INLINE cvec_##X##_value_t \
    cvec_##X##_value_clone(cvec_##X##_value_t val) {return valueFromRaw(valueToRaw(&val));} \
    STC_INLINE void \
    cvec_##X##_clear(cvec_##X* self); \
    STC_API void \
    cvec_##X##_del(cvec_##X* self); \
    STC_API void \
    cvec_##X##_reserve(cvec_##X* self, size_t cap); \
    STC_API void \
    cvec_##X##_resize(cvec_##X* self, size_t size, Value fill_val); \
    STC_INLINE void \
    cvec_##X##_swap(cvec_##X* a, cvec_##X* b) {c_swap(cvec_##X##_value_t*, a->data, b->data);} \
\
    STC_INLINE cvec_##X \
    cvec_##X##_with_size(size_t size, Value null_val) { \
        cvec_##X x = cvec_##X##_init(); \
        cvec_##X##_resize(&x, size, null_val); \
        return x; \
    } \
    STC_INLINE cvec_##X \
    cvec_##X##_with_capacity(size_t size) { \
        cvec_##X x = cvec_##X##_init(); \
        cvec_##X##_reserve(&x, size); \
        return x; \
    } \
    STC_API cvec_##X \
    cvec_##X##_clone(cvec_##X vec); \
\
    STC_INLINE void \
    cvec_##X##_shrink_to_fit(cvec_##X *self) { \
        cvec_##X x = cvec_##X##_clone(*self); \
        cvec_##X##_del(self); *self = x; \
    } \
    STC_API void \
    cvec_##X##_push_n(cvec_##X *self, const cvec_##X##_rawvalue_t arr[], size_t size); \
    STC_API void \
    cvec_##X##_push_back(cvec_##X* self, Value value); \
    STC_INLINE void \
    cvec_##X##_emplace_back(cvec_##X* self, RawValue raw) { \
        cvec_##X##_push_back(self, valueFromRaw(raw)); \
    } \
    STC_INLINE void \
    cvec_##X##_pop_back(cvec_##X* self) { \
        valueDestroy(&self->data[--_cvec_rep(self)->size]); \
    } \
\
    STC_API cvec_##X##_iter_t \
    cvec_##X##_insert_range_p(cvec_##X* self, cvec_##X##_value_t* pos, const cvec_##X##_value_t* pfirst, const cvec_##X##_value_t* pfinish); \
\
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_insert_range(cvec_##X* self, cvec_##X##_iter_t pos, cvec_##X##_iter_t first, cvec_##X##_iter_t finish) { \
        return cvec_##X##_insert_range_p(self, pos.ref, first.ref, finish.ref); \
    } \
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_insert_at(cvec_##X* self, cvec_##X##_iter_t pos, Value value) { \
        return cvec_##X##_insert_range_p(self, pos.ref, &value, &value + 1); \
    } \
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_insert(cvec_##X* self, size_t idx, Value value) { \
        return cvec_##X##_insert_range_p(self, self->data + idx, &value, &value + 1); \
    } \
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_emplace_at(cvec_##X* self, cvec_##X##_iter_t pos, RawValue raw) { \
        return cvec_##X##_insert_at(self, pos, valueFromRaw(raw)); \
    } \
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_emplace(cvec_##X* self, size_t idx, RawValue raw) { \
        return cvec_##X##_insert(self, idx, valueFromRaw(raw)); \
    } \
\
    STC_API cvec_##X##_iter_t \
    cvec_##X##_erase_range_p(cvec_##X* self, cvec_##X##_value_t* first, cvec_##X##_value_t* finish); \
\
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_erase_range(cvec_##X* self, cvec_##X##_iter_t first, cvec_##X##_iter_t finish) { \
        return cvec_##X##_erase_range_p(self, first.ref, finish.ref); \
    } \
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_erase_at(cvec_##X* self, cvec_##X##_iter_t pos) { \
        return cvec_##X##_erase_range_p(self, pos.ref, pos.ref + 1); \
    } \
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_erase(cvec_##X* self, size_t idx, size_t n) { \
        return cvec_##X##_erase_range_p(self, self->data + idx, self->data + idx + n); \
    } \
\
    STC_API cvec_##X##_iter_t \
    cvec_##X##_find(const cvec_##X* self, RawValue raw); \
    STC_API cvec_##X##_iter_t \
    cvec_##X##_find_in_range(const cvec_##X* self, cvec_##X##_iter_t first, cvec_##X##_iter_t finish, RawValue raw); \
\
    STC_INLINE cvec_##X##_value_t* \
    cvec_##X##_front(cvec_##X* self) {return self->data;} \
    STC_INLINE cvec_##X##_value_t* \
    cvec_##X##_back(cvec_##X* self) {return self->data + _cvec_rep(self)->size - 1;} \
    STC_INLINE cvec_##X##_value_t* \
     cvec_##X##_at(cvec_##X* self, size_t i) { \
        assert(i < _cvec_rep(self)->size); \
        return self->data + i; \
    } \
\
    STC_API int \
    cvec_##X##_value_compare(const cvec_##X##_value_t* x, const cvec_##X##_value_t* y); \
    STC_INLINE void \
    cvec_##X##_sort_with(cvec_##X* self, size_t ifirst, size_t ifinish, int(*cmp)(const cvec_##X##_value_t*, const cvec_##X##_value_t*)) { \
        qsort(self->data + ifirst, ifinish - ifirst, sizeof(Value), (_cvec_cmp) cmp); \
    } \
    STC_INLINE void \
    cvec_##X##_sort(cvec_##X* self) { \
        cvec_##X##_sort_with(self, 0, _cvec_rep(self)->size, cvec_##X##_value_compare); \
    } \
\
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_begin(const cvec_##X* self) { \
        cvec_##X##_iter_t it = {self->data}; return it; \
    } \
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_end(const cvec_##X* self) { \
        cvec_##X##_iter_t it = {self->data + _cvec_rep(self)->size}; return it; \
    } \
    STC_INLINE void \
    cvec_##X##_next(cvec_##X##_iter_t* it) {++it->ref;} \
    STC_INLINE cvec_##X##_value_t* \
    cvec_##X##_itval(cvec_##X##_iter_t it) {return it.ref;} \
    STC_INLINE size_t \
    cvec_##X##_index(cvec_##X vec, cvec_##X##_iter_t it) {return it.ref - vec.data;} \
\
    _c_implement_cvec_7(X, Value, valueCompareRaw, valueDestroy, valueFromRaw, valueToRaw, RawValue) \
    typedef cvec_##X cvec_##X##_t

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
static struct cvec_rep _cvec_inits = {0, 0};

#define _c_implement_cvec_7(X, Value, valueCompareRaw, valueDestroy, valueFromRaw, valueToRaw, RawValue) \
\
    STC_DEF cvec_##X \
    cvec_##X##_init(void) { \
        cvec_##X vec = {(cvec_##X##_value_t *) _cvec_inits.data}; return vec; \
    } \
\
    STC_DEF void \
    cvec_##X##_push_n(cvec_##X *self, const cvec_##X##_rawvalue_t arr[], size_t n) { \
        if (!n) return; \
        cvec_##X##_reserve(self, _cvec_rep(self)->size + n); \
        cvec_##X##_value_t* p = self->data + _cvec_rep(self)->size; \
        for (size_t i=0; i < n; ++i) *p++ = valueFromRaw(arr[i]); \
        _cvec_rep(self)->size += n; \
    } \
\
    STC_DEF void \
    cvec_##X##_clear(cvec_##X* self) { \
        struct cvec_rep* rep = _cvec_rep(self); if (rep->cap) { \
            for (cvec_##X##_value_t *p = self->data, *q = p + rep->size; p != q; ++p) \
                valueDestroy(p); \
            rep->size = 0; \
        } \
    } \
    STC_DEF void \
    cvec_##X##_del(cvec_##X* self) { \
        cvec_##X##_clear(self); \
        if (_cvec_rep(self)->cap) \
            c_free(_cvec_rep(self)); \
    } \
\
    STC_DEF void \
    cvec_##X##_reserve(cvec_##X* self, size_t cap) { \
        struct cvec_rep* rep = _cvec_rep(self); \
        size_t len = rep->size, oldcap = rep->cap; \
        if (cap > oldcap) { \
            rep = (struct cvec_rep*) c_realloc(oldcap ? rep : NULL, \
                                               sizeof(struct cvec_rep) + cap*sizeof(Value)); \
            self->data = (cvec_##X##_value_t*) rep->data; \
            rep->size = len; \
            rep->cap = cap; \
        } \
    } \
    STC_DEF void \
    cvec_##X##_resize(cvec_##X* self, size_t len, Value null_val) { \
        cvec_##X##_reserve(self, len); \
        struct cvec_rep* rep = _cvec_rep(self); \
        size_t i, n = rep->size; \
        for (i = len; i < n; ++i) valueDestroy(self->data + i); \
        for (i = n; i < len; ++i) self->data[i] = null_val; \
        if (rep->cap) rep->size = len; \
    } \
\
    STC_DEF void \
    cvec_##X##_push_back(cvec_##X* self, Value value) { \
        size_t len = _cvec_rep(self)->size; \
        if (len == cvec_##X##_capacity(*self)) \
            cvec_##X##_reserve(self, 4 + len*3/2); \
        self->data[_cvec_rep(self)->size++] = value; \
    } \
\
    STC_DEF cvec_##X \
    cvec_##X##_clone(cvec_##X vec) { \
        size_t len = _cvec_rep(&vec)->size; \
        cvec_##X out = cvec_##X##_with_capacity(len); \
        cvec_##X##_insert_range_p(&out, out.data, vec.data, vec.data + len); \
        return out; \
    } \
\
    STC_DEF cvec_##X##_iter_t \
    cvec_##X##_insert_range_p(cvec_##X* self, cvec_##X##_value_t* pos, const cvec_##X##_value_t* first, const cvec_##X##_value_t* finish) { \
        size_t len = finish - first, idx = pos - self->data, size = _cvec_rep(self)->size; \
        cvec_##X##_iter_t it = {pos}; \
        if (len == 0) return it; \
        if (size + len > cvec_##X##_capacity(*self)) \
            cvec_##X##_reserve(self, 4 + (size + len)*3/2), \
            it.ref = pos = self->data + idx; \
        _cvec_rep(self)->size += len; \
        memmove(pos + len, pos, (size - idx) * sizeof(Value)); \
        while (first != finish) \
            *pos++ = valueFromRaw(valueToRaw(first++)); \
        return it; \
    } \
\
    STC_DEF cvec_##X##_iter_t \
    cvec_##X##_erase_range_p(cvec_##X* self, cvec_##X##_value_t* first, cvec_##X##_value_t* finish) { \
        intptr_t len = finish - first; \
        if (len > 0) { \
            cvec_##X##_value_t* p = first, *end = self->data + _cvec_rep(self)->size; \
            while (p != finish) valueDestroy(p++); \
            memmove(first, finish, (end - finish) * sizeof(Value)); \
            _cvec_rep(self)->size -= len; \
        } \
        cvec_##X##_iter_t it = {first}; return it; \
    } \
\
    STC_DEF cvec_##X##_iter_t \
    cvec_##X##_find_in_range(const cvec_##X* self, cvec_##X##_iter_t first, cvec_##X##_iter_t finish, RawValue raw) { \
        for (; first.ref != finish.ref; cvec_##X##_next(&first)) { \
            RawValue r = valueToRaw(first.ref); \
            if (valueCompareRaw(&r, &raw) == 0) return first; \
        } \
        return cvec_##X##_end(self); \
    } \
    STC_DEF cvec_##X##_iter_t \
    cvec_##X##_find(const cvec_##X* self, RawValue raw) { \
        return cvec_##X##_find_in_range(self, cvec_##X##_begin(self), cvec_##X##_end(self), raw); \
    } \
\
    STC_DEF int \
    cvec_##X##_value_compare(const cvec_##X##_value_t* x, const cvec_##X##_value_t* y) { \
        RawValue rx = valueToRaw(x); \
        RawValue ry = valueToRaw(y); \
        return valueCompareRaw(&rx, &ry); \
    }

#else
#define _c_implement_cvec_7(X, Value, valueCompareRaw, valueDestroy, valueFromRaw, valueToRaw, RawValue)
#endif

#if defined(_WIN32) && defined(_DLL)
#define STC_EXTERN_IMPORT extern __declspec(dllimport)
#else
#define STC_EXTERN_IMPORT extern
#endif
typedef int(*_cvec_cmp)(const void*, const void*);
STC_EXTERN_IMPORT void qsort(void *base, size_t nitems, size_t size, _cvec_cmp cmp);

#endif
