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

#define _c_using_cstack(Self, ctype) \
    typedef ctype Self; \
    typedef ctype##_value_t cx_value_t; \
    typedef ctype##_rawvalue_t cx_rawvalue_t; \
    typedef ctype##_iter_t cx_iter_t; \
\
    STC_INLINE Self               cx_memb(_init)(void) { return ctype##_init(); } \
    STC_INLINE Self               cx_memb(_clone)(Self st) { return ctype##_clone(st); } \
    STC_INLINE cx_value_t     cx_memb(_value_clone)(cx_value_t val) \
                                    { return ctype##_value_clone(val); } \
    STC_INLINE void             cx_memb(_clear)(Self* self) {ctype##_clear(self); } \
    STC_INLINE void             cx_memb(_del)(Self* self) {ctype##_del(self); } \
\
    STC_INLINE size_t           cx_memb(_size)(Self st) { return ctype##_size(st); } \
    STC_INLINE bool             cx_memb(_empty)(Self st) { return ctype##_empty(st); } \
    STC_INLINE cx_value_t*    cx_memb(_top)(const Self* self) { return ctype##_back(self); } \
\
    STC_INLINE void             cx_memb(_pop)(Self* self) {ctype##_pop_back(self); } \
    STC_INLINE void             cx_memb(_push)(Self* self, ctype##_value_t value) \
                                    {ctype##_push_back(self, value); } \
    STC_INLINE void             cx_memb(_emplace)(Self* self, cx_rawvalue_t raw) \
                                    {ctype##_emplace_back(self, raw); } \
    STC_INLINE void             cx_memb(_emplace_items)(Self *self, const cx_rawvalue_t arr[], size_t n) \
                                    {ctype##_emplace_items(self, arr, n); } \
\
    STC_INLINE cx_iter_t      cx_memb(_begin)(const Self* self) { return ctype##_begin(self); } \
    STC_INLINE cx_iter_t      cx_memb(_end)(const Self* self) { return ctype##_end(self); } \
    STC_INLINE void             cx_memb(_next)(cx_iter_t* it) {ctype##_next(it); } \
    struct stc_trailing_semicolon

#endif
