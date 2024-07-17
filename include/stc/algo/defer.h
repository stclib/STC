/* MIT License
 *
 * Copyright (c) 2024 Tyge Løvset
 * Based on Pieter Stevens DEFER.h, 2024
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

#ifndef STC_DEFER_H_INCLUDED
#define STC_DEFER_H_INCLUDED

// c_scope { ... }: Create a defer scope.
//
// c_defer({ ... }): Adds ... code that be deferred in the current c_scope.
//
// c_return(X): Call to return from current function inside an outermost
//   c_scope level. This calls all the defers added in opposite order of
//   definition, after which it returns X.
//
//   NB! c_return will only work correctly at the outermost c_scope level.
//   For nested c_scope levels, use 'continue;' to exit current c_scope. The
//   added defers will be called before it breaks out of the scope.

#include <setjmp.h>
#include <stdlib.h>

#define _c_UNWIND \
    (setjmp(_defer.jmp[0]) ? (void)0 : longjmp(_defer.jmp[_defer.top], 1))

#define c_scope \
    for (struct { short state, top, cap; jmp_buf* jmp; } _defer = {.cap=2, .jmp=calloc(3, sizeof(jmp_buf))}; \
                  _defer.state == 0; \
                  _defer.state = 1, _c_UNWIND, free(_defer.jmp))

#define c_defer(...) do { \
    if (_defer.top == _defer.cap) \
        _defer.jmp = realloc(_defer.jmp, ((_defer.cap *= 2) + 1)*sizeof(jmp_buf)); \
    if (setjmp(_defer.jmp[++_defer.top])) { \
        do __VA_ARGS__ while (0); \
        longjmp(_defer.jmp[--_defer.top], 1); \
    } \
} while (0)

#define c_return(x) do { \
    _c_UNWIND; \
    free(_defer.jmp); \
    return x; \
} while (0)

#endif // STC_DEFER_H_INCLUDED
