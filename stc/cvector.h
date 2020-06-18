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

#define cvector_init           {NULL}
#define cvector_size(cv)       _cvector_safe_size((cv).data)
#define cvector_capacity(cv)   _cvector_safe_capacity((cv).data)
#define cvector_empty(cv)      (_cvector_safe_size((cv).data) == 0)

#define declare_CVector(...)   c_MACRO_OVERLOAD(declare_CVector, __VA_ARGS__)
#define declare_CVector_2(tag, Value) \
                                declare_CVector_3(tag, Value, c_noDestroy)
#define declare_CVector_3(tag, Value, valueDestroy) \
                                declare_CVector_4(tag, Value, valueDestroy, c_defaultCompare)
#define declare_CVector_4(tag, Value, valueDestroy, valueCompare) \
                                declare_CVector_6(tag, Value, valueDestroy, Value, valueCompare, c_defaultGetRaw)
#define declare_CVector_string(tag) \
                                declare_CVector_6(tag, CString, cstring_destroy, const char*, cstring_compareRaw, cstring_getRaw)


#define declare_CVector_6(tag, Value, valueDestroy, RawValue, valueCompareRaw, valueGetRaw) \
 \
typedef struct CVector_##tag { \
    Value* data; \
} CVector_##tag; \
 \
STC_API void \
cvector_##tag##_destroy(CVector_##tag* self); \
STC_API void \
cvector_##tag##_reserve(CVector_##tag* self, size_t cap); \
STC_API void \
cvector_##tag##_clear(CVector_##tag* self); \
STC_API void \
cvector_##tag##_pushBack(CVector_##tag* self, Value value); \
static inline void \
cvector_##tag##_popBack(CVector_##tag* self) { \
    valueDestroy(&self->data[_cvector_size(*self) - 1]); \
    --_cvector_size(*self); \
} \
static inline Value \
cvector_##tag##_back(CVector_##tag cv) { \
    return cv.data[_cvector_size(cv) - 1]; \
} \
STC_API void \
cvector_##tag##_insert(CVector_##tag* self, size_t pos, Value value); \
STC_API void \
cvector_##tag##_erase(CVector_##tag* self, size_t pos, size_t size); \
STC_API void \
cvector_##tag##_sort(CVector_##tag* self); \
STC_API size_t \
cvector_##tag##_find(CVector_##tag cv, RawValue rawValue); \
static inline void \
cvector_##tag##_swap(CVector_##tag* a, CVector_##tag* b) { \
    c_swap(Value*, a->data, b->data); \
} \
 \
typedef struct { \
    Value *item, *end; \
} CVectorIter_##tag, cvector_##tag##_iter_t; \
 \
static inline cvector_##tag##_iter_t \
cvector_##tag##_begin(CVector_##tag* vec) { \
    const size_t n = cvector_size(*vec); \
    cvector_##tag##_iter_t it = {n ? vec->data : NULL, vec->data + n}; \
    return it; \
} \
static inline cvector_##tag##_iter_t \
cvector_##tag##_next(cvector_##tag##_iter_t it) { \
    if (++it.item == it.end) it.item = NULL; \
    return it; \
} \
 \
implement_CVector_6(tag, Value, valueDestroy, RawValue, valueCompareRaw, valueGetRaw) \
typedef Value CVectorValue_##tag; \
typedef RawValue CVectorRawValue_##tag

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
#define implement_CVector_6(tag, Value, valueDestroy, RawValue, valueCompareRaw, valueGetRaw) \
 \
STC_API void \
cvector_##tag##_destroy(CVector_##tag* self) { \
    Value* p = self->data; \
    size_t i = 0, n = cvector_size(*self); \
    for (; i < n; ++p, ++i) valueDestroy(p); \
    free(_cvector_alloced(self->data)); \
} \
 \
STC_API void \
cvector_##tag##_reserve(CVector_##tag* self, size_t cap) { \
    size_t len = cvector_size(*self); \
    if (cap >= len) { \
        size_t* rep = (size_t *) realloc(_cvector_alloced(self->data), 2 * sizeof(size_t) + cap * sizeof(Value)); \
        self->data = (Value *) (rep + 2); \
        rep[0] = len; \
        rep[1] = cap; \
    } \
} \
 \
STC_API void \
cvector_##tag##_clear(CVector_##tag* self) { \
    CVector_##tag cv = cvector_init; \
    cvector_##tag##_destroy(self); \
    *self = cv; \
} \
 \
STC_API void \
cvector_##tag##_pushBack(CVector_##tag* self, Value value) { \
    size_t len = cvector_size(*self); \
    if (len == cvector_capacity(*self)) \
        cvector_##tag##_reserve(self, 7 + len * 5 / 3); \
    self->data[cvector_size(*self)] = value; \
    ++_cvector_size(*self); \
} \
 \
STC_API void \
cvector_##tag##_insert(CVector_##tag* self, size_t pos, Value value) { \
    size_t len = cvector_size(*self); \
    if (len == cvector_capacity(*self)) \
        cvector_##tag##_reserve(self, 7 + len * 5 / 3); \
    memmove(&self->data[pos + 1], &self->data[pos], (len - pos) * sizeof(Value)); \
    self->data[pos] = value; \
    ++_cvector_size(*self); \
} \
 \
STC_API void \
cvector_##tag##_erase(CVector_##tag* self, size_t pos, size_t size) { \
    size_t len = cvector_size(*self); \
    if (len) { \
        Value* p = &self->data[pos], *start = p, *end = p + size; \
        while (p != end) valueDestroy(p++); \
        memmove(start, end, (len - pos - size) * sizeof(Value)); \
        _cvector_size(*self) -= size; \
    } \
} \
 \
STC_API size_t \
cvector_##tag##_find(CVector_##tag cv, RawValue rawValue) { \
    size_t n = cvector_size(cv); \
    for (size_t i = 0; i < n; ++i) { \
        RawValue r = valueGetRaw(&cv.data[i]); \
        if (valueCompareRaw(&r, &rawValue) == 0) return i; \
    } \
    return (size_t) (-1); /*SIZE_MAX;*/ \
} \
 \
STC_API int \
cvector_##tag##_sortCompare(const void* x, const void* y) { \
    RawValue rx = valueGetRaw((const Value *) x); \
    RawValue ry = valueGetRaw((const Value *) y); \
    return valueCompareRaw(&rx, &ry); \
} \
STC_EXTERN_IMPORT void qsort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void*)); \
STC_API void \
cvector_##tag##_sort(CVector_##tag* self) { \
    size_t len = cvector_size(*self); \
    if (len) qsort(self->data, len, sizeof(Value), cvector_##tag##_sortCompare); \
}

#else
#define implement_CVector_6(tag, Value, valueDestroy, RawValue, valueCompareRaw, valueGetRaw)
#endif

#if defined(_WIN32) && defined(_DLL)
#define STC_EXTERN_IMPORT extern __declspec(dllimport)
#else
#define STC_EXTERN_IMPORT extern
#endif

#define _cvector_size(cv) ((size_t *)(cv).data)[-2]
#define _cvector_capacity(cv) ((size_t *)(cv).data)[-1]

static inline size_t* _cvector_alloced(void* data) {
    return data ? ((size_t *) data) - 2 : NULL;
}
static inline size_t _cvector_safe_size(const void* data) {
    return data ? ((const size_t *) data)[-2] : 0;
}
static inline size_t _cvector_safe_capacity(const void* data) {
    return data ? ((const size_t *) data)[-1] : 0;
}


#endif
