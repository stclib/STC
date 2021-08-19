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
#ifndef CQUEUE_H_INCLUDED
#define CQUEUE_H_INCLUDED

/*  Queue adapter, default uses clist.

    #include <stc/crandom.h>
    #include <stc/cqueue.h>
    using_cdeq(i, int);
    using_cqueue(i, cdeq_i);

    int main() {
        int n = 10000000;
        stc64_t rng = stc64_init(1234);
        stc64_uniform_t dist = stc64_uniform_init(rng, 0, n);

        c_forvar (cqueue_i queue = cqueue_i_init(), cqueue_i_del(&queue))
        {
            // Push ten million random numbers onto the queue.
            for (int i=0; i<n; ++i)
                cqueue_i_push(&queue, stc64_uniform(&dist));

            // Push or pop on the queue ten million times
            printf("%d\n", n);
            for (int i=n; i>0; --i) {
                int r = stc64_uniform(&dist);
                if (r & 1)
                    ++n, cqueue_i_push(&queue, r);
                else
                    --n, cqueue_i_pop(&queue);
            }
        }
        printf("%d\n", n);
    }
*/
#include "cdeq.h"

#define using_cqueue(X, ctype) \
            _c_using_cqueue(cqueue_##X, ctype)

#define _c_using_cqueue(CX, ctype) \
    typedef struct { ctype rep; size_t size; } CX; \
    typedef ctype##_value_t CX##_value_t; \
    typedef ctype##_rawvalue_t CX##_rawvalue_t; \
    typedef ctype##_iter_t CX##_iter_t; \
\
    STC_INLINE CX               CX##_init(void) {return c_make(CX){ctype##_init(), 0};} \
    STC_INLINE CX               CX##_clone(CX q) {return c_make(CX){ctype##_clone(q.rep), q.size};} \
    STC_INLINE CX##_value_t     CX##_value_clone(CX##_value_t val) \
                                    {return ctype##_value_clone(val);} \
    STC_INLINE void             CX##_clear(CX* self) {ctype##_clear(&self->rep); self->size = 0;} \
    STC_INLINE void             CX##_del(CX* self) {ctype##_del(&self->rep);} \
\
    STC_INLINE size_t           CX##_size(CX q) {return q.size;} \
    STC_INLINE bool             CX##_empty(CX q) {return q.size == 0;} \
    STC_INLINE CX##_value_t*    CX##_front(const CX* self) {return ctype##_front(&self->rep);} \
    STC_INLINE CX##_value_t*    CX##_back(const CX* self) {return ctype##_back(&self->rep);} \
\
    STC_INLINE void             CX##_pop(CX* self) {ctype##_pop_front(&self->rep); --self->size;} \
    STC_INLINE void             CX##_push(CX* self, ctype##_value_t value) \
                                    {ctype##_push_back(&self->rep, value); ++self->size;} \
    STC_INLINE void             CX##_emplace(CX* self, CX##_rawvalue_t raw) \
                                    {ctype##_emplace_back(&self->rep, raw); ++self->size;} \
    STC_INLINE void             CX##_emplace_items(CX *self, const CX##_rawvalue_t arr[], size_t n) \
                                    {ctype##_emplace_items(&self->rep, arr, n); self->size += n;} \
\
    STC_INLINE CX##_iter_t      CX##_begin(const CX* self) {return ctype##_begin(&self->rep);} \
    STC_INLINE CX##_iter_t      CX##_end(const CX* self) {return ctype##_end(&self->rep);} \
    STC_INLINE void             CX##_next(CX##_iter_t* it) {ctype##_next(it);} \
    struct stc_trailing_semicolon

#endif
