/* MIT License
 *
 * Copyright (c) 2026 Tyge Løvset
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
#ifndef STC_REFLECT_H_INCLUDED
#define STC_REFLECT_H_INCLUDED

// This is the basis for reflection in C, and are used to implement tagged unions
// and serialization in stc/sys/sumtype.h and stc/c11/serialize.h

#include "../common.h"

//#define c_JOIN(a, b) c_JOIN0(a, b)
//#define c_JOIN0(a, b) a ## b
//#define c_TUPLE_AT_1(x,y,...) y
//#define c_EXPAND(...) __VA_ARGS__
//#define c_NUMARGS(...) _c_APPLY_ARG_N((__VA_ARGS__, _c_RSEQ_N))
//#define _c_APPLY_ARG_N(args) _c_ARG_N args
//#define _c_RSEQ_N 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1,
//#define _c_ARG_N(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,N,...) N

#define _c_EMPTY()
#define _c_LOOP_INDIRECTION() _c_LOOP
#define _c_LOOP_END_1 ,_c_LOOP1
#define _c_LOOP0(S,fn,x,...) fn c_EXPAND((S, c_EXPAND x)) _c_LOOP_INDIRECTION _c_EMPTY()()(S,fn,__VA_ARGS__)
#define _c_LOOP1(...)
#define _c_CHECK(x,...) c_TUPLE_AT_1(__VA_ARGS__,x,)
#define _c_E0(...) __VA_ARGS__
#define _c_E1(...) _c_E0(_c_E0(_c_E0(_c_E0(_c_E0(_c_E0(__VA_ARGS__))))))
#define _c_E2(...) _c_E1(_c_E1(_c_E1(_c_E1(_c_E1(_c_E1(__VA_ARGS__))))))
#define _c_EVAL(...) _c_E2(_c_E2(_c_E2(__VA_ARGS__))) // currently supports up to 130 pairs of (a, b),
#define _c_LOOP(S,fn,x,...) _c_CHECK(_c_LOOP0, c_JOIN(_c_LOOP_END_, c_NUMARGS(c_EXPAND x)))(S,fn,x,__VA_ARGS__)

#define c_reflect_each(S, fn, ...) \
    _c_EVAL(_c_LOOP(S, fn, __VA_ARGS__, (0),))

#define c_const_if(Cond, Then, Else) \
    _Generic(&(char[1 + !(Cond)]){0}, char(*)[1]: (Then), char(*)[2]: (Else))

#define c_is_type(x, type) \
    _Generic((x), type: 1, default: 0)

#endif // STC_REFLECT_H_INCLUDED
