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
#define i_val int
#include <stc/cstack.h>
#include <stc/algo/filter.h>

int main()
{
    c_with (cstack_int stk = c_make(cstack_int, {1, 2, 3, 4, 5, 6, 7, 8, 9}),
            cstack_int_drop(&stk))
    {
        c_foreach (i, cstack_int, stk)
            printf(" %d", *i.ref);
        puts("");
        
        c_forfilter (i, cstack_int, stk
                    , c_flt_skipwhile(i, *i.ref < 3)
                    && (*i.ref & 1) == 0 // even only
                    , c_flt_take(i, 2)) // break after 2
            printf(" %d", *i.ref);
        puts("");
    }
}
*/
#ifndef STC_FILTER_H_INCLUDED
#define STC_FILTER_H_INCLUDED

#include <stc/ccommon.h>

#ifndef c_NFILTERS
  #define c_NFILTERS 14 /* 22, 30, .. */
#endif

#define c_flt_take(i, n) (c_flt_inc(i) <= (n))
#define c_flt_skip(i, n) (c_flt_inc(i) > (n))
#define c_flt_skipwhile(i, pred) ((i).s2[(i).s2top++] |= !(pred))
#define c_flt_takewhile(i, pred) !c_flt_skipwhile(i, pred)
#define c_flt_last(i) (i).s1[(i).s1top-1]
#define c_flt_inc(i) ++(i).s1[(i).s1top++]

#define c_forfilter(...) c_MACRO_OVERLOAD(c_forfilter, __VA_ARGS__)

#define c_forfilter_4(i, C, cnt, filter) \
    c_forfilter_B(i, C, C##_begin(&cnt), filter)

#define c_forfilter_5(i, C, cnt, filter, cond) \
    c_forfilter_B(i, C, C##_begin(&cnt), filter) if (!(cond)) break; else

#define c_forfilter_B(i, C, start, filter) \
    for (struct {C##_iter it; C##_value *ref; \
                 uint32_t s1[c_NFILTERS], index; \
                 bool s2[c_NFILTERS]; uint8_t s1top, s2top;} \
         i = {.it=start, .ref=i.it.ref}; i.it.ref \
         ; C##_next(&i.it), i.ref = i.it.ref, ++i.index, i.s1top=0, i.s2top=0) \
      if (!(filter)) ; else

#endif
