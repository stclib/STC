/* MIT License
 *
 * Copyright (c) 2020 Tyge Løvset, NORCE, www.norceresearch.no
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

#define cvec__init         {NULL}
#define cvec_size(v)       _cvec_safe_size((v).data)
#define cvec_capacity(v)   _cvec_safe_capacity((v).data)
#define cvec_empty(v)      (cvec_size(v) == 0)

#define using_cvec(...) c_MACRO_OVERLOAD(using_cvec, __VA_ARGS__)
#define using_cvec_2(X, Value) \
                    using_cvec_3(X, Value, c_default_del)
#define using_cvec_3(X, Value, valueDestroy) \
                    using_cvec_4(X, Value, valueDestroy, c_default_compare)
#define using_cvec_4(X, Value, valueDestroy, valueCompare) \
                    using_cvec_7(X, Value, valueDestroy, valueCompare, Value, c_default_to_raw, c_default_from_raw)
#define using_cvec_str() \
                    using_cvec_7(str, cstr_t, cstr_del, cstr_compare_raw, const char*, cstr_to_raw, cstr_from)


#define using_cvec_7(X, Value, valueDestroy, valueCompareRaw, RawValue, valueToRaw, valueFromRaw) \
\
    typedef Value cvec_##X##_value_t; \
    typedef RawValue cvec_##X##_rawvalue_t; \
    typedef cvec_##X##_rawvalue_t cvec_##X##_input_t; \
    typedef struct { cvec_##X##_value_t *val; } cvec_##X##_iter_t; \
\
    typedef struct { \
        cvec_##X##_value_t* data; \
    } cvec_##X; \
\
    STC_INLINE cvec_##X \
    cvec_##X##_init(void) {cvec_##X v = cvec__init; return v;} \
    STC_INLINE bool \
    cvec_##X##_empty(cvec_##X v) {return cvec_empty(v);} \
    STC_INLINE size_t \
    cvec_##X##_size(cvec_##X v) {return cvec_size(v);} \
    STC_INLINE size_t \
    cvec_##X##_capacity(cvec_##X v) {return cvec_capacity(v);} \
    STC_INLINE Value \
    cvec_##X##_value_from_raw(RawValue rawValue) {return valueFromRaw(rawValue);} \
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
        cvec_##X x = cvec__init; \
        cvec_##X##_resize(&x, size, null_val); \
        return x; \
    } \
    STC_INLINE cvec_##X \
    cvec_##X##_with_capacity(size_t size) { \
        cvec_##X x = cvec__init; \
        cvec_##X##_reserve(&x, size); \
        return x; \
    } \
\
    STC_API void \
    cvec_##X##_push_n(cvec_##X *self, const cvec_##X##_input_t in[], size_t size); \
    STC_API void \
    cvec_##X##_push_back(cvec_##X* self, Value value); \
    STC_INLINE void \
    cvec_##X##_emplace_back(cvec_##X* self, RawValue rawValue) { \
        cvec_##X##_push_back(self, valueFromRaw(rawValue)); \
    } \
    STC_INLINE void \
    cvec_##X##_pop_back(cvec_##X* self) { \
        valueDestroy(&self->data[--_cvec_size(self)]); \
    } \
\
    STC_API cvec_##X##_iter_t \
    cvec_##X##_insert_range_p(cvec_##X* self, cvec_##X##_value_t* pos, const cvec_##X##_value_t* pfirst, const cvec_##X##_value_t* pfinish); \
\
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_insert_range(cvec_##X* self, cvec_##X##_iter_t pos, cvec_##X##_iter_t first, cvec_##X##_iter_t finish) { \
        return cvec_##X##_insert_range_p(self, pos.val, first.val, finish.val); \
    } \
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_insert_at(cvec_##X* self, cvec_##X##_iter_t pos, Value value) { \
        return cvec_##X##_insert_range_p(self, pos.val, &value, &value + 1); \
    } \
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_insert_at_idx(cvec_##X* self, size_t idx, Value value) { \
        return cvec_##X##_insert_range_p(self, self->data + idx, &value, &value + 1); \
    } \
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_emplace_at(cvec_##X* self, cvec_##X##_iter_t pos, RawValue rawValue) { \
        return cvec_##X##_insert_at(self, pos, valueFromRaw(rawValue)); \
    } \
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_emplace_at_idx(cvec_##X* self, size_t idx, RawValue rawValue) { \
        return cvec_##X##_insert_at_idx(self, idx, valueFromRaw(rawValue)); \
    } \
\
    STC_API cvec_##X##_iter_t \
    cvec_##X##_erase_range_p(cvec_##X* self, cvec_##X##_value_t* first, cvec_##X##_value_t* finish); \
\
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_erase_range(cvec_##X* self, cvec_##X##_iter_t first, cvec_##X##_iter_t finish) { \
        return cvec_##X##_erase_range_p(self, first.val, finish.val); \
    } \
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_erase_at(cvec_##X* self, cvec_##X##_iter_t pos) { \
        return cvec_##X##_erase_range_p(self, pos.val, pos.val + 1); \
    } \
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_erase_at_idx(cvec_##X* self, size_t idx) { \
        return cvec_##X##_erase_range_p(self, self->data + idx, self->data + idx + 1); \
    } \
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_erase_range_idx(cvec_##X* self, size_t ifirst, size_t ifinish) { \
        return cvec_##X##_erase_range_p(self, self->data + ifirst, self->data + ifinish); \
    } \
\
    STC_API cvec_##X##_iter_t \
    cvec_##X##_find(const cvec_##X* self, RawValue rawValue); \
    STC_API cvec_##X##_iter_t \
    cvec_##X##_find_in_range(const cvec_##X* self, cvec_##X##_iter_t first, cvec_##X##_iter_t finish, RawValue rawValue); \
\
    STC_INLINE cvec_##X##_value_t* \
    cvec_##X##_front(cvec_##X* self) {return self->data;} \
    STC_INLINE cvec_##X##_value_t* \
    cvec_##X##_back(cvec_##X* self) {return self->data + _cvec_size(self) - 1;} \
    STC_INLINE cvec_##X##_value_t* \
    cvec_##X##_at(cvec_##X* self, size_t i) { \
        assert(i < cvec_size(*self)); \
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
        cvec_##X##_sort_with(self, 0, cvec_size(*self), cvec_##X##_value_compare); \
    } \
\
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_begin(const cvec_##X* self) { \
        cvec_##X##_iter_t it = {self->data}; return it; \
    } \
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_end(const cvec_##X* self) { \
        cvec_##X##_iter_t it = {self->data + cvec_size(*self)}; return it; \
    } \
    STC_INLINE void \
    cvec_##X##_next(cvec_##X##_iter_t* it) {++it->val;} \
    STC_INLINE cvec_##X##_value_t* \
    cvec_##X##_itval(cvec_##X##_iter_t it) {return it.val;} \
    STC_INLINE size_t \
    cvec_##X##_idx(cvec_##X v, cvec_##X##_iter_t it) {return it.val - v.data;} \
\
    _c_implement_cvec_7(X, Value, valueDestroy, RawValue, valueCompareRaw, valueToRaw, valueFromRaw) \
    typedef cvec_##X cvec_##X##_t

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
#define _c_implement_cvec_7(X, Value, valueDestroy, RawValue, valueCompareRaw, valueToRaw, valueFromRaw) \
\
    STC_DEF void \
    cvec_##X##_push_n(cvec_##X *self, const cvec_##X##_input_t in[], size_t size) { \
        cvec_##X##_reserve(self, cvec_size(*self) + size); \
        _cvec_size(self) += size; \
        for (size_t i=0; i<size; ++i) self->data[i] = valueFromRaw(in[i]); \
    } \
\
    STC_DEF void \
    cvec_##X##_clear(cvec_##X* self) { \
        cvec_##X##_value_t* p = self->data; if (p) { \
            for (cvec_##X##_value_t* q = p + _cvec_size(self); p != q; ++p) valueDestroy(p); \
            _cvec_size(self) = 0; \
        } \
    } \
    STC_DEF void \
    cvec_##X##_del(cvec_##X* self) { \
        cvec_##X##_clear(self); \
        if (self->data) c_free(_cvec_alloced(self->data)); \
    } \
\
    STC_DEF void \
    cvec_##X##_reserve(cvec_##X* self, size_t cap) { \
        size_t len = cvec_size(*self); \
        if (cap >= len) { \
            size_t* rep = (size_t *) c_realloc(_cvec_alloced(self->data), 2 * sizeof(size_t) + cap * sizeof(Value)); \
            self->data = (Value *) (rep + 2); \
            rep[0] = len; \
            rep[1] = cap; \
        } \
    } \
    STC_DEF void \
    cvec_##X##_resize(cvec_##X* self, size_t size, Value null_val) { \
        cvec_##X##_reserve(self, size); \
        for (size_t i=cvec_size(*self); i<size; ++i) self->data[i] = null_val; \
        if (self->data) _cvec_size(self) = size; \
    } \
\
    STC_DEF void \
    cvec_##X##_push_back(cvec_##X* self, Value value) { \
        size_t len = cvec_size(*self); \
        if (len == cvec_capacity(*self)) \
            cvec_##X##_reserve(self, 4 + len * 3 / 2); \
        self->data[_cvec_size(self)++] = value; \
    } \
\
    STC_DEF cvec_##X##_iter_t \
    cvec_##X##_insert_range_p(cvec_##X* self, cvec_##X##_value_t* pos, const cvec_##X##_value_t* first, const cvec_##X##_value_t* finish) { \
        size_t len = finish - first, idx = pos - self->data, size = cvec_size(*self); \
        c_withbuffer (cvec_##X##_value_t, buf, len) { \
            for (size_t i=0; i<len; ++i, ++first) \
                buf[i] = valueFromRaw(valueToRaw(first)); \
            if (size + len > cvec_capacity(*self)) \
                cvec_##X##_reserve(self, 4 + (size + len) * 3 / 2); \
            pos = self->data + idx; \
            memmove(pos + len, pos, (size - idx) * sizeof(Value)); \
            memcpy(pos, buf, len * sizeof(Value)); \
            _cvec_size(self) += len; \
        } \
        cvec_##X##_iter_t it = {pos}; return it; \
    } \
\
    STC_DEF cvec_##X##_iter_t \
    cvec_##X##_erase_range_p(cvec_##X* self, cvec_##X##_value_t* first, cvec_##X##_value_t* finish) { \
        intptr_t len = finish - first; \
        if (len > 0) { \
            cvec_##X##_value_t* p = first, *end = self->data + _cvec_size(self); \
            while (p != finish) valueDestroy(p++); \
            memmove(first, finish, (end - finish) * sizeof(Value)); \
            _cvec_size(self) -= len; \
        } \
        cvec_##X##_iter_t it = {first}; return it; \
    } \
\
    STC_DEF cvec_##X##_iter_t \
    cvec_##X##_find_in_range(const cvec_##X* self, cvec_##X##_iter_t first, cvec_##X##_iter_t finish, RawValue rawValue) { \
        for (; first.val != finish.val; cvec_##X##_next(&first)) { \
            RawValue r = valueToRaw(first.val); \
            if (valueCompareRaw(&r, &rawValue) == 0) return first; \
        } \
        return cvec_##X##_end(self); \
    } \
    STC_DEF cvec_##X##_iter_t \
    cvec_##X##_find(const cvec_##X* self, RawValue rawValue) { \
        return cvec_##X##_find_in_range(self, cvec_##X##_begin(self), cvec_##X##_end(self), rawValue); \
    } \
\
    STC_DEF int \
    cvec_##X##_value_compare(const cvec_##X##_value_t* x, const cvec_##X##_value_t* y) { \
        RawValue rx = valueToRaw(x); \
        RawValue ry = valueToRaw(y); \
        return valueCompareRaw(&rx, &ry); \
    }

#else
#define _c_implement_cvec_7(X, Value, valueDestroy, valueCompareRaw, RawValue, valueToRaw, valueFromRaw)
#endif

#if defined(_WIN32) && defined(_DLL)
#define STC_EXTERN_IMPORT extern __declspec(dllimport)
#else
#define STC_EXTERN_IMPORT extern
#endif

typedef int(*_cvec_cmp)(const void*, const void*);
STC_EXTERN_IMPORT void qsort(void *base, size_t nitems, size_t size, _cvec_cmp cmp);

#define _cvec_size(self) ((size_t *) (self)->data)[-2]

STC_INLINE size_t* _cvec_alloced(void* data) {
    return data ? ((size_t *) data) - 2 : NULL;
}
STC_INLINE size_t _cvec_safe_size(const void* data) {
    return data ? ((const size_t *) data)[-2] : 0;
}
STC_INLINE size_t _cvec_safe_capacity(const void* data) {
    return data ? ((const size_t *) data)[-1] : 0;
}

#endif
