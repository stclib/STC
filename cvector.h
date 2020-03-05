// MIT License
//
// Copyright (c) 2020 Tyge Løvset, NORCE, www.norceresearch.no
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef CVECTOR__H__
#define CVECTOR__H__

#include <malloc.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#include "cdef.h"

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


#define cvector_initializer    {NULL}
#define cvector_size(cv)       _cvector_safe_size((cv).data)
#define cvector_capacity(cv)   _cvector_safe_capacity((cv).data)
#define cvector_empty(cv)      (_cvector_safe_size((cv).data) == 0)

#define declare_CVector(...)           cdef_MACRO_OVERLOAD(declare_CVector, __VA_ARGS__)
#define declare_CVector_2(tag, Value)  declare_CVector_3(tag, Value, cdef_destroy)
#define declare_CStringVector(tag)     declare_CVector_3(tag, CString, cstring_destroy)


#define declare_CVector_3(tag, Value, valueDestroy) \
typedef struct CVector_##tag { \
    Value* data; \
} CVector_##tag; \
typedef struct cvector_##tag##_iter_t { \
    Value* item; \
} cvector_##tag##_iter_t; \
 \
static inline CVector_##tag cvector_##tag##_init(void) { \
    CVector_##tag cv = cvector_initializer; \
    return cv; \
} \
 \
static inline void cvector_##tag##_swap(CVector_##tag* a, CVector_##tag* b) { \
    Value* data = a->data; a->data = b->data; b->data = data; \
} \
 \
static inline void cvector_##tag##_destroy(CVector_##tag* self) { \
    Value* p = self->data; \
    size_t i = 0, n = cvector_size(*self); \
    for (; i < n; ++p, ++i) valueDestroy(p); \
    free(_cvector_alloced(self->data)); \
} \
 \
static inline void cvector_##tag##_reserve(CVector_##tag* self, size_t cap) { \
    if (cap > cvector_capacity(*self)) { \
        size_t len = cvector_size(*self); \
        size_t* rep = (size_t *) realloc(_cvector_alloced(self->data), 2 * sizeof(size_t) + cap * sizeof(Value)); \
        self->data = (Value *) (rep + 2); \
        rep[0] = len; \
        rep[1] = cap; \
    } \
} \
 \
static inline void cvector_##tag##_clear(CVector_##tag* self) { \
    CVector_##tag cv = cvector_initializer; \
    cvector_##tag##_destroy(self); \
    *self = cv; \
} \
 \
 \
static inline void cvector_##tag##_push(CVector_##tag* self, Value value) { \
    size_t newsize = cvector_size(*self) + 1; \
    if (newsize > cvector_capacity(*self)) \
        cvector_##tag##_reserve(self, 7 + newsize * 5 / 3); \
    self->data[cvector_size(*self)] = value; \
    _cvector_size(*self) = newsize; \
} \
 \
static inline void cvector_##tag##_insert(CVector_##tag* self, size_t pos, Value value) { \
    cvector_##tag##_push(self, value); \
    size_t len = cvector_size(*self); \
    memmove(&self->data[pos + 1], &self->data[pos], (len - pos - 1) * sizeof(Value)); \
    self->data[pos] = value; \
} \
 \
 \
static inline Value cvector_##tag##_back(CVector_##tag cv) { \
    return cv.data[_cvector_size(cv) - 1]; \
} \
 \
 \
static inline void cvector_##tag##_pop(CVector_##tag* self) { \
    valueDestroy(&self->data[_cvector_size(*self) - 1]); \
    --_cvector_size(*self); \
} \
 \
 \
static inline cvector_##tag##_iter_t cvector_##tag##_begin(CVector_##tag vec) { \
    cvector_##tag##_iter_t it = {vec.data}; \
    return it; \
} \
 \
static inline cvector_##tag##_iter_t cvector_##tag##_next(cvector_##tag##_iter_t it) { \
    ++it.item; \
    return it; \
} \
 \
static inline cvector_##tag##_iter_t cvector_##tag##_end(CVector_##tag vec) { \
    cvector_##tag##_iter_t it = {vec.data + cvector_size(vec)}; \
    return it; \
} \
typedef Value cvector_##tag##_value_t

#endif
