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
using_clview(IView, int);

int main()
{
    int array[] = {1, 2, 3, 4, 5};
    IView iv = {array, c_arraylen(array)};
    
    c_foreach (i, IView, iv)
        printf(" %d", *i.ref);
    puts("");
    
    // use a temporary IView object.
    c_forfilter (i, IView, clview_literal(IView, {10, 20, 30, 23, 22, 21})
                  , c_flt_skipwhile(i, *i.ref < 25)
                 && (*i.ref & 1) == 0 // even only
                  , c_flt_take(i, 2)) // break after 2
        printf(" %d", *i.ref);
    puts("");

    // crange:

    crange r1 = crange_make(80, 90);
    c_foreach (i, crange, r1)
        printf(" %lld", *i.ref);
    puts("");

    // use a temporary crange object.
    int a = 100, b = INT32_MAX;
    c_forfilter (i, crange, crange_literal(a, b, 8)
                  , i.index > 10
                  , c_flt_take(i, 3))
        printf(" %lld", *i.ref);
    puts("");
}
*/
#ifndef STC_VIEWS_H_INCLUDED
#define STC_VIEWS_H_INCLUDED

#include <stc/ccommon.h>

#ifndef c_FLT_STACK
  #define c_FLT_STACK 14 /* 22, 30, .. */
#endif
#define c_flt_take(i, n) (++(i).s1[(i).s1top++] <= (n))
#define c_flt_skip(i, n) (++(i).s1[(i).s1top++] > (n))
#define c_flt_skipwhile(i, pred) ((i).s2[(i).s2top++] |= !(pred))
#define c_flt_takewhile(i, pred) !c_flt_skipwhile(i, pred)

#define c_forfilter(...) c_MACRO_OVERLOAD(c_forfilter, __VA_ARGS__)
#define c_forfilter4(i, C, cnt, filter) \
    c_forfilter_s(i, C, C##_begin(&cnt), filter)
#define c_forfilter5(i, C, cnt, filter, cond) \
    c_forfilter_s(i, C, C##_begin(&cnt), filter) if (!(cond)) break; else
#define c_forfilter_s(i, C, start, filter) \
    for (struct {C##_iter it; C##_value *ref; \
                 uint32_t s1[c_FLT_STACK], index, count; \
                 bool s2[c_FLT_STACK]; uint8_t s1top, s2top;} \
         i = {.it=start, .ref=i.it.ref}; i.it.ref \
         ; C##_next(&i.it), i.ref = i.it.ref, ++i.index, i.s1top=0, i.s2top=0) \
      if (!((filter) && ++i.count)) ; else


// clview type

#define clview_literal(C, ...) \
    ((C){.data = (C##_value[])__VA_ARGS__, \
         .size = sizeof((C##_value[])__VA_ARGS__)/sizeof(C##_value)})

#define using_clview(Self, T) \
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


// crange type

#define crange_literal(...) \
    (*(crange[]){crange_make(__VA_ARGS__)})

typedef long long crange_value;
typedef struct { crange_value start, end, step, value; } crange;
typedef struct { crange_value *ref, end, step; } crange_iter;

#define crange_make(...) c_MACRO_OVERLOAD(crange_make, __VA_ARGS__)
#define crange_make1(stop) crange_make3(0, stop, 1)
#define crange_make2(start, stop) crange_make3(start, stop, 1)

STC_INLINE crange crange_make3(crange_value start, crange_value stop, crange_value step)
    { crange r = {start, stop - (step > 0), step}; return r; }

STC_INLINE crange_iter crange_begin(crange* self)
    { self->value = self->start; crange_iter it = {&self->value, self->end, self->step}; return it; }

STC_INLINE crange_iter crange_end(crange* self) 
    { crange_iter it = {NULL}; return it; }

STC_INLINE void crange_next(crange_iter* it) 
    { *it->ref += it->step; if ((it->step > 0) == (*it->ref > it->end)) it->ref = NULL; }

#endif
