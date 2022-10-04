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
#include <stc/views.h>
using_listview(IView, int);

int main()
{
    int array[] = {1, 2, 3, 4, 5};
    IView iv = IView_init(array, c_arraylen(array));
    
    c_foreach (i, IView, iv) printf(" %d", *i.ref);
    puts("");

    c_forfilter (i, IView, c_listview(IView, {10, 20, 30, 22, 23})
                  , c_flt_skipwhile(i, *i.ref < 25)
                  , c_flt_take(i, 2))
        printf(" %d", *i.ref);
    puts("");

    crange r1 = crange_init(80, 90);
    c_foreach (i, crange, r1) printf(" %lld", *i.ref);
    puts("");

    c_foreach (i, crange, c_range(0, 100, 8)) printf(" %lld", *i.ref);
    puts("");
}
*/
#ifndef STC_VIEWS_H_INCLUDED
#define STC_VIEWS_H_INCLUDED

#include <stc/ccommon.h>

#define c_listview(C, ...) \
    ((C){.data = (C##_value[])__VA_ARGS__, \
         .size = sizeof((C##_value[])__VA_ARGS__)/sizeof(C##_value)})

#define using_listview(Self, T) \
typedef T Self##_raw; typedef const Self##_raw Self##_value; \
typedef struct { Self##_value *data; size_t size; } Self; \
typedef struct { Self##_value *ref, *end; } Self##_iter; \
 \
STC_INLINE Self Self##_init(Self##_value* data, size_t size) \
    { Self me = {.data=data, .size=size}; return me; } \
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


#define c_range(...) \
    (*(crange[]){crange_init(__VA_ARGS__)})
#define crange_MAX INT64_MAX

typedef long long crange_value;
typedef struct { crange_value start, end, step, val; } crange;
typedef struct { crange_value *ref, end, step; } crange_iter;

#define crange_init(...) c_MACRO_OVERLOAD(crange_init, __VA_ARGS__)
#define crange_init1(stop) crange_init3(0, stop, 1)
#define crange_init2(start, stop) crange_init3(start, stop, 1)

STC_INLINE crange crange_init3(crange_value start, crange_value stop, crange_value step)
    { crange r = {start, stop - (step > 0), step}; return r; }

STC_INLINE crange_iter crange_begin(crange* self)
    { self->val = self->start; crange_iter it = {&self->val, self->end, self->step}; return it; }

STC_INLINE crange_iter crange_end(crange* self) 
    { crange_iter it = {NULL}; return it; }

STC_INLINE void crange_next(crange_iter* it) 
    { *it->ref += it->step; if ((it->step > 0) == (*it->ref > it->end)) it->ref = NULL; }

#endif
