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

/* Priority queue using CVec as heap. */

#ifndef CVECQUE__H__
#define CVECQUE__H__

#include "cvec.h"

/* Requires declare_CVec(tag, ...) to be declared */
#define declare_CVecque(tag) \
 \
STC_API void \
cvec_##tag##_queueify(CVec_##tag* self); \
STC_API CVecValue_##tag \
cvecque_##tag##_erase(CVec_##tag* self, size_t i); \
STC_INLINE CVecValue_##tag \
cvecque_##tag##_top(CVec_##tag* self) {return self->data[0];} \
STC_INLINE CVecValue_##tag \
cvecque_##tag##_pop(CVec_##tag* self) {return cvecque_##tag##_erase(self, 0);} \
STC_API void \
cvecque_##tag##_push(CVec_##tag* self, CVecValue_##tag value); \
 \
implement_CVecque(tag) \
typedef CVec_##tag CVecdeque_##tag
    
/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
#define implement_CVecque(tag) \
 \
STC_INLINE void \
_cvecque_##tag##_siftDown(CVecValue_##tag* arr, size_t i, size_t n) { \
    size_t r = i, c = i << 1; \
    while (c <= n) { \
        if (c < n && arr[c] > arr[c + 1]) \
            ++c; \
        if (arr[r] > arr[c]) { \
            CVecValue_##tag t = arr[r]; arr[r] = arr[c]; arr[r = c] = t; \
        } else \
            return; \
        c <<= 1; \
    } \
} \
 \
STC_API CVecValue_##tag \
cvecque_##tag##_erase(CVec_##tag* self, size_t i) { \
    CVecValue_##tag ret = self->data[i]; \
    self->data[i] = cvec_##tag##_back(*self); \
    cvec_##tag##_popBack(self); \
    _cvecque_##tag##_siftDown(self->data - 1, i + 1, cvec_size(*self)); \
    return ret; \
} \
 \
STC_API void \
cvecque_##tag##_push(CVec_##tag* self, CVecValue_##tag value) { \
    cvec_##tag##_pushBack(self, value); \
    size_t n = cvec_size(*self), i = n; \
    CVecValue_##tag *arr = self->data - 1; \
    for (; i > 1 && arr[i >> 1] > value; i >>= 1) \
        arr[i] = arr[i >> 1]; \
    arr[i] = value; \
} \
 \
STC_API void \
cvec_##tag##_queueify(CVec_##tag* self) { \
    size_t n = cvec_size(*self);  \
    CVecValue_##tag *arr = self->data - 1; \
    for (size_t i = n >> 1; i; --i) \
        _cvecque_##tag##_siftDown(arr, i, n); \
}

#else
#define implement_CVecque(tag)
#endif

#endif
