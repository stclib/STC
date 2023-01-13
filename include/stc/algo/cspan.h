/* MIT License
 *
 * Copyright (c) 2023 Tyge Løvset
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
    IntSpan span = cspan_from(array);
    
    c_FOREACH (i, IntSpan, span)
        printf(" %d", *i.ref);
    puts("");
    
    // use a temporary IntSpan object.
    c_FORFILTER (i, IntSpan, cspan_object(IntSpan, {10, 20, 30, 23, 22, 21})
                  , c_FLT_SKIPWHILE(i, *i.ref < 25)
                 && (*i.ref & 1) == 0 // even only
                  , c_FLT_TAKE(i, 2)) // break after 2
        printf(" %d", *i.ref);
    puts("");
}
*/
#ifndef STC_CSPAN_H_INCLUDED
#define STC_CSPAN_H_INCLUDED

#include <stc/ccommon.h>

#define cspan_object(C, ...) \
    ((C){.data = (C##_value[])__VA_ARGS__, \
         .size = sizeof((C##_value[])__VA_ARGS__)/sizeof(C##_value)})

#define cspan_from(data) \
    {data + c_STATIC_ASSERT(sizeof(data) != sizeof(void*)), c_ARRAYLEN(data)}
#define cspan_make(data, size) \
    {data + c_STATIC_ASSERT(c_ARRAYLEN(data) >= size || sizeof(data) == sizeof(void*)), size}
#define cspan_size(self) ((size_t)(self)->size)

#define using_cspan(Self, T) \
    typedef T Self##_raw, Self##_value; \
    typedef struct { Self##_value *data; size_t size; } Self; \
    typedef struct { Self##_value *ref, *end; } Self##_iter; \
    \
    STC_INLINE Self##_value* Self##_at(const Self* self, size_t idx) \
        { assert(idx < self->size); return self->data + idx; } \
    STC_INLINE Self##_iter Self##_begin(const Self* self) { \
        Self##_iter it = {self->data, self->data + self->size}; \
        return it; \
    } \
    STC_INLINE Self##_iter Self##_end(const Self* self) { \
        Self##_iter it = {NULL, self->data + self->size}; \
        return it; \
    } \
    STC_INLINE void Self##_next(Self##_iter* it) \
        { if (++it->ref == it->end) it->ref = NULL; } \
    struct stc_nostruct

#endif
