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

/*  Priority Queue using heap, with adapter class (normally cvec).

    #include <stc/crandom.h>
    #include <stc/cpqueue.h>
    typedef_cvec(f, float);
    typedef_cpqueue(f, cvec_f, >); // min-heap (increasing values)

    int main() {
        crand_rng32_t gen = crand_rng32_init(1234);
        crand_uniform_f32_t dist = crand_uniform_f32_init(10.0f, 100.0f);

        cpqueue_f queue = cpqueue_f_init();
        // Push ten million random numbers onto the queue.
        for (int i=0; i<10000000; ++i)
            cpqueue_f_push(&queue, crand_uniform_f32(&gen, dist));
        // Extract the 100 smallest.
        for (int i=0; i<100; ++i) {
            printf("%f ", *cpqueue_f_top(queue));
            cpqueue_f_pop(&queue);
        }
        cpqueue_f_destroy(&queue);
    }
*/

#ifndef CPQUEUE__H__
#define CPQUEUE__H__

#include "cvec.h"

#define typedef_cpqueue(X, ctype, cmpOpr) /* cmpOpr: < or > */ \
\
    typedef struct ctype cpqueue_##X; \
    typedef ctype##_value_t cpqueue_##X##_value_t; \
    typedef ctype##_rawvalue_t cpqueue_##X##_rawvalue_t; \
    typedef ctype##_input_t cpqueue_##X##_input_t; \
    STC_INLINE cpqueue_##X \
    cpqueue_##X##_init() {return ctype##_init();} \
    STC_INLINE size_t \
    cpqueue_##X##_size(cpqueue_##X pq) {return ctype##_size(pq);} \
    STC_INLINE bool \
    cpqueue_##X##_empty(cpqueue_##X pq) {return ctype##_empty(pq);} \
    STC_INLINE void \
    cpqueue_##X##_destroy(cpqueue_##X* self) {ctype##_destroy(self);} \
    STC_API void \
    cpqueue_##X##_build(cpqueue_##X* self); \
    STC_API void \
    cpqueue_##X##_erase(cpqueue_##X* self, size_t i); \
    STC_INLINE const cpqueue_##X##_value_t* \
    cpqueue_##X##_top(const cpqueue_##X* self) {return &self->data[0];} \
    STC_INLINE void \
    cpqueue_##X##_pop(cpqueue_##X* self) {cpqueue_##X##_erase(self, 0);} \
    STC_API void \
    cpqueue_##X##_push(cpqueue_##X* self, cpqueue_##X##_value_t value); \
    STC_INLINE void \
    cpqueue_##X##_emplace(cpqueue_##X* self, cpqueue_##X##_rawvalue_t rawValue) { \
        cpqueue_##X##_push(self, ctype##_value_from_raw(rawValue)); \
    } \
    STC_API void \
    cpqueue_##X##_push_n(cpqueue_##X *self, const cpqueue_##X##_input_t in[], size_t size); \
\
    implement_cpqueue(X, ctype, cmpOpr)

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
#define implement_cpqueue(X, ctype, cmpOpr) \
\
    STC_INLINE void \
    _cpqueue_##X##_sift_down(cpqueue_##X##_value_t* arr, size_t i, size_t n) { \
        size_t r = i, c = i << 1; \
        while (c <= n) { \
            if (c < n && ctype##_value_compare(&arr[c], &arr[c + 1]) cmpOpr 0) \
                ++c; \
            if (ctype##_value_compare(&arr[r], &arr[c]) cmpOpr 0) { \
                cpqueue_##X##_value_t tmp = arr[r]; arr[r] = arr[c]; arr[r = c] = tmp; \
            } else \
                return; \
            c <<= 1; \
        } \
    } \
\
    STC_API void \
    cpqueue_##X##_build(cpqueue_##X* self) { \
        size_t n = cpqueue_##X##_size(*self);  \
        cpqueue_##X##_value_t *arr = self->data - 1; \
        for (size_t k = n >> 1; k != 0; --k) \
            _cpqueue_##X##_sift_down(arr, k, n); \
    } \
\
    STC_API void \
    cpqueue_##X##_erase(cpqueue_##X* self, size_t i) { \
        size_t n = cpqueue_##X##_size(*self) - 1; \
        self->data[i] = self->data[n]; \
        ctype##_pop_back(self); \
        _cpqueue_##X##_sift_down(self->data - 1, i + 1, n); \
    } \
\
    STC_API void \
    cpqueue_##X##_push(cpqueue_##X* self, cpqueue_##X##_value_t value) { \
        ctype##_push_back(self, value); /* sift-up the value */ \
        size_t n = cpqueue_##X##_size(*self), c = n; \
        cpqueue_##X##_value_t *arr = self->data - 1; \
        for (; c > 1 && ctype##_value_compare(&arr[c >> 1], &value) cmpOpr 0; c >>= 1) \
            arr[c] = arr[c >> 1]; \
        if (c != n) arr[c] = value; \
    } \
    STC_API void \
    cpqueue_##X##_push_n(cpqueue_##X *self, const cpqueue_##X##_input_t in[], size_t size) { \
        ctype##_reserve(self, cpqueue_##X##_size(*self) + size); \
        for (size_t i=0; i<size; ++i) cpqueue_##X##_push(self, in[i]); \
    } \
\
    typedef int cpqueue_##X##_dud

#else
#define implement_cpqueue(X, ctype, cmpOpr)
#endif

#endif
