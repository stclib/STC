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

// Non-portable two-level nested scoped DEFER.
//
// c_guard:
// ========
// c_guard { ... }: Create a defer scope that allows max 16 defer statements.
// c_guard_max(N) { ... }: Defer scope that allows max N defer statements.
//
// c_defer({ ... }): Add code inside a c_guard scope, which executions is
//     deferred until a c_return, c_break, or end of c_guard scope is reached.
//
// c_panic({ ... }): Executes all added deferred statements in reverse
//     followed by the code in ..., and breaks the guarded scope. It is
//     allowed to call `return` or exit() in the code block.
//
// c_break: Break out of a c_guard. Unlike regular break it will also break
//     out of any nested loop/switch. Before breaking, it calls all added
//     c_defers in opposite order of definition, before it exits the scope.
//
// c_return(x): Return from current function. It first evaluates x, then
//     calls all added c_defers in opposite order of definition, before
//     it returns the evaluated result of x.
//
// NOTE1: c_guard requires computed goto's GNUC extension.
//
// NOTE2: Regular `return`, `break` and `continue` must not be used
//     anywhere inside a defer scope (except inside a c_panic clause).
//     Use c_return and c_break to exit clause, otherwise assertion fails.
//
// NOTE3: Nested c_guard's are not supported, as c_return will only
//     execute the innermost defers before returning.

#ifndef STC_DEFER_H_INCLUDED
#define STC_DEFER_H_INCLUDED
#include <assert.h>

#if !(defined __GNUC__ || defined __TINYC__)
#error "c_defer requires computed goto's GNUC extension"
#else
#include <setjmp.h>
#define c_JOIN0(a, b) a ## b

#define c_guard c_guard_max(16)
#define c_guard_max(N) \
    for (struct { int top; void *jmp[(N) + 1]; jmp_buf brk; } _defer = {0} \
        ; !setjmp(_defer.brk) \
        ; assert(!"missing c_return / c_break"))

#define c_defer(...) _Defer(__LINE__, __VA_ARGS__)
#define c_break _Panic(__LINE__, {})
#define c_panic(...) _Panic(__LINE__, __VA_ARGS__)
#define c_return(x) _Return(__LINE__, __typeof__(x), x)

#define _Defer(n, ...) do { \
    _defer.jmp[++_defer.top] = &&c_JOIN0(_defer_lbl, n); \
    if (0) { \
        c_JOIN0(_defer_lbl, n): do __VA_ARGS__ while (0); \
        goto *_defer.jmp[--_defer.top]; \
    } \
} while (0)

#define _Panic(n, ...) do { \
    _defer.jmp[0] = &&c_JOIN0(_defer_brk, n); \
    goto *_defer.jmp[_defer.top]; \
    c_JOIN0(_defer_brk, n): do __VA_ARGS__ while (0); \
    longjmp(_defer.brk, 1); \
} while (0)

#define _Return(n, T, x) do { \
    volatile T _ret = x; \
    _defer.jmp[0] = &&c_JOIN0(_defer_ret, n); \
    goto *_defer.jmp[_defer.top]; \
    c_JOIN0(_defer_ret, n): return _ret; \
} while (0)

#endif
#endif // STC_DEFER_H_INCLUDED
