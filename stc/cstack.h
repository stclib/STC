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

/*  Stack adapter (normally uses cvec).

    #include <stc/cstack.h>
    #include <stdio.h>

    c_cvec(i, int);
    c_cstack(i, cvec_i);

    int main() {
        cstack_i stack = cstack_i_init();

        for (int i=0; i<100; ++i)
            cstack_i_push(&stack, i*i);

        for (int i=0; i<90; ++i)
            cstack_i_pop(&stack);

        printf("top: %d\n", *cstack_i_top(&stack));
    }
*/

#ifndef CSTACK__H__
#define CSTACK__H__

#include "cvec.h"

#define c_cstack(X, ctype) \
\
    typedef struct ctype cstack_##X; \
    typedef ctype##_value_t cstack_##X##_value_t; \
    typedef ctype##_rawvalue_t cstack_##X##_rawvalue_t; \
    typedef ctype##_input_t cstack_##X##_input_t; \
    STC_INLINE cstack_##X \
    cstack_##X##_init() {return ctype##_init();} \
    STC_INLINE void \
    cstack_##X##_destroy(cstack_##X* self) {ctype##_destroy(self);} \
    STC_INLINE size_t \
    cstack_##X##_size(cstack_##X pq) {return ctype##_size(pq);} \
    STC_INLINE bool \
    cstack_##X##_empty(cstack_##X pq) {return ctype##_empty(pq);} \
    STC_INLINE cstack_##X##_value_t* \
    cstack_##X##_top(cstack_##X* self) {return ctype##_back(self);} \
    STC_INLINE void \
    cstack_##X##_pop(cstack_##X* self) {ctype##_pop_back(self);} \
    STC_API void \
    cstack_##X##_push(cstack_##X* self, ctype##_value_t value) { \
        ctype##_push_back(self, value); \
    } \
    STC_INLINE void \
    cstack_##X##_emplace(cstack_##X* self, cstack_##X##_rawvalue_t rawValue) { \
        ctype##_emplace_back(self, rawValue); \
    } \
    STC_API void \
    cstack_##X##_push_n(cstack_##X *self, const cstack_##X##_input_t in[], size_t size) { \
        ctype##_push_n(self, in, size); \
    } \
    typedef ctype##_iter_t cstack_##X##_iter_t; \
    STC_INLINE cstack_##X##_iter_t \
    cstack_##X##_begin(cstack_##X* self) {return ctype##_begin(self);} \
    STC_INLINE cstack_##X##_iter_t \
    cstack_##X##_end(cstack_##X* self) {return ctype##_end(self);} \
    STC_INLINE void \
    cstack_##X##_next(cstack_##X##_iter_t* it) {ctype##_next(it);} \
    STC_INLINE cstack_##X##_value_t* \
    cstack_##X##_itval(cstack_##X##_iter_t it) {return ctype##_itval(it);} \
\
    typedef int cstack_##X##_dud

#endif
