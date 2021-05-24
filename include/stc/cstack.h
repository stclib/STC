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
#ifndef CSTACK_H_INCLUDED
#define CSTACK_H_INCLUDED

/*  Stack adapter, default uses cvec.

    #include <stc/cstack.h>
    #include <stdio.h>

    using_cvec(i, int);
    using_cstack(i, cvec_i);

    int main() {
        c_forvar (cstack_i stack = cstack_i_init(), cstack_i_del(&stack))
        {
            for (int i=0; i<100; ++i)
                cstack_i_push(&stack, i*i);

            for (int i=0; i<90; ++i)
                cstack_i_pop(&stack);

            printf("top: %d\n", *cstack_i_top(&stack));
        }
    }
*/
#include "cvec.h"

#define using_cstack(X, ctype) \
            _c_using_cstack(cstack_##X, ctype)

#define _c_using_cstack(CX, ctype) \
    typedef ctype CX; \
    typedef ctype##_value_t CX##_value_t; \
    typedef ctype##_rawvalue_t CX##_rawvalue_t; \
    typedef ctype##_iter_t CX##_iter_t; \
\
    STC_INLINE CX               CX##_init(void) {return ctype##_init();} \
    STC_INLINE CX               CX##_clone(CX st) {return ctype##_clone(st);} \
    STC_INLINE CX##_value_t     CX##_value_clone(CX##_value_t val) \
                                    {return ctype##_value_clone(val);} \
    STC_INLINE void             CX##_clear(CX* self) {ctype##_clear(self);} \
    STC_INLINE void             CX##_del(CX* self) {ctype##_del(self);} \
\
    STC_INLINE size_t           CX##_size(CX st) {return ctype##_size(st);} \
    STC_INLINE bool             CX##_empty(CX st) {return ctype##_empty(st);} \
    STC_INLINE CX##_value_t*    CX##_top(const CX* self) {return ctype##_back(self);} \
\
    STC_INLINE void             CX##_pop(CX* self) {ctype##_pop_back(self);} \
    STC_INLINE void             CX##_push(CX* self, ctype##_value_t value) \
                                    {ctype##_push_back(self, value);} \
    STC_INLINE void             CX##_emplace(CX* self, CX##_rawvalue_t raw) \
                                    {ctype##_emplace_back(self, raw);} \
    STC_INLINE void             CX##_emplace_items(CX *self, const CX##_rawvalue_t arr[], size_t n) \
                                    {ctype##_emplace_items(self, arr, n);} \
\
    STC_INLINE CX##_iter_t      CX##_begin(const CX* self) {return ctype##_begin(self);} \
    STC_INLINE CX##_iter_t      CX##_end(const CX* self) {return ctype##_end(self);} \
    STC_INLINE void             CX##_next(CX##_iter_t* it) {ctype##_next(it);} \
    struct stc_trailing_semicolon

#endif
