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
#include "stc/algo/filter.h"
#include "stc/algo/crange.h"

int main(void)
{
    crange r1 = crange_make(80, 90);
    c_foreach (i, crange, r1)
        printf(" %d", (int)*i.ref);
    puts("");

    // use a temporary crange object.
    int a = 100, b = INT32_MAX;
    crange r2 = crange_make(a, b, 8);
    c_filter(crange, r2
         , c_flt_skip(10)
        && (printf(" %zi", *value), c_flt_take(3))
    );
    puts("");
}
*/
#ifndef STC_CRANGE_H_INCLUDED
#define STC_CRANGE_H_INCLUDED

#include "../priv/linkage.h"
#include "../common.h"

typedef intptr_t crange_value;
typedef struct { crange_value start, end, step, value; } crange;
typedef struct { crange_value *ref, end, step; } crange_iter;

#define crange_init crange_make // [deprecated]
#define crange_make(...) c_MACRO_OVERLOAD(crange_make, __VA_ARGS__)
#define crange_make_1(stop) crange_make_3(0, stop, 1)
#define crange_make_2(start, stop) crange_make_3(start, stop, 1)

STC_INLINE crange crange_make_3(crange_value start, crange_value stop, crange_value step)
    { crange r = {start, stop - (step > 0), step}; return r; }

STC_INLINE crange_iter crange_begin(crange* self)
    { self->value = self->start; crange_iter it = {&self->value, self->end, self->step}; return it; }

STC_INLINE crange_iter crange_end(crange* self)
    { (void)self; crange_iter it = {0}; return it; }

STC_INLINE void crange_next(crange_iter* it)
    { *it->ref += it->step; if ((it->step > 0) == (*it->ref > it->end)) it->ref = NULL; }

#include "../priv/linkage2.h"
#endif // STC_CRANGE_H_INCLUDE
