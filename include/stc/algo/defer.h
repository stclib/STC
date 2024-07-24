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

// Fully portable two-level nested scoped DEFER for C99.

// c_scope:
// ========
// c_scope { ... }: Create a defer scope that allow up to 50 defer statements.
// c_scope_with_cap(N) { ... }: Defer scope that allow max N defer statements.
//
// c_defer({ ... }): Adds ... code that be deferred in the current c_scope.
//
// c_break: Break out of a c_scope. Unlike regular break it will also break
//     out of any nested loops/switch. Before breaking, it calls all added
//     c_defers in opposite order of definition, before it returns x.
//
// c_return x: Call to return from current function. This calls all added
//     c_defers in opposite order of definition, before it returns x.
//
//
// CAVEAT 1: Unlike in other programming languages, the return expression
//     is evaluated after deferred statements. The workaround is to assign
//     the return expression to a local variable, and then c_return that.
//
// CAVEAT 2: Code will only compile with a single c_scope per function,
//     but it is possible to use multiple sequential cx_scope's within
//     one function, or c2_scope inside a c_scope (see below).

// c2_scope:
// =========
// c2_scope, c2_defer(), c2_break, c2_break_outer, and c2_return may be used when
// you need a second defer scope inside a c_scope. c2_scope is functionally
// identical to c_scope, but the default max number of deferred statements are
// 10 versus c_scope's 50. Use c2_scope_with_cap(N) to control max number of
// statements.
//
// c2_break: Break out of a c2_scope. Unlike regular break it will also break
//     out of any nested loops/switch. Before breaking, it calls all c2_defers
//     in opposite order of definition, before it breaks out of c2_scope.
//
// c2_break_outer: Break out of a c2_scope + c_scope. It will first call all added
//     c2_defers, then all c_defers before exiting c_scope.
//
// NOTE: Placing a c2_scope inside another c2_scope (or cx_scope) is UB, but it
//     is allowed to have multiple c2_scopes in sequence inside a c_scope.

// cx_scope:
// =========
// cx_scope, cx_defer(), cx_break, and cx_return may be used when you need extra
// defer scopes within a single function. cx_scope is functionally identical
// to c_scope, but the default max number of deferred statements are 10.
// Use cx_scope_with_cap(N) to control max number of statements.
//
// NOTE: Prefer to use c_scope as the first defer scope per function as it is much
//     faster, uses far less resources than cx_scope, and it may wrap a c2_scope.
//
// NOTE: cx_scope must not enclose or be enclosed by any other defer scopes.

/*
#include <stdio.h>
#include "stc/algo/defer.h"

int test_defer(int x) {
    printf("\nx=%d:\n", x);

    c_scope_with_cap(5) {
        c_defer({ puts("c: one"); });
        if (x == -1) c_return -1;

        c2_scope {
            c2_defer({ puts("c2: defer"); });
            if (x == 1) c2_break; // break out of c2_scope
            if (x == 2) c2_return 2;
            puts("c2: done");
        }

        c_defer({ puts("c: two"); });
        if (x == 1) c_break; // break out of c_scope
        puts("c: done");
    }

    cx_scope {
        cx_defer({ puts("cx: defer"); });
        puts("cb: done");
    }

    puts("DONE");
    return 0;
}

int main() {
    printf("RET: %d\n", test_defer(0));
    printf("RET: %d\n", test_defer(1));
    printf("RET: %d\n", test_defer(2));
    printf("RET: %d\n", test_defer(-1));
}
*/

#define c_scope c_scope_with_cap(50)
#define c_scope_with_cap(N) \
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

#define c_return \
    if (_defer_top) { \
        _defer_jmp[0] = -__LINE__ - 1; \
        goto _defer_next; \
    } else /* fall through */ case -__LINE__ - 1: return

// --------------------------------------------------------------------------

#include <setjmp.h>

#define cx_scope cx_scope_with_cap(10)
#define cx_scope_with_cap(N) \
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

#define cx_return \
    if (!setjmp(_defer.jmp[0])) \
        longjmp(_defer.jmp[_defer.top], 1); \
    else return

// --------------------------------------------------------------------------

#define c2_scope cx_scope
#define c2_scope_with_cap cx_scope_with_cap
#define c2_defer cx_defer
#define c2_break cx_break

#define c2_break_outer \
    if (!setjmp(_defer.jmp[0])) \
        longjmp(_defer.jmp[_defer.top], 1); \
    else c_break

#define c2_return \
    if (!setjmp(_defer.jmp[0])) \
        longjmp(_defer.jmp[_defer.top], 1); \
    else c_return

#endif // STC_DEFER_H_INCLUDED
