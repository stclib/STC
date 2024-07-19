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

// c_scope { ... }: Create a defer scope that allow up to 32 defer statements.
// c_scope_max(N) { ... }: Create a defer scope that allow max N defer statements.
//
// c_defer({ ... }): Adds ... code that be deferred in the current c_scope.
//
// c_return x: Call to return from current function. This calls all the defers
//     added in opposite order of definition before it returns x.
//
// continue: If at the c_scope level, it exits the scope after all the defer
//     statements are executed in reverse order.
//
// Note: c_scope will only compile with a single c_scope, as nested levels
//     can not be supported.

#define c_scope c_scope_max(32)
#define c_scope_max(N) \
    for (int _defer_top = 1, _defer_jmp[(N) + 1] = {-1}; _defer_top >= 0; ) \
        _defer_next: switch (_defer_jmp[_defer_top--]) case 0:

#define c_defer(...) do { \
    _defer_jmp[++_defer_top] = __LINE__; \
    if (0) { \
        case __LINE__: do __VA_ARGS__ while (0); \
        goto _defer_next; \
    } \
} while (0)

#define c_return \
    if (_defer_top) { \
        _defer_jmp[0] = __LINE__ + 1000000; \
        goto _defer_next; \
    } else case __LINE__ + 1000000: return

#endif // STC_DEFER_H_INCLUDED
