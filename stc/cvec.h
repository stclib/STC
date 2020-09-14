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

#include <stdlib.h>
#include <string.h>
#include "cdefs.h"

#define cvec_ini           {NULL}
#define cvec_size(v)       _cvec_safe_size((v).data)
#define cvec_capacity(v)   _cvec_safe_capacity((v).data)
#define cvec_empty(v)      (cvec_size(v) == 0)

#define declare_cvec(...)   c_MACRO_OVERLOAD(declare_cvec, __VA_ARGS__)
#define declare_cvec_2(X, Value) \
                            declare_cvec_3(X, Value, c_default_destroy)
#define declare_cvec_3(X, Value, valueDestroy) \
                            declare_cvec_4(X, Value, valueDestroy, c_default_compare)
#define declare_cvec_4(X, Value, valueDestroy, valueCompare) \
                            declare_cvec_7(X, Value, valueDestroy, valueCompare, Value, c_default_to_raw, c_default_from_raw)
#define declare_cvec_str() \
                            declare_cvec_7(str, cstr_t, cstr_destroy, cstr_compare_raw, const char*, cstr_to_raw, cstr_make)


#define declare_cvec_7(X, Value, valueDestroy, valueCompareRaw, RawValue, valueToRaw, valueFromRaw) \
\
    typedef struct cvec_##X { \
        Value* data; \
    } cvec_##X; \
    typedef Value cvec_##X##_value_t; \
    typedef RawValue cvec_##X##_rawvalue_t; \
    typedef cvec_##X##_rawvalue_t cvec_##X##_input_t; \
    typedef struct { Value *item; } cvec_##X##_iter_t; \
\
    STC_INLINE cvec_##X \
    cvec_##X##_init(void) {cvec_##X v = cvec_ini; return v;} \
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
    cvec_##X##_destroy(cvec_##X* self); \
    STC_API void \
    cvec_##X##_reserve(cvec_##X* self, size_t cap); \
    STC_API void \
    cvec_##X##_resize(cvec_##X* self, size_t size, Value fill_val); \
    STC_INLINE void \
    cvec_##X##_swap(cvec_##X* a, cvec_##X* b) {c_swap(Value*, a->data, b->data);} \
\
    STC_INLINE cvec_##X \
    cvec_##X##_with_size(size_t size, Value null_val) { \
        cvec_##X x = cvec_ini; \
        cvec_##X##_resize(&x, size, null_val); \
        return x; \
    } \
    STC_INLINE cvec_##X \
    cvec_##X##_with_capacity(size_t size) { \
        cvec_##X x = cvec_ini; \
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
    cvec_##X##_insert_range(cvec_##X* self, cvec_##X##_iter_t pos, cvec_##X##_iter_t first, cvec_##X##_iter_t finish); \
    \
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_insert_range_ptr(cvec_##X* self, size_t idx, Value* pfirst, Value* pfinish) { \
        cvec_##X##_iter_t pos = {self->data + idx}, first = {pfirst}, finish = {pfinish}; \
        return cvec_##X##_insert_range(self, pos, first, finish); \
    } \
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_insert_at(cvec_##X* self, cvec_##X##_iter_t pos, Value value) { \
        cvec_##X##_iter_t first = {&value}, finish = {&value + 1}; \
        return cvec_##X##_insert_range(self, pos, first, finish); \
    } \
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_insert_at_idx(cvec_##X* self, size_t idx, Value value) { \
        cvec_##X##_iter_t pos = {self->data + idx}, first = {&value}, finish = {&value + 1}; \
        return cvec_##X##_insert_range(self, pos, first, finish); \
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
    cvec_##X##_erase_range(cvec_##X* self, cvec_##X##_iter_t first, cvec_##X##_iter_t finish); \
\
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_erase_at(cvec_##X* self, cvec_##X##_iter_t pos) { \
        cvec_##X##_iter_t next = {pos.item + 1}; \
        return cvec_##X##_erase_range(self, pos, next); \
    } \
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_erase_at_idx(cvec_##X* self, size_t idx) { \
        cvec_##X##_iter_t first = {self->data + idx}, finish = {first.item + 1}; \
        return cvec_##X##_erase_range(self, first, finish); \
    } \
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_erase_range_idx(cvec_##X* self, size_t ifirst, size_t ifinish) { \
        cvec_##X##_iter_t first = {self->data + ifirst}, finish = {self->data + ifinish}; \
        return cvec_##X##_erase_range(self, first, finish); \
    } \
\
    STC_API cvec_##X##_iter_t \
    cvec_##X##_find(const cvec_##X* self, RawValue rawValue); \
    STC_API cvec_##X##_iter_t \
    cvec_##X##_find_in_range(const cvec_##X* self, cvec_##X##_iter_t first, cvec_##X##_iter_t finish, RawValue rawValue); \
\
    STC_INLINE Value* \
    cvec_##X##_front(cvec_##X* self) {return self->data;} \
    STC_INLINE Value* \
    cvec_##X##_back(cvec_##X* self) {return self->data + _cvec_size(self) - 1;} \
    STC_INLINE Value* \
    cvec_##X##_at(cvec_##X* self, size_t i) { \
        assert(i < cvec_size(*self)); \
        return self->data + i; \
    } \
\
    STC_API int \
    cvec_##X##_value_compare(const Value* x, const Value* y); \
    STC_INLINE void \
    cvec_##X##_sort_with(cvec_##X* self, size_t ifirst, size_t ifinish, int(*cmp)(const Value*, const Value*)) { \
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
    cvec_##X##_next(cvec_##X##_iter_t* it) {++it->item;} \
    STC_INLINE cvec_##X##_value_t* \
    cvec_##X##_itval(cvec_##X##_iter_t it) {return it.item;} \
    STC_INLINE size_t \
    cvec_##X##_idx(cvec_##X v, cvec_##X##_iter_t it) {return it.item - v.data;} \
\
    implement_cvec_7(X, Value, valueDestroy, RawValue, valueCompareRaw, valueToRaw, valueFromRaw)

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
#define implement_cvec_7(X, Value, valueDestroy, RawValue, valueCompareRaw, valueToRaw, valueFromRaw) \
\
    STC_API void \
    cvec_##X##_push_n(cvec_##X *self, const cvec_##X##_input_t in[], size_t size) { \
        cvec_##X##_reserve(self, cvec_size(*self) + size); \
        _cvec_size(self) += size; \
        for (size_t i=0; i<size; ++i) self->data[i] = valueFromRaw(in[i]); \
    } \
\
    STC_API void \
    cvec_##X##_clear(cvec_##X* self) { \
        Value* p = self->data; if (p) { \
            for (Value* q = p + _cvec_size(self); p != q; ++p) valueDestroy(p); \
            _cvec_size(self) = 0; \
        } \
    } \
    STC_API void \
    cvec_##X##_destroy(cvec_##X* self) { \
        cvec_##X##_clear(self); \
        if (self->data) free(_cvec_alloced(self->data)); \
    } \
\
    STC_API void \
    cvec_##X##_reserve(cvec_##X* self, size_t cap) { \
        size_t len = cvec_size(*self); \
        if (cap >= len) { \
            size_t* rep = (size_t *) realloc(_cvec_alloced(self->data), 2 * sizeof(size_t) + cap * sizeof(Value)); \
            self->data = (Value *) (rep + 2); \
            rep[0] = len; \
            rep[1] = cap; \
        } \
    } \
    STC_API void \
    cvec_##X##_resize(cvec_##X* self, size_t size, Value null_val) { \
        cvec_##X##_reserve(self, size); \
        for (size_t i=cvec_size(*self); i<size; ++i) self->data[i] = null_val; \
        if (self->data) _cvec_size(self) = size; \
    } \
\
    STC_API void \
    cvec_##X##_push_back(cvec_##X* self, Value value) { \
        size_t len = cvec_size(*self); \
        if (len == cvec_capacity(*self)) \
            cvec_##X##_reserve(self, 4 + len * 3 / 2); \
        self->data[_cvec_size(self)++] = value; \
    } \
\
    STC_API cvec_##X##_iter_t \
    cvec_##X##_insert_range(cvec_##X* self, cvec_##X##_iter_t pos, cvec_##X##_iter_t first, cvec_##X##_iter_t finish) { \
        enum {max_buf = c_max_alloca / sizeof(Value) + 1}; Value buf[max_buf]; \
        size_t len = finish.item - first.item, idx = pos.item - self->data, size = cvec_size(*self); \
        Value* xbuf = (Value *) memcpy(len > max_buf ? c_new_n(Value, len) : buf, first.item, len * sizeof(Value)); \
        if (size + len > cvec_capacity(*self)) \
            cvec_##X##_reserve(self, 4 + (size + len) * 3 / 2); \
        pos.item = self->data + idx; \
        memmove(pos.item + len, pos.item, (size - idx) * sizeof(Value)); \
        memcpy(pos.item, xbuf, len * sizeof(Value)); \
        _cvec_size(self) += len; \
        if (len > max_buf) free(xbuf); \
        return pos; \
    } \
\
    STC_API cvec_##X##_iter_t \
    cvec_##X##_erase_range(cvec_##X* self, cvec_##X##_iter_t first, cvec_##X##_iter_t finish) { \
        intptr_t len = finish.item - first.item; \
        if (len > 0) { \
            Value* p = first.item, *end = p + _cvec_size(self); \
            while (p != finish.item) valueDestroy(p++); \
            memmove(first.item, finish.item, (end - finish.item) * sizeof(Value)); \
            _cvec_size(self) -= len; \
        } \
        return first; \
    } \
\
    STC_API cvec_##X##_iter_t \
    cvec_##X##_find_in_range(const cvec_##X* self, cvec_##X##_iter_t first, cvec_##X##_iter_t finish, RawValue rawValue) { \
        for (; first.item != finish.item; cvec_##X##_next(&first)) { \
            RawValue r = valueToRaw(first.item); \
            if (valueCompareRaw(&r, &rawValue) == 0) return first; \
        } \
        return cvec_##X##_end(self); \
    } \
    STC_API cvec_##X##_iter_t \
    cvec_##X##_find(const cvec_##X* self, RawValue rawValue) { \
        return cvec_##X##_find_in_range(self, cvec_##X##_begin(self), cvec_##X##_end(self), rawValue); \
    } \
\
    STC_API int \
    cvec_##X##_value_compare(const Value* x, const Value* y) { \
        RawValue rx = valueToRaw(x); \
        RawValue ry = valueToRaw(y); \
        return valueCompareRaw(&rx, &ry); \
    } \
    typedef int cvec_##taq##_dud

#else
#define implement_cvec_7(X, Value, valueDestroy, valueCompareRaw, RawValue, valueToRaw, valueFromRaw)
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
