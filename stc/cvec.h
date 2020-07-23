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
#ifndef CVECTOR__H__
#define CVECTOR__H__

#include <stdlib.h>
#include <string.h>
#include "cdefs.h"

#define cvec_init           {NULL}
#define cvec_size(cv)       _cvec_safe_size((cv).data)
#define cvec_capacity(cv)   _cvec_safe_capacity((cv).data)
#define cvec_empty(cv)      (_cvec_safe_size((cv).data) == 0)

#define declare_CVec(...)   c_MACRO_OVERLOAD(declare_CVec, __VA_ARGS__)
#define declare_CVec_2(tag, Value) \
                            declare_CVec_3(tag, Value, c_defaultDestroy)
#define declare_CVec_3(tag, Value, valueDestroy) \
                            declare_CVec_4(tag, Value, valueDestroy, c_defaultCompare)
#define declare_CVec_4(tag, Value, valueDestroy, valueCompare) \
                            declare_CVec_6(tag, Value, valueDestroy, valueCompare, Value, c_defaultGetRaw)
#define declare_CVec_str() \
                            declare_CVec_6(str, CStr, cstr_destroy, cstr_compareRaw, const char*, cstr_getRaw)


#define declare_CVec_6(tag, Value, valueDestroy, valueCompareRaw, RawValue, valueGetRaw) \
 \
typedef struct CVec_##tag { \
    Value* data; \
} CVec_##tag; \
 \
STC_API CVec_##tag \
cvec_##tag##_make(size_t size, Value null); \
STC_API void \
cvec_##tag##_pushN(CVec_##tag *self, const Value in[], size_t size); \
STC_API void \
cvec_##tag##_destroy(CVec_##tag* self); \
STC_API void \
cvec_##tag##_reserve(CVec_##tag* self, size_t cap); \
STC_API void \
cvec_##tag##_clear(CVec_##tag* self); \
STC_API void \
cvec_##tag##_pushBack(CVec_##tag* self, Value value); \
STC_INLINE void \
cvec_##tag##_popBack(CVec_##tag* self) { \
    valueDestroy(&self->data[_cvec_size(*self) - 1]); \
    --_cvec_size(*self); \
} \
STC_INLINE Value* \
cvec_##tag##_front(CVec_##tag* self) {return self->data;} \
STC_INLINE Value* \
cvec_##tag##_back(CVec_##tag* self) {return self->data + _cvec_size(*self) - 1;} \
STC_INLINE Value* \
cvec_##tag##_at(CVec_##tag* self, size_t i) {return self->data + i;} \
STC_API void \
cvec_##tag##_insert(CVec_##tag* self, size_t pos, Value value); \
STC_API void \
cvec_##tag##_erase(CVec_##tag* self, size_t pos, size_t size); \
STC_API void \
cvec_##tag##_sort(CVec_##tag* self); \
STC_API size_t \
cvec_##tag##_find(const CVec_##tag* self, RawValue rawValue); \
STC_INLINE void \
cvec_##tag##_swap(CVec_##tag* a, CVec_##tag* b) { \
    c_swap(Value*, a->data, b->data); \
} \
 \
typedef struct { \
    Value *item, *end; \
} CVecIter_##tag, cvec_##tag##_iter_t; \
 \
STC_INLINE cvec_##tag##_iter_t \
cvec_##tag##_begin(CVec_##tag* vec) { \
    const size_t n = cvec_size(*vec); \
    cvec_##tag##_iter_t it = {n ? vec->data : NULL, vec->data + n}; \
    return it; \
} \
STC_INLINE cvec_##tag##_iter_t \
cvec_##tag##_next(cvec_##tag##_iter_t it) { \
    if (++it.item == it.end) it.item = NULL; \
    return it; \
} \
 \
implement_CVec_6(tag, Value, valueDestroy, RawValue, valueCompareRaw, valueGetRaw) \
typedef Value CVecValue_##tag, cvec_##tag##_input_t; \
typedef RawValue CVecRawValue_##tag

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
#define implement_CVec_6(tag, Value, valueDestroy, RawValue, valueCompareRaw, valueGetRaw) \
 \
STC_API CVec_##tag \
cvec_##tag##_make(size_t size, Value null) { \
    CVec_##tag vec = cvec_init; \
    cvec_##tag##_reserve(&vec, size); \
    _cvec_size(vec) = size; \
    for (size_t i=0; i<size; ++i) vec.data[i] = null; \
    return vec; \
} \
STC_API void \
cvec_##tag##_pushN(CVec_##tag *self, const Value in[], size_t size) { \
    cvec_##tag##_reserve(self, cvec_size(*self) + size); \
    _cvec_size(*self) += size; \
    for (size_t i=0; i<size; ++i) self->data[i] = in[i]; \
} \
 \
STC_API void \
cvec_##tag##_destroy(CVec_##tag* self) { \
    Value* p = self->data; \
    size_t i = 0, n = cvec_size(*self); \
    for (; i < n; ++p, ++i) valueDestroy(p); \
    free(_cvec_alloced(self->data)); \
} \
 \
STC_API void \
cvec_##tag##_reserve(CVec_##tag* self, size_t cap) { \
    size_t len = cvec_size(*self); \
    if (cap >= len) { \
        size_t* rep = (size_t *) realloc(_cvec_alloced(self->data), 2 * sizeof(size_t) + cap * sizeof(Value)); \
        self->data = (Value *) (rep + 2); \
        rep[0] = len; \
        rep[1] = cap; \
    } \
} \
 \
STC_API void \
cvec_##tag##_clear(CVec_##tag* self) { \
    CVec_##tag cv = cvec_init; \
    cvec_##tag##_destroy(self); \
    *self = cv; \
} \
 \
STC_API void \
cvec_##tag##_pushBack(CVec_##tag* self, Value value) { \
    size_t len = cvec_size(*self); \
    if (len == cvec_capacity(*self)) \
        cvec_##tag##_reserve(self, 7 + len * 5 / 3); \
    self->data[cvec_size(*self)] = value; \
    ++_cvec_size(*self); \
} \
 \
STC_API void \
cvec_##tag##_insert(CVec_##tag* self, size_t pos, Value value) { \
    size_t len = cvec_size(*self); \
    if (len == cvec_capacity(*self)) \
        cvec_##tag##_reserve(self, 7 + len * 5 / 3); \
    memmove(&self->data[pos + 1], &self->data[pos], (len - pos) * sizeof(Value)); \
    self->data[pos] = value; \
    ++_cvec_size(*self); \
} \
 \
STC_API void \
cvec_##tag##_erase(CVec_##tag* self, size_t pos, size_t size) { \
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
cvec_##tag##_find(const CVec_##tag* self, RawValue rawValue) { \
    const Value *p = self->data, *end = p + cvec_size(*self); \
    for (; p != end; ++p) { \
        RawValue r = valueGetRaw(p); \
        if (valueCompareRaw(&r, &rawValue) == 0) return p - self->data; \
    } \
    return SIZE_MAX; /*(size_t) (-1)*/ \
} \
 \
STC_API int \
cvec_##tag##_sortCompare(const void* x, const void* y) { \
    RawValue rx = valueGetRaw((const Value *) x); \
    RawValue ry = valueGetRaw((const Value *) y); \
    return valueCompareRaw(&rx, &ry); \
} \
STC_EXTERN_IMPORT void qsort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void*)); \
STC_API void \
cvec_##tag##_sort(CVec_##tag* self) { \
    size_t len = cvec_size(*self); \
    if (len) qsort(self->data, len, sizeof(Value), cvec_##tag##_sortCompare); \
}

#else
#define implement_CVec_6(tag, Value, valueDestroy, valueCompareRaw, RawValue, valueGetRaw)
#endif

#if defined(_WIN32) && defined(_DLL)
#define STC_EXTERN_IMPORT extern __declspec(dllimport)
#else
#define STC_EXTERN_IMPORT extern
#endif

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
