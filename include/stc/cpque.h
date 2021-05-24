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

        c_forvar (cpque_f queue = cpque_f_init(), cpque_f_del(&queue))
        {
            // Push ten million random numbers onto the queue.
            for (int i=0; i<10000000; ++i)
                cpque_f_push(&queue, stc64_uniformf(&rng, dist));

            // Extract the 100 smallest.
            for (int i=0; i<100; ++i) {
                printf("%f ", *cpque_f_top(queue));
                cpque_f_pop(&queue);
            }
        }
    }
*/
#ifndef CPQUEUE_H_INCLUDED
#define CPQUEUE_H_INCLUDED

#include "cvec.h"

#define using_cpque(...) c_MACRO_OVERLOAD(using_cpque, __VA_ARGS__)

#define using_cpque_2(X, ctype) \
            _c_using_cpque(cpque_##X, ctype, ctype##_value_compare)
#define using_cpque_3(X, ctype, valueCompare) \
            _c_using_cpque(cpque_##X, ctype, valueCompare)

#define _c_using_cpque(CX, ctype, valueCompare) \
    typedef ctype CX; \
    typedef ctype##_value_t CX##_value_t; \
    typedef ctype##_rawvalue_t CX##_rawvalue_t; \
\
    STC_INLINE CX           CX##_init(void) {return ctype##_init();} \
    STC_INLINE CX           CX##_clone(CX pq) {return ctype##_clone(pq);} \
    STC_INLINE CX##_value_t CX##_value_clone(CX##_value_t val) \
                                {return ctype##_value_clone(val);} \
    STC_INLINE void         CX##_clear(CX* self) {ctype##_clear(self);} \
    STC_INLINE void         CX##_del(CX* self) {ctype##_del(self);} \
\
    STC_INLINE size_t       CX##_size(CX pq) {return ctype##_size(pq);} \
    STC_INLINE bool         CX##_empty(CX pq) {return ctype##_empty(pq);} \
    \
    STC_API void            CX##_make_heap(CX* self); \
    STC_API void            CX##_erase_at(CX* self, size_t idx); \
    STC_INLINE \
    const CX##_value_t*     CX##_top(const CX* self) {return &self->data[0];} \
    STC_INLINE void         CX##_pop(CX* self) {CX##_erase_at(self, 0);} \
    \
    STC_API void            CX##_push(CX* self, CX##_value_t value); \
    STC_INLINE void         CX##_emplace(CX* self, CX##_rawvalue_t raw) \
                                {CX##_push(self, ctype##_value_fromraw(raw));} \
    STC_API void            CX##_emplace_items(CX *self, const CX##_rawvalue_t arr[], size_t n); \
\
    _c_implement_cpque(CX, ctype, valueCompare) \
    struct stc_trailing_semicolon


/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)

#define _c_implement_cpque(CX, ctype, valueCompare) \
\
    STC_INLINE void \
    CX##_sift_down_(CX##_value_t* arr, size_t i, size_t n) { \
        size_t r = i, c = i << 1; \
        while (c <= n) { \
            c += (c < n && valueCompare(&arr[c], &arr[c + 1]) < 0); \
            if (valueCompare(&arr[r], &arr[c]) < 0) { \
                CX##_value_t tmp = arr[r]; arr[r] = arr[c]; arr[r = c] = tmp; \
            } else \
                return; \
            c <<= 1; \
        } \
    } \
\
    STC_API void \
    CX##_make_heap(CX* self) { \
        size_t n = CX##_size(*self); \
        CX##_value_t *arr = self->data - 1; \
        for (size_t k = n >> 1; k != 0; --k) \
            CX##_sift_down_(arr, k, n); \
    } \
\
    STC_API void \
    CX##_erase_at(CX* self, size_t idx) { \
        size_t n = CX##_size(*self) - 1; \
        self->data[idx] = self->data[n]; \
        ctype##_pop_back(self); \
        CX##_sift_down_(self->data - 1, idx + 1, n); \
    } \
\
    STC_API void \
    CX##_push(CX* self, CX##_value_t value) { \
        ctype##_push_back(self, value); /* sift-up the value */ \
        size_t n = CX##_size(*self), c = n; \
        CX##_value_t *arr = self->data - 1; \
        for (; c > 1 && valueCompare(&arr[c >> 1], &value) < 0; c >>= 1) \
            arr[c] = arr[c >> 1]; \
        if (c != n) arr[c] = value; \
    } \
\
    STC_API void \
    CX##_emplace_items(CX *self, const CX##_rawvalue_t arr[], size_t n) { \
        for (size_t i = 0; i < n; ++i) \
            CX##_push(self, ctype##_value_fromraw(arr[i])); \
    } \

#else
#define _c_implement_cpque(CX, ctype, valueCompare)
#endif

#endif
