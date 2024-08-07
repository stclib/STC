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

// Portable two-level nested scoped DEFER for C99.
//
// c_guard:
// ========
// c_guard { ... }: Create a defer scope that allow up to 32 defer statements.
// c_guard_max(N) { ... }: Defer scope that allow max N defer statements.
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
// NOTE1: c_guard can only guard variables already defined, i.e.,
//     do not define variables inside the c_guard scope which are
//     referred to by c_defer/c_panic.
//
// NOTE2: c_return(x) uses `typeof`, which is standard C23, but also
//     supported by virtually every C99 compiler except MSVC prior to
//     VS17.9. If `typeof` is not available, use c_return_typed(Type, x).
//
// NOTE3: `return` must not be used anywhere inside a defer scope (except
//     in argument to c_panic). Use `break` and `continue` only in local
//     loops/switch. Else use c_return and c_break.
//
// NOTE4: One one c_guard per function is supported.

#ifndef STC_DEFER_H_INCLUDED
#define STC_DEFER_H_INCLUDED
#include <assert.h>

#if defined __GNUC__ || defined __TINYC__
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
#define c_return_typed(T, x) _Return(__LINE__, T, x)

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
    T _ret = x; \
    _defer.jmp[0] = &&c_JOIN0(_defer_ret, n); \
    goto *_defer.jmp[_defer.top]; \
    c_JOIN0(_defer_ret, n): return _ret; \
} while (0)

#else // !__GNUC__ && !__TINYC__ (=> MSVC)

#define c_guard c_guard_max(32)
#define c_guard_max(N) \
    for (int _defer_top = 1, _brk = 0, _defer_jmp[(N) + 1] = {-1} \
        ; _defer_top >= 0 \
        ; assert(_brk && "missing c_return / c_break")) \
        _defer_next: switch (_defer_jmp[_defer_top--]) case 0:

#define c_defer(...) do { \
    _defer_jmp[++_defer_top] = __LINE__; \
    if (0) { \
        case __LINE__: do __VA_ARGS__ while (0); \
        goto _defer_next; \
    } \
} while (0)

#define c_break \
    do { _brk = 1; goto _defer_next; } while (0)

#define c_panic(...) do { \
    _defer_jmp[0] = __LINE__; \
    goto _defer_next; \
    case __LINE__: do __VA_ARGS__ while (0); \
    _defer_jmp[_defer_top = 0] = -1; \
    goto _defer_next; \
} while (0)

#define c_return(x) c_return_typed(__typeof__(x), x)
#define c_return_typed(T, x) do { \
    T _ret = x; \
    _defer_jmp[0] = -__LINE__ - 1; \
    goto _defer_next; \
    case -__LINE__ - 1: return _ret; \
} while (0)

#endif
#endif // STC_DEFER_H_INCLUDED
