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

    #include <stc/crandom.h>
    #include <stc/cpqueue.h>
    declare_cvec(f, float);
    declare_cpqueue(f, >, cvec); // min-heap (increasing values)

    int main() {
        crandom_eng32_t gen = crandom_eng32_init(1234);
        crandom_uniform_f32_t dist = crandom_uniform_f32_init(10.0f, 100.0f);

        cpqueue_f queue = cpqueue_init;
        // Push ten million random numbers onto the queue.
        for (int i=0; i<10000000; ++i)
            cpqueue_f_push(&queue, crandom_uniform_f32(&gen, dist));
        // Extract the 100 smallest.
        for (int i=0; i<100; ++i) {
            printf("%f ", *cpqueue_f_top(&queue));
            cpqueue_f_pop(&queue);
        }
        cpqueue_f_destroy(&queue);
    }
*/

#ifndef CPQUEUE__H__
#define CPQUEUE__H__

#include "cdefs.h"

#define declare_cpqueue(tag, cmpOpr, type) /* cmpOpr: < or > */ \
 \
typedef type##_##tag cpqueue_##tag; \
typedef type##_##tag##_value_t cpqueue_##tag##_value_t; \
typedef type##_##tag##_rawvalue_t cpqueue_##tag##_rawvalue_t; \
typedef type##_##tag##_input_t cpqueue_##tag##_input_t; \
STC_INLINE cpqueue_##tag \
cpqueue_##tag##_init() {return type##_##tag##_init();} \
STC_INLINE size_t \
cpqueue_##tag##_size(cpqueue_##tag q) {return type##_##tag##_size(q);} \
STC_INLINE bool \
cpqueue_##tag##_empty(cpqueue_##tag q) {return type##_##tag##_empty(q);} \
STC_INLINE void \
cpqueue_##tag##_destroy(cpqueue_##tag* self) {type##_##tag##_destroy(self);} \
STC_API void \
cpqueue_##tag##_build(cpqueue_##tag* self); \
STC_API void \
cpqueue_##tag##_erase(cpqueue_##tag* self, size_t i); \
STC_INLINE cpqueue_##tag##_value_t* \
cpqueue_##tag##_top(cpqueue_##tag* self) {return &self->data[0];} \
STC_INLINE void \
cpqueue_##tag##_pop(cpqueue_##tag* self) {cpqueue_##tag##_erase(self, 0);} \
STC_API void \
cpqueue_##tag##_push_v(cpqueue_##tag* self, cpqueue_##tag##_value_t value); \
STC_INLINE void \
cpqueue_##tag##_push(cpqueue_##tag* self, cpqueue_##tag##_rawvalue_t rawValue) { \
    cpqueue_##tag##_push_v(self, type##_##tag##_value_from_raw(rawValue)); \
} \
STC_API void \
cpqueue_##tag##_push_n(cpqueue_##tag *self, const cpqueue_##tag##_input_t in[], size_t size); \
 \
implement_cpqueue(tag, cmpOpr, type)
    
/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
#define implement_cpqueue(tag, cmpOpr, type) \
 \
STC_INLINE void \
_cpqueue_##tag##_sift_down(cpqueue_##tag##_value_t* arr, size_t i, size_t n) { \
    size_t r = i, c = i << 1; \
    while (c <= n) { \
        if (c < n && type##_##tag##_value_compare(&arr[c], &arr[c + 1]) cmpOpr 0) \
            ++c; \
        if (type##_##tag##_value_compare(&arr[r], &arr[c]) cmpOpr 0) { \
            cpqueue_##tag##_value_t t = arr[r]; arr[r] = arr[c]; arr[r = c] = t; \
        } else \
            return; \
        c <<= 1; \
    } \
} \
 \
STC_API void \
cpqueue_##tag##_build(cpqueue_##tag* self) { \
    size_t n = cpqueue_##tag##_size(*self);  \
    cpqueue_##tag##_value_t *arr = self->data - 1; \
    for (size_t k = n >> 1; k != 0; --k) \
        _cpqueue_##tag##_sift_down(arr, k, n); \
} \
 \
STC_API void \
cpqueue_##tag##_erase(cpqueue_##tag* self, size_t i) { \
    size_t n = cpqueue_##tag##_size(*self) - 1; \
    self->data[i] = self->data[n]; \
    type##_##tag##_pop_back(self); \
    _cpqueue_##tag##_sift_down(self->data - 1, i + 1, n); \
} \
 \
STC_API void \
cpqueue_##tag##_push_v(cpqueue_##tag* self, cpqueue_##tag##_value_t value) { \
    type##_##tag##_push_back(self, value); /* sift-up the value */ \
    size_t n = cpqueue_##tag##_size(*self), c = n; \
    cpqueue_##tag##_value_t *arr = self->data - 1; \
    for (; c > 1 && type##_##tag##_value_compare(&arr[c >> 1], &value) cmpOpr 0; c >>= 1) \
        arr[c] = arr[c >> 1]; \
    if (c != n) arr[c] = value; \
} \
STC_API void \
cpqueue_##tag##_push_n(cpqueue_##tag *self, const cpqueue_##tag##_input_t in[], size_t size) { \
    type##_##tag##_reserve(self, cpqueue_##tag##_size(*self) + size); \
    for (size_t i=0; i<size; ++i) cpqueue_##tag##_push(self, in[i]); \
} \
typedef int cpqueue_##tag##_dud

#else
#define implement_cpqueue(tag, cmpOpr, type)
#endif

#endif
