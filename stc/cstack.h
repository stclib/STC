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

    declare_cvec(i, int);
    declare_cstack(i, cvec_i);

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

#define declare_cstack(tag, ctype) \
 \
typedef ctype cstack_##tag; \
typedef ctype##_value_t cstack_##tag##_value_t; \
typedef ctype##_rawvalue_t cstack_##tag##_rawvalue_t; \
typedef ctype##_input_t cstack_##tag##_input_t; \
STC_INLINE cstack_##tag \
cstack_##tag##_init() {return ctype##_init();} \
STC_INLINE void \
cstack_##tag##_destroy(cstack_##tag* self) {ctype##_destroy(self);} \
STC_INLINE size_t \
cstack_##tag##_size(cstack_##tag pq) {return ctype##_size(pq);} \
STC_INLINE bool \
cstack_##tag##_empty(cstack_##tag pq) {return ctype##_empty(pq);} \
STC_INLINE cstack_##tag##_value_t* \
cstack_##tag##_top(cstack_##tag* self) {return ctype##_back(self);} \
STC_INLINE void \
cstack_##tag##_pop(cstack_##tag* self) {ctype##_pop_back(self);} \
STC_API void \
cstack_##tag##_push_v(cstack_##tag* self, ctype##_value_t value) { \
    ctype##_push_back_v(self, value); \
} \
STC_INLINE void \
cstack_##tag##_push(cstack_##tag* self, cstack_##tag##_rawvalue_t rawValue) { \
    ctype##_push_back(self, rawValue); \
} \
STC_API void \
cstack_##tag##_push_n(cstack_##tag *self, const cstack_##tag##_input_t in[], size_t size) { \
    ctype##_push_n(self, in, size); \
} \
 \
typedef ctype##_iter_t cstack_##tag##_iter_t; \
STC_INLINE cstack_##tag##_iter_t \
cstack_##tag##_begin(cstack_##tag* self) {return ctype##_begin(self);} \
STC_INLINE void \
cstack_##tag##_next(cstack_##tag##_iter_t* it) {ctype##_next(it);} \
STC_INLINE cstack_##tag##_value_t* \
cstack_##tag##_itval(cstack_##tag##_iter_t it) {return ctype##_itval(it);} \
 \
typedef int cstack_##tag##_dud

#endif
