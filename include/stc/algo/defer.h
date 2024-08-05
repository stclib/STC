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

// Portable two-level nested scoped DEFER for C99.

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
// NOTE4: Code will only compile with one c_guard per function, but it is
//     possible to have c2_guard inside a c_guard, or multiple cx_guard's
//     in sequence within a single function (see below).

// c2_guard:
// =========
// Use c2_guard, c2_defer, c2_break, and c2_return to have
// defer scopes inside a c_guard. c2_guard is functionally identical to
// c_guard, but the default max number of deferred statements is 8.
// Use c2_guard_max(N) to control max number of statements.
//
// c2_break: Break out of a c2_guard. Unlike regular break it will also break
//     out of any nested loop/switch. Before breaking, it calls all c2_defer
//     in opposite order of definition, before it exit the c2_guard scope.
//
// c2_panic({ ... }): Like c_panic, but executes both c2_defers + c_defers,
//     then the code in ..., and finally breaks out of the c_guard scope.
//
// c2_return(x): Like c_return, but executes both c2_defers + c_defers first.
//
// NOTE4: Placing a c2_guard inside another c2_guard (or cx_guard) is UB, but it
//     is allowed to have multiple c2_guard in sequence inside a c_guard.
//     Use only c2_return, and c2_break inside a c2_guard.

// cx_guard:
// =========
// Use cx_guard, cx_defer, cx_break, and cx_return to have additional defer
// scopes within a single function. cx_guard is functionally identical to
// c_guard, but the default max number of deferred statements is 8.
// Use cx_guard_max(N) to control max number of statements.
//
// NOTE5: cx_guard must not enclose or be enclosed by any other defer scopes.
//
// Prefer to use c_guard as the first defer scope per function as it is
// much faster and uses far less stack space than cx_guard. It can also
// enclose c2_guard if needed.

/*
// Use of all c_guard, c2_guard's and cx_guard's in one function:
#include <stdio.h>
#include "stc/algo/defer.h"

int test_defer(int x) {
    printf("\nx=%d:\n", x);

    c_guard {
        c_defer({ puts("c: defer-one"); });
        if (x == 5) c_return (5);

        c2_guard {
            c2_defer({ puts("c2-1: defer"); });
            if (x == 4) c2_return (4);
            if (x == 3) c2_break;
            if (x == 2) c2_panic({}); // break out of c_guard
            puts("c2-1: done");
        }
        c2_guard {
            c2_defer({ puts("c2-2: defer"); });
            puts("c2-2: done");
        }

        c_defer({ puts("c: defer-two"); });
        if (x == 1) c_break; // break out of c_guard
        puts("c: done");
    }

    cx_guard {
        cx_defer({ puts("cx: defer-1"); });
        if (x == 1) cx_return (1);
        puts("cx-1: done");
    }
    cx_guard {
        cx_defer({ puts("cx: defer-2"); });
        if (x == 2) cx_break;
        puts("cx-2: done");
    }
    puts("DONE");
    return 0;
}

int main() {
    for (int i = 0; i <= 5; ++i)
        printf("RET: %d\n", test_defer(i));
}
*/
// --- c_guard -------------------------------------------------------------------

#define c_guard c_guard_max(32)
#define c_guard_max(N) \
    for (int _defer_top = 1, _defer_jmp[(N) + 1] = {-1}; _defer_top >= 0; ) \
        _defer_next: switch (_defer_jmp[_defer_top--]) case 0:

#define c_defer(...) do { \
    _defer_jmp[++_defer_top] = __LINE__; \
    if (0) { \
        case __LINE__: do __VA_ARGS__ while (0); \
        goto _defer_next; \
    } \
} while (0)

#define c_break \
    goto _defer_next

#define c_return(x) c_return_typed(__typeof__(x), x)
#define c_return_typed(T, x) do { \
    T _ret = x; \
    _defer_jmp[0] = -__LINE__ - 1; \
    goto _defer_next; \
    case -__LINE__ - 1: return _ret; \
} while (0)

#define c_panic(...) do { \
    _defer_jmp[0] = __LINE__; \
    goto _defer_next; \
    case __LINE__: do __VA_ARGS__ while (0); \
    _defer_jmp[_defer_top = 0] = -1; \
    goto _defer_next; \
} while (0)

// --- cx_guard ------------------------------------------------------------------

#include <setjmp.h>

#define cx_guard cx_guard_max(8)
#define cx_guard_max(N) \
    for (struct { int top; jmp_buf jmp[(N) + 1]; } _defer = {0}; \
         !setjmp(_defer.jmp[0]); \
         longjmp(_defer.jmp[_defer.top], 1))

#define cx_defer(...) do { \
    if (setjmp(_defer.jmp[++_defer.top])) { \
        do __VA_ARGS__ while (0); \
        longjmp(_defer.jmp[--_defer.top], 1); \
    } \
} while (0)

#define cx_break \
    longjmp(_defer.jmp[_defer.top], 1)

#define cx_return(x) cx_return_typed(__typeof__(x), x)
#define cx_return_typed(T, x) do { \
    T _ret = x; \
    if (!setjmp(_defer.jmp[0])) \
        longjmp(_defer.jmp[_defer.top], 1); \
    else return _ret; \
} while (0)

// --- c2_guard ------------------------------------------------------------------

#define c2_guard cx_guard
#define c2_guard_max cx_guard_max
#define c2_defer cx_defer
#define c2_break cx_break

#define c2_return(x) c2_return_typed(__typeof__(x), x)
#define c2_return_typed(T, x) do { \
    typedef T _typ2; _typ2 _ret2 = x; \
    if (!setjmp(_defer.jmp[0])) \
        longjmp(_defer.jmp[_defer.top], 1); \
    else c_return_typed(_typ2, _ret2); \
} while (0)

#define c2_panic(...) do { \
    if (!setjmp(_defer.jmp[0])) \
        longjmp(_defer.jmp[_defer.top], 1); \
    else c_panic(__VA_ARGS__); \
} while (0)

#endif // STC_DEFER_H_INCLUDED
