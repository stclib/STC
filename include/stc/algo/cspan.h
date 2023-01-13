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

using_cspan(Span3f, float, 3);

int demo1() {
    float raw[3*4*5];
    Span3f span = cspan_make(raw, 3, 4, 5);
    *cspan_at(&span, 2, 3, 4) = 100;
    
    printf("%f\n", *cspan_at(&span, 2, 3, 4));
}

using_cspan(Intspan, int, 1);

int demo2() {
    int array[] = {1, 2, 3, 4, 5};
    Intspan span = cspan_from(array);
    
    c_FOREACH (i, Intspan, span)
        printf(" %d", *i.ref);
    puts("");
    
    // use a temporary IntSpan object.
    c_FORFILTER (i, Intspan, cspan_object(Intspan, {10, 20, 30, 23, 22, 21})
                  , c_FLT_SKIPWHILE(i, *i.ref < 25)
                 && (*i.ref & 1) == 0 // even only
                  , c_FLT_TAKE(i, 2)) // break after 2
        printf(" %d", *i.ref);
    puts("");
}
*/
#ifndef STC_CSPAN_H_INCLUDED
#define STC_CSPAN_H_INCLUDED

#include "../ccommon.h"

#define using_cspan(Self, T, RANK) \
    typedef T Self##_value, Self##_raw; \
    typedef struct { Self##_value *data; uint32_t dim[RANK]; } Self; \
    typedef struct { Self##_value *ref, *end; } Self##_iter; \
    \
    STC_INLINE Self##_iter Self##_begin(const Self* self) { \
        Self##_iter it = {self->data, self->data + cspan_size(self)}; \
        return it; \
    } \
    STC_INLINE Self##_iter Self##_end(const Self* self) { \
        Self##_iter it = {NULL, self->data + cspan_size(self)}; \
        return it; \
    } \
    STC_INLINE void Self##_next(Self##_iter* it) \
        { if (++it->ref == it->end) it->ref = NULL; } \
    struct stc_nostruct

#define cspan_assert(self, rank) c_STATIC_ASSERT(cspan_rank(self) == rank)

#define cspan_object(S, ...) \
    ((S){.data = (S##_value[])__VA_ARGS__, \
         .dim = {sizeof((S##_value[])__VA_ARGS__)/sizeof(S##_value)}})

#define cspan_make(data, ...) {data, {__VA_ARGS__}}
#define cspan_from(data) \
    {data + c_STATIC_ASSERT(sizeof(data) != sizeof(void*)), {c_ARRAYLEN(data)}}

#define cspan_size(self) _cspan_size((self)->dim, cspan_rank(self))
#define cspan_rank(self) c_ARRAYLEN((self)->dim)

#define cspan_reshape(self, ...) \
    memcpy((self)->dim, (uint32_t[]){__VA_ARGS__}, \
            sizeof((self)->dim) + cspan_assert(self, c_NUMARGS(__VA_ARGS__)))

#define cspan_at(self, ...) \
    ((self)->data + c_PASTE(_cspan_i, c_NUMARGS(__VA_ARGS__))((self)->dim, __VA_ARGS__) \
                  + cspan_assert(self, c_NUMARGS(__VA_ARGS__)))

#define cspan_4to3(self, x) \
    {cspan_at(self, x, 0, 0, 0), {(self)->dim[1], (self)->dim[2], (self)->dim[3]}}
#define cspan_4to2(self, x, y) \
    {cspan_at(self, x, y, 0, 0), {(self)->dim[2], (self)->dim[3]}}
#define cspan_4to1(self, x, y, z) \
    {cspan_at(self, x, y, z, 0), {(self)->dim[3]}}
#define cspan_3to2(self, x) \
    {cspan_at(self, x, 0, 0), {(self)->dim[1], (self)->dim[2]}}
#define cspan_3to1(self, x, y) \
    {cspan_at(self, x, y, 0), {(self)->dim[2]}}
#define cspan_2to1(self, x) \
    {cspan_at(self, x, 0), {(self)->dim[1]}}

STC_INLINE uint32_t _cspan_i1(const uint32_t dim[1], uint32_t x)
    { assert(x < dim[0]); return x; }

STC_INLINE uint32_t _cspan_i2(const uint32_t dim[2], uint32_t x, uint32_t y)
    { assert(x < dim[0] && y < dim[1]); return dim[1]*x + y; }

STC_INLINE uint32_t _cspan_i3(const uint32_t dim[3], uint32_t x, uint32_t y, uint32_t z) {
    assert(x < dim[0] && y < dim[1] && z < dim[2]);
    return dim[2]*(dim[1]*x + y) + z;
}
STC_INLINE uint32_t _cspan_i4(const uint32_t dim[4], uint32_t x, uint32_t y, uint32_t z, uint32_t w) {
    assert(x < dim[0] && y < dim[1] && z < dim[3] && w < dim[3]);
    return dim[3]*(dim[2]*(dim[1]*x + y) + z) + w;
}
STC_INLINE size_t _cspan_size(const uint32_t dim[], unsigned rank) {
    size_t sz = dim[0];
    while (rank --> 1) sz *= dim[rank];
    return sz;
}

#endif
