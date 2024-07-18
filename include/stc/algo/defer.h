/* MIT License
 *
 * Copyright (c) 2024 Tyge Løvset
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
// c_return x: Call to return from current function inside an outermost
//   c_scope level. This calls all the defers added in opposite order of
//   definition before it returns x.
//
//   NB! c_return will only work correctly at the outermost c_scope level.
//   For nested c_scope levels, use 'continue;' to exit current c_scope. The
//   added defers will be called before it breaks out of the scope.

#ifndef DEFER_MAX_STATEMENTS
  #define DEFER_MAX_STATEMENTS 32
#endif

#define c_scope \
    for (struct { int top, jmp[DEFER_MAX_STATEMENTS + 1]; } _defer = {.top=1, .jmp={-1}}; _defer.top >= 0; ) \
        _resume: switch (_defer.jmp[_defer.top--]) case 0:

#define c_defer(...) do { \
    _defer.jmp[++_defer.top] = __LINE__; \
    if (0) { \
        case __LINE__: do __VA_ARGS__ while (0); \
        goto _resume; \
    } \
} while (0)

#define c_return \
    if (_defer.top) { \
        _defer.jmp[0] = __LINE__ + 1000000; \
        goto _resume; \
    } else case __LINE__ + 1000000: return

#endif // STC_DEFER_H_INCLUDED
