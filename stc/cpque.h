/* MIT License
 *
 * Copyright (c) 2021 Tyge Løvset, NORCE, www.norceresearch.no
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

/*  Priority-Queue adapter (implemented as heap), default uses cvec.

    #include <stc/crandom.h>
    #include <stc/cpque.h>
    using_cvec(f, float);
    using_cpque(f, cvec_f, -c_default_compare); // min-heap (increasing values)

    int main() {
        stc64_t rng = stc64_init(1234);
        stc64_uniformf_t dist = stc64_uniformf_init(10.0f, 100.0f);

        cpque_f queue = cpque_f_init();
        // Push ten million random numbers onto the queue.
        for (int i=0; i<10000000; ++i)
            cpque_f_push(&queue, stc64_uniformf(&rng, dist));
        // Extract the 100 smallest.
        for (int i=0; i<100; ++i) {
            printf("%f ", *cpque_f_top(queue));
            cpque_f_pop(&queue);
        }
        cpque_f_del(&queue);
    }
*/
#ifndef CPQUEUE_H_INCLUDED
#define CPQUEUE_H_INCLUDED

#include "cvec.h"

#define using_cpque(...) c_MACRO_OVERLOAD(using_cpque, __VA_ARGS__)

#define using_cpque_2(X, ctype) \
    using_cpque_3(X, ctype, ctype##_value_compare)

#define using_cpque_3(X, ctype, valueCompare) \
    typedef ctype##_t cpque_##X; \
    typedef ctype##_value_t cpque_##X##_value_t; \
    typedef ctype##_rawvalue_t cpque_##X##_rawvalue_t; \
\
    STC_INLINE cpque_##X \
    cpque_##X##_init(void) {return ctype##_init();} \
    STC_INLINE cpque_##X \
    cpque_##X##_clone(cpque_##X pq) {return ctype##_clone(pq);} \
    STC_INLINE cpque_##X##_value_t \
    cpque_##X##_value_clone(cpque_##X##_value_t val) {return ctype##_value_clone(val);} \
    STC_INLINE void \
    cpque_##X##_clear(cpque_##X* self) {ctype##_clear(self);} \
    STC_INLINE void \
    cpque_##X##_del(cpque_##X* self) {ctype##_del(self);} \
\
    STC_INLINE size_t \
    cpque_##X##_size(cpque_##X pq) {return ctype##_size(pq);} \
    STC_INLINE bool \
    cpque_##X##_empty(cpque_##X pq) {return ctype##_empty(pq);} \
    STC_API void \
    cpque_##X##_make_heap(cpque_##X* self); \
    STC_API void \
    cpque_##X##_erase_at(cpque_##X* self, size_t i); \
    STC_INLINE const cpque_##X##_value_t* \
    cpque_##X##_top(const cpque_##X* self) {return &self->data[0];} \
    STC_INLINE void \
    cpque_##X##_pop(cpque_##X* self) {cpque_##X##_erase_at(self, 0);} \
    STC_API void \
    cpque_##X##_push(cpque_##X* self, cpque_##X##_value_t value); \
    STC_INLINE void \
    cpque_##X##_emplace(cpque_##X* self, cpque_##X##_rawvalue_t raw) { \
        cpque_##X##_push(self, ctype##_value_fromraw(raw)); \
    } \
    STC_API void \
    cpque_##X##_emplace_n(cpque_##X *self, const cpque_##X##_rawvalue_t arr[], size_t size); \
\
    _c_implement_cpque(X, ctype, valueCompare) \
    typedef cpque_##X cpque_##X##_t


/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)

#define _c_implement_cpque(X, ctype, valueCompare) \
\
    STC_INLINE void \
    _cpque_##X##_sift_down(cpque_##X##_value_t* arr, size_t i, size_t n) { \
        size_t r = i, c = i << 1; \
        while (c <= n) { \
            c += (c < n && valueCompare(&arr[c], &arr[c + 1]) < 0); \
            if (valueCompare(&arr[r], &arr[c]) < 0) { \
                cpque_##X##_value_t tmp = arr[r]; arr[r] = arr[c]; arr[r = c] = tmp; \
            } else \
                return; \
            c <<= 1; \
        } \
    } \
\
    STC_API void \
    cpque_##X##_make_heap(cpque_##X* self) { \
        size_t n = cpque_##X##_size(*self); \
        cpque_##X##_value_t *arr = self->data - 1; \
        for (size_t k = n >> 1; k != 0; --k) \
            _cpque_##X##_sift_down(arr, k, n); \
    } \
\
    STC_API void \
    cpque_##X##_erase_at(cpque_##X* self, size_t i) { \
        size_t n = cpque_##X##_size(*self) - 1; \
        self->data[i] = self->data[n]; \
        ctype##_pop_back(self); \
        _cpque_##X##_sift_down(self->data - 1, i + 1, n); \
    } \
\
    STC_API void \
    cpque_##X##_push(cpque_##X* self, cpque_##X##_value_t value) { \
        ctype##_push_back(self, value); /* sift-up the value */ \
        size_t n = cpque_##X##_size(*self), c = n; \
        cpque_##X##_value_t *arr = self->data - 1; \
        for (; c > 1 && valueCompare(&arr[c >> 1], &value) < 0; c >>= 1) \
            arr[c] = arr[c >> 1]; \
        if (c != n) arr[c] = value; \
    } \
    STC_API void \
    cpque_##X##_emplace_n(cpque_##X *self, const cpque_##X##_rawvalue_t arr[], size_t size) { \
        for (size_t i=0; i<size; ++i) cpque_##X##_push(self, arr[i]); \
    }

#else
#define _c_implement_cpque(X, ctype, valueCompare)
#endif

#endif
