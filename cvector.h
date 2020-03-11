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

#ifndef C_VECTOR__H__
#define C_VECTOR__H__

#include <malloc.h>
#include <string.h>
#include "c_defs.h"


#define c_vector_initializer    {NULL}
#define c_vector_size(cv)       _c_vector_safe_size((cv).data)
#define c_vector_capacity(cv)   _c_vector_safe_capacity((cv).data)
#define c_vector_empty(cv)      (_c_vector_safe_size((cv).data) == 0)

#define c_declare_Vector(...)           c_defs_MACRO_OVERLOAD(c_declare_Vector, __VA_ARGS__)
#define c_declare_Vector_2(tag, Value)  c_declare_Vector_3(tag, Value, c_defs_destroy)
#define declare_CStringVector(tag)     c_declare_Vector_3(tag, CString, cstring_destroy)


#define c_declare_Vector_3(tag, Value, valueDestroy) \
typedef struct c_Vector_##tag { \
    Value* data; \
} c_Vector_##tag; \
typedef struct c_vector_##tag##_iter_t { \
    Value* item; \
} c_vector_##tag##_iter_t; \
 \
static inline c_Vector_##tag c_vector_##tag##_init(void) { \
    c_Vector_##tag cv = c_vector_initializer; \
    return cv; \
} \
 \
static inline void c_vector_##tag##_swap(c_Vector_##tag* a, c_Vector_##tag* b) { \
    Value* data = a->data; a->data = b->data; b->data = data; \
} \
 \
static inline void c_vector_##tag##_destroy(c_Vector_##tag* self) { \
    Value* p = self->data; \
    size_t i = 0, n = c_vector_size(*self); \
    for (; i < n; ++p, ++i) valueDestroy(p); \
    free(_c_vector_alloced(self->data)); \
} \
 \
static inline void c_vector_##tag##_reserve(c_Vector_##tag* self, size_t cap) { \
    if (cap > c_vector_capacity(*self)) { \
        size_t len = c_vector_size(*self); \
        size_t* rep = (size_t *) realloc(_c_vector_alloced(self->data), 2 * sizeof(size_t) + cap * sizeof(Value)); \
        self->data = (Value *) (rep + 2); \
        rep[0] = len; \
        rep[1] = cap; \
    } \
} \
 \
static inline void c_vector_##tag##_clear(c_Vector_##tag* self) { \
    c_Vector_##tag cv = c_vector_initializer; \
    c_vector_##tag##_destroy(self); \
    *self = cv; \
} \
 \
 \
static inline void c_vector_##tag##_push(c_Vector_##tag* self, Value value) { \
    size_t newsize = c_vector_size(*self) + 1; \
    if (newsize > c_vector_capacity(*self)) \
        c_vector_##tag##_reserve(self, 7 + newsize * 5 / 3); \
    self->data[c_vector_size(*self)] = value; \
    _c_vector_size(*self) = newsize; \
} \
 \
static inline void c_vector_##tag##_insert(c_Vector_##tag* self, size_t pos, Value value) { \
    c_vector_##tag##_push(self, value); \
    size_t len = c_vector_size(*self); \
    memmove(&self->data[pos + 1], &self->data[pos], (len - pos - 1) * sizeof(Value)); \
    self->data[pos] = value; \
} \
 \
static inline void c_vector_##tag##_erase(c_Vector_##tag* self, size_t pos, size_t size) { \
    size_t len = c_vector_size(*self); \
    if (len) { \
        Value* p = &self->data[pos], *start = p, *end = p + size; \
        while (p != end) valueDestroy(p++); \
        memmove(start, end, (len - pos - size) * sizeof(Value)); \
        _c_vector_size(*self) -= size; \
    } \
} \
 \
 \
static inline Value c_vector_##tag##_back(c_Vector_##tag cv) { \
    return cv.data[_c_vector_size(cv) - 1]; \
} \
 \
static inline void c_vector_##tag##_pop(c_Vector_##tag* self) { \
    valueDestroy(&self->data[_c_vector_size(*self) - 1]); \
    --_c_vector_size(*self); \
} \
 \
static inline c_vector_##tag##_iter_t c_vector_##tag##_begin(c_Vector_##tag vec) { \
    c_vector_##tag##_iter_t it = {vec.data}; \
    return it; \
} \
 \
static inline c_vector_##tag##_iter_t c_vector_##tag##_next(c_vector_##tag##_iter_t it) { \
    ++it.item; \
    return it; \
} \
 \
static inline c_vector_##tag##_iter_t c_vector_##tag##_end(c_Vector_##tag vec) { \
    c_vector_##tag##_iter_t it = {vec.data + c_vector_size(vec)}; \
    return it; \
} \
typedef Value c_vector_##tag##_value_t


#define _c_vector_size(cv) ((size_t *)(cv).data)[-2]
#define _c_vector_capacity(cv) ((size_t *)(cv).data)[-1]

static inline size_t* _c_vector_alloced(void* data) {
    return data ? ((size_t *) data) - 2 : NULL;
}
static inline size_t _c_vector_safe_size(const void* data) {
    return data ? ((const size_t *) data)[-2] : 0;
}
static inline size_t _c_vector_safe_capacity(const void* data) {
    return data ? ((const size_t *) data)[-1] : 0;
}

#endif
