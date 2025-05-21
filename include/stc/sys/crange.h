/* MIT License
 *
 * Copyright (c) 2025 Tyge Løvset
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
#include <stc/algorithm.h>

int main(void)
{
    crange r1 = crange_make(80, 90);
    for (c_each(i, crange, r1))
        printf(" %d", *i.ref);
    puts("");

    c_filter(crange, c_iota(100, INT_MAX, 10), true
        && c_flt_skip(25)
        && c_flt_take(3)
        && printf(" %d", *value)
    );
    puts("");
}
*/
// IWYU pragma: private, include "stc/algorithm.h"
#ifndef STC_CRANGE_H_INCLUDED
#define STC_CRANGE_H_INCLUDED

#include "../priv/linkage.h"
#include "../common.h"

// crange: isize range -----

typedef isize crange_value;
typedef struct { crange_value start, end, step, value; } crange;
typedef struct { crange_value *ref, end, step; } crange_iter;

STC_INLINE crange crange_make_3(crange_value start, crange_value stop, crange_value step)
    { crange r = {start, stop - (step > 0), step}; return r; }

#define crange_make(...) c_MACRO_OVERLOAD(crange_make, __VA_ARGS__)
#define crange_make_1(stop) crange_make_3(0, stop, 1) // NB! arg is stop
#define crange_make_2(start, stop) crange_make_3(start, stop, 1)

STC_INLINE crange_iter crange_begin(crange* self) {
    self->value = self->start;
    crange_iter it = {&self->value, self->end, self->step};
    return it;
}

STC_INLINE void crange_next(crange_iter* it) {
    if ((it->step > 0) == ((*it->ref += it->step) > it->end))
        it->ref = NULL;
}

STC_INLINE crange_iter crange_advance(crange_iter it, size_t n) {
    if ((it.step > 0) == ((*it.ref += it.step*(isize)n) > it.end))
        it.ref = NULL;
    return it;
}

// iota: c++-like std::iota, use in iterations on-the-fly -----
// Note: c_iota() does not compile with c++, crange does.
#define c_iota(...) c_MACRO_OVERLOAD(c_iota, __VA_ARGS__)
#define c_iota_1(start) c_iota_3(start, INTPTR_MAX, 1) // NB! arg is start.
#define c_iota_2(start, stop) c_iota_3(start, stop, 1)
#define c_iota_3(start, stop, step) ((crange[]){crange_make_3(start, stop, step)})[0]


// crange32 -----

typedef int32_t crange32_value;
typedef struct { crange32_value start, end, step, value; } crange32;
typedef struct { crange32_value *ref, end, step; } crange32_iter;

STC_INLINE crange32 crange32_make_3(crange32_value start, crange32_value stop, crange32_value step)
    { crange32 r = {start, stop - (step > 0), step}; return r; }

#define crange32_make(...) c_MACRO_OVERLOAD(crange32_make, __VA_ARGS__)
#define crange32_make_1(stop) crange32_make_3(0, stop, 1) // NB! arg is stop
#define crange32_make_2(start, stop) crange32_make_3(start, stop, 1)

STC_INLINE crange32_iter crange32_begin(crange32* self) {
    self->value = self->start;
    crange32_iter it = {&self->value, self->end, self->step};
    return it;
}

STC_INLINE void crange32_next(crange32_iter* it) {
    if ((it->step > 0) == ((*it->ref += it->step) > it->end))
        it->ref = NULL;
}

STC_INLINE crange32_iter crange32_advance(crange32_iter it, uint32_t n) {
    if ((it.step > 0) == ((*it.ref += it.step*(int32_t)n) > it.end))
        it.ref = NULL;
    return it;
}

#include "../priv/linkage2.h"
#endif // STC_CRANGE_H_INCLUDE
