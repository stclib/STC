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

#define cvec_init           {NULL}
#define cvec_size(v)       _cvec_safe_size((v).data)
#define cvec_capacity(v)   _cvec_safe_capacity((v).data)
#define cvec_empty(v)      (_cvec_safe_size((v).data) == 0)
#define cvec_front(v)      (v).data[0]
#define cvec_back(v)       (v).data[_cvec_size(v) - 1] /* may have side effect */

#define declare_cvec(...)   c_MACRO_OVERLOAD(declare_cvec, __VA_ARGS__)
#define declare_cvec_2(tag, Value) \
                            declare_cvec_3(tag, Value, c_default_destroy)
#define declare_cvec_3(tag, Value, valueDestroy) \
                            declare_cvec_4(tag, Value, valueDestroy, c_default_compare)
#define declare_cvec_4(tag, Value, valueDestroy, valueCompare) \
                            declare_cvec_7(tag, Value, valueDestroy, valueCompare, Value, c_default_to_raw, c_default_from_raw)
#define declare_cvec_str() \
                            declare_cvec_7(str, cstr_t, cstr_destroy, cstr_compare_raw, const char*, cstr_to_raw, cstr_make)


#define declare_cvec_7(tag, Value, valueDestroy, valueCompareRaw, RawValue, valueToRaw, valueFromRaw) \
 \
typedef struct cvec_##tag { \
    Value* data; \
} cvec_##tag; \
typedef RawValue cvec_##tag##_rawvalue_t; \
typedef cvec_##tag##_rawvalue_t cvec_##tag##_input_t; \
 \
STC_API void \
cvec_##tag##_destroy(cvec_##tag* self); \
STC_API void \
cvec_##tag##_reserve(cvec_##tag* self, size_t cap); \
STC_API void \
cvec_##tag##_resize(cvec_##tag* self, size_t size, Value null_val); \
STC_API void \
cvec_##tag##_push_n(cvec_##tag *self, const cvec_##tag##_input_t in[], size_t size); \
STC_API void \
cvec_##tag##_push_back_v(cvec_##tag* self, Value value); \
STC_INLINE void \
cvec_##tag##_push_back(cvec_##tag* self, cvec_##tag##_rawvalue_t rawValue) { \
    cvec_##tag##_push_back_v(self, valueFromRaw(rawValue)); \
} \
STC_API void \
cvec_##tag##_insert_v(cvec_##tag* self, size_t pos, Value value); \
STC_INLINE void \
cvec_##tag##_insert(cvec_##tag* self, size_t pos, RawValue rawValue) { \
    cvec_##tag##_insert_v(self, pos, valueFromRaw(rawValue)); \
} \
STC_API void \
cvec_##tag##_erase(cvec_##tag* self, size_t pos, size_t size); \
STC_API void \
cvec_##tag##_sort(cvec_##tag* self); \
STC_API void \
cvec_##tag##_sort_with(cvec_##tag* self, int(*cmp)(const Value*, const Value*)); \
STC_API size_t \
cvec_##tag##_find(const cvec_##tag* self, RawValue rawValue); \
STC_API int \
cvec_##tag##_value_compare(const Value* x, const Value* y); \
 \
STC_INLINE cvec_##tag \
cvec_##tag##_init(void) { \
    cvec_##tag x = cvec_init; return x; \
} \
STC_INLINE cvec_##tag \
cvec_##tag##_with_size(size_t size, Value null_val) { \
    cvec_##tag x = cvec_init; \
    cvec_##tag##_resize(&x, size, null_val); \
    return x; \
} \
STC_INLINE cvec_##tag \
cvec_##tag##_with_capacity(size_t size) { \
    cvec_##tag x = cvec_init; \
    cvec_##tag##_reserve(&x, size); \
    return x; \
} \
STC_INLINE void \
cvec_##tag##_clear(cvec_##tag* self) { \
    if (self->data) _cvec_size(*self) = 0; \
} \
STC_INLINE void \
cvec_##tag##_pop_back(cvec_##tag* self) { \
    valueDestroy(&self->data[_cvec_size(*self) - 1]); \
    --_cvec_size(*self); \
} \
STC_INLINE Value* \
cvec_##tag##_front(cvec_##tag* self) {return self->data;} \
STC_INLINE Value* \
cvec_##tag##_back(cvec_##tag* self) {return self->data + _cvec_size(*self) - 1;} \
STC_INLINE Value* \
cvec_##tag##_at(cvec_##tag* self, size_t i) {return self->data + i;} \
STC_INLINE void \
cvec_##tag##_swap(cvec_##tag* a, cvec_##tag* b) { \
    c_swap(Value*, a->data, b->data); \
} \
STC_INLINE void \
cvec_##tag##_sort(cvec_##tag* self) { \
    qsort(self->data, cvec_size(*self), sizeof(Value), (_cvec_cmp) cvec_##tag##_value_compare); \
} \
STC_INLINE void \
cvec_##tag##_sort_with(cvec_##tag* self, int(*cmp)(const Value*, const Value*)) { \
    qsort(self->data, cvec_size(*self), sizeof(Value), (_cvec_cmp) cmp); \
} \
 \
typedef struct { \
    Value *item, *end; \
} cvec_##tag##_iter_t; \
 \
STC_INLINE cvec_##tag##_iter_t \
cvec_##tag##_begin(cvec_##tag* vec) { \
    cvec_##tag##_iter_t it = {vec->data, vec->data + cvec_size(*vec)}; \
    return it; \
} \
STC_INLINE void \
cvec_##tag##_next(cvec_##tag##_iter_t* it) { ++it->item; } \
 \
implement_cvec_7(tag, Value, valueDestroy, RawValue, valueCompareRaw, valueToRaw, valueFromRaw) \
typedef Value cvec_##tag##_value_t

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
#define implement_cvec_7(tag, Value, valueDestroy, RawValue, valueCompareRaw, valueToRaw, valueFromRaw) \
 \
STC_API void \
cvec_##tag##_push_n(cvec_##tag *self, const cvec_##tag##_input_t in[], size_t size) { \
    cvec_##tag##_reserve(self, cvec_size(*self) + size); \
    _cvec_size(*self) += size; \
    for (size_t i=0; i<size; ++i) self->data[i] = valueFromRaw(in[i]); \
} \
 \
STC_API void \
cvec_##tag##_destroy(cvec_##tag* self) { \
    Value* p = self->data; \
    size_t i = 0, n = cvec_size(*self); \
    for (; i < n; ++p, ++i) valueDestroy(p); \
    free(_cvec_alloced(self->data)); \
} \
 \
STC_API void \
cvec_##tag##_reserve(cvec_##tag* self, size_t cap) { \
    size_t len = cvec_size(*self); \
    if (cap >= len) { \
        size_t* rep = (size_t *) realloc(_cvec_alloced(self->data), 2 * sizeof(size_t) + cap * sizeof(Value)); \
        self->data = (Value *) (rep + 2); \
        rep[0] = len; \
        rep[1] = cap; \
    } \
} \
STC_API void \
cvec_##tag##_resize(cvec_##tag* self, size_t size, Value null_val) { \
    cvec_##tag##_reserve(self, size); \
    for (size_t i=cvec_size(*self); i<size; ++i) self->data[i] = null_val; \
    if (self->data) _cvec_size(*self) = size; \
} \
 \
STC_API void \
cvec_##tag##_push_back_v(cvec_##tag* self, Value value) { \
    size_t len = cvec_size(*self); \
    if (len == cvec_capacity(*self)) \
        cvec_##tag##_reserve(self, 4 + len * 3 / 2); \
    self->data[cvec_size(*self)] = value; \
    ++_cvec_size(*self); \
} \
 \
STC_API void \
cvec_##tag##_insert_v(cvec_##tag* self, size_t pos, Value value) { \
    size_t len = cvec_size(*self); \
    if (len == cvec_capacity(*self)) \
        cvec_##tag##_reserve(self, 4 + len * 3 / 2); \
    memmove(&self->data[pos + 1], &self->data[pos], (len - pos) * sizeof(Value)); \
    self->data[pos] = value; \
    ++_cvec_size(*self); \
} \
 \
STC_API void \
cvec_##tag##_erase(cvec_##tag* self, size_t pos, size_t size) { \
    size_t len = cvec_size(*self); \
    if (len) { \
        Value* p = &self->data[pos], *start = p, *end = p + size; \
        while (p != end) valueDestroy(p++); \
        memmove(start, end, (len - pos - size) * sizeof(Value)); \
        _cvec_size(*self) -= size; \
    } \
} \
 \
STC_API size_t \
cvec_##tag##_find(const cvec_##tag* self, RawValue rawValue) { \
    const Value *p = self->data, *end = p + cvec_size(*self); \
    for (; p != end; ++p) { \
        RawValue r = valueToRaw(p); \
        if (valueCompareRaw(&r, &rawValue) == 0) return p - self->data; \
    } \
    return SIZE_MAX; /*(size_t) (-1)*/ \
} \
 \
STC_API int \
cvec_##tag##_value_compare(const Value* x, const Value* y) { \
    RawValue rx = valueToRaw(x); \
    RawValue ry = valueToRaw(y); \
    return valueCompareRaw(&rx, &ry); \
}

#else
#define implement_cvec_7(tag, Value, valueDestroy, valueCompareRaw, RawValue, valueToRaw, valueFromRaw)
#endif

#if defined(_WIN32) && defined(_DLL)
#define STC_EXTERN_IMPORT extern __declspec(dllimport)
#else
#define STC_EXTERN_IMPORT extern
#endif

typedef int(*_cvec_cmp)(const void*, const void*);
STC_EXTERN_IMPORT void qsort(void *base, size_t nitems, size_t size, _cvec_cmp cmp);

#define _cvec_size(cv) ((size_t *)(cv).data)[-2]
#define _cvec_capacity(cv) ((size_t *)(cv).data)[-1]

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
