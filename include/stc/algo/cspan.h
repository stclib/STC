/* MIT License
 *
 * Copyright (c) 2022 Tyge Løvset, NORCE, www.norceresearch.no
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
/* 
#include <stdio.h>
#include <stc/algo/cspan.h>
using_cspan(IntSpan, int);

int main()
{
    int array[] = {1, 2, 3, 4, 5};
    IntSpan iv = {array, c_arraylen(array)};
    
    c_foreach (i, IntSpan, iv)
        printf(" %d", *i.ref);
    puts("");
    
    // use a temporary IntSpan object.
    c_forfilter (i, IntSpan, cspan_LITERAL(IntSpan, {10, 20, 30, 23, 22, 21})
                  , c_flt_skipwhile(i, *i.ref < 25)
                 && (*i.ref & 1) == 0 // even only
                  , c_flt_take(i, 2)) // break after 2
        printf(" %d", *i.ref);
    puts("");
}
*/
#ifndef STC_CSPAN_H_INCLUDED
#define STC_CSPAN_H_INCLUDED

#include <stc/ccommon.h>

#define cspan_LITERAL(C, ...) \
    ((C){.data = (C##_value[])__VA_ARGS__, \
         .size = sizeof((C##_value[])__VA_ARGS__)/sizeof(C##_value)})

#define using_cspan(Self, T) \
typedef T Self##_raw; typedef const Self##_raw Self##_value; \
typedef struct { Self##_value *data; size_t size; } Self; \
typedef struct { Self##_value *ref, *end; } Self##_iter; \
 \
STC_INLINE Self##_value* Self##_at(const Self* self, size_t idx) \
    { assert(idx < self->size); return self->data + idx; } \
 \
STC_INLINE Self##_iter Self##_begin(const Self* self) { \
    Self##_iter it = {self->data, self->data + self->size}; \
    return it; \
} \
 \
STC_INLINE Self##_iter Self##_end(const Self* self) { \
    Self##_iter it = {NULL, self->data + self->size}; \
    return it; \
} \
 \
STC_INLINE void Self##_next(Self##_iter* it) \
    { if (++it->ref == it->end) it->ref = NULL; } \
struct stc_nostruct

#endif
