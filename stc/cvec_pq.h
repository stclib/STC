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

/*  Priority Queue using cvec as heap.

    #include <stc/crand.h>
    #include <stc/cvec_pq.h>
    declare_cvec(f, float);
    declare_cvec_pqueue(f, >); // min-heap (increasing values)

    int main() {
        crand_eng32_t gen = crand_eng32_init(1234);
        crand_uniform_f32_t dist = crand_uniform_f32_init(10.0f, 100.0f);

        cvec_f queue = cvec_init;
        // Push ten million random numbers onto the queue.
        for (int i=0; i<10000000; ++i)
            cvec_f_pqueue_push(&queue, crand_uniform_f32(&gen, dist));
        // Extract the 100 smallest.
        for (int i=0; i<100; ++i) {
            printf("%f ", cvec_f_pqueue_top(&queue));
            cvec_f_pqueue_pop(&queue);
        }
        cvec_f_destroy(&queue);
    }
*/

#ifndef CVEC_PQ__H__
#define CVEC_PQ__H__

#include "cvec.h"

#define declare_cvec_pqueue(tag, cmpOpr) /* < or > */ \
 \
STC_API void \
cvec_##tag##_pqueue_build(cvec_##tag* self); \
STC_API void \
cvec_##tag##_pqueue_erase(cvec_##tag* self, size_t i); \
STC_INLINE cvec_##tag##_value_t \
cvec_##tag##_pqueue_top(cvec_##tag* self) {return self->data[0];} \
STC_INLINE void \
cvec_##tag##_pqueue_pop(cvec_##tag* self) {cvec_##tag##_pqueue_erase(self, 0);} \
STC_API void \
cvec_##tag##_pqueue_push(cvec_##tag* self, cvec_##tag##_value_t value); \
STC_API void \
cvec_##tag##_pqueue_push_n(cvec_##tag *self, const cvec_##tag##_value_t in[], size_t size); \
 \
implement_cvec_pqueue(tag, cmpOpr) \
typedef cvec_##tag##_value_t cvec_##tag##_pqueue_input_t
    
/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
#define implement_cvec_pqueue(tag, cmpOpr) \
 \
STC_INLINE void \
_cvec_##tag##_pqueue_sift_down(cvec_##tag##_value_t* arr, size_t i, size_t n) { \
    size_t r = i, c = i << 1; \
    while (c <= n) { \
        if (c < n && cvec_##tag##_sort_compare(&arr[c], &arr[c + 1]) cmpOpr 0) \
            ++c; \
        if (cvec_##tag##_sort_compare(&arr[r], &arr[c]) cmpOpr 0) { \
            cvec_##tag##_value_t t = arr[r]; arr[r] = arr[c]; arr[r = c] = t; \
        } else \
            return; \
        c <<= 1; \
    } \
} \
 \
STC_API void \
cvec_##tag##_pqueue_build(cvec_##tag* self) { \
    size_t n = cvec_size(*self);  \
    cvec_##tag##_value_t *arr = self->data - 1; \
    for (size_t k = n >> 1; k != 0; --k) \
        _cvec_##tag##_pqueue_sift_down(arr, k, n); \
} \
 \
STC_API void \
cvec_##tag##_pqueue_erase(cvec_##tag* self, size_t i) { \
    size_t n = cvec_size(*self) - 1; \
    self->data[i] = self->data[n]; \
    cvec_##tag##_pop_back(self); \
    _cvec_##tag##_pqueue_sift_down(self->data - 1, i + 1, n); \
} \
 \
STC_API void \
cvec_##tag##_pqueue_push(cvec_##tag* self, cvec_##tag##_value_t value) { \
    cvec_##tag##_push_back(self, value); /* sift-up the value */ \
    size_t n = cvec_size(*self), c = n; \
    cvec_##tag##_value_t *arr = self->data - 1; \
    for (; c > 1 && cvec_##tag##_sort_compare(&arr[c >> 1], &value) cmpOpr 0; c >>= 1) \
        arr[c] = arr[c >> 1]; \
    if (c != n) arr[c] = value; \
} \
STC_API void \
cvec_##tag##_pqueue_push_n(cvec_##tag *self, const cvec_##tag##_value_t in[], size_t size) { \
    cvec_##tag##_reserve(self, cvec_size(*self) + size); \
    for (size_t i=0; i<size; ++i) cvec_##tag##_pqueue_push(self, in[i]); \
}

#else
#define implement_cvec_pqueue(tag, cmpOpr)
#endif

#endif
