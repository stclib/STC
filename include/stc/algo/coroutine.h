/*
 * coroutine.h is copyright 1995, 2000 Simon Tatham.
 * Modernized/improved 2023 Tyge Løvset.
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL SIMON TATHAM BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/*
 * Coroutine mechanics, implemented on top of standard ANSI C. See
 * https://www.chiark.greenend.org.uk/~sgtatham/coroutines.html for
 * a full discussion of the theory behind this.
 * 
 * To use these macros to define a coroutine, you need to write a
 * function that looks something like this:
 * 
 * 
 * int ascending(cco_handle* ctx) {
 *    cco_context(ctx,
 *      int idx;
 *    );
 *
 *    cco_routine(c,
 *      for (c->idx = 0; c->idx < 10; c->idx += 1)
 *        cco_yield(c->idx);
 *
 *      cco_finish:; // add this to support cco_stop!
 *    );
 *
 *    return -1;
 * }
 * 
 * You declare your persistent variables with the `cco_context'
 * macro. This macro takes the cco_handle as first parameter.
 *
 * Note that the context variable is set back to zero when the
 * coroutine terminates (by cco_stop, or by control reaching
 * finish. This can make the re-entrant coroutines more useful
 * than the static ones, because you can tell when they have
 * finished.
 * 
 * This mechanism could have been better implemented using GNU C
 * and its ability to store pointers to labels, but sadly this is
 * not part of the ANSI C standard and so the mechanism is done by
 * case statements instead. That's why you can't put a cco_yield()
 * inside a switch-statement.
 * 
 * The macros will malloc() the state structure on first call, and
 * free() it when end is reached. If you want to abort in the middle,
 * the caller must call the coroutine with `cco_stop(&ctx)' as
 * parameter instead of &ctx.
 *
 *
 * Ground rules:
 *  - never put `cco_yield' within an explicit `switch'.
 *  - never put two `cco_yield' statements on the same source line.
 *  - add `cco_finish:' label at the end of the coroutine, before
 *    any cleanup code. Required to support cco_stop(ctx) argument.
 *  - in order to stop before all items are consumed, call the
 *    coroutine with `cco_stop(&handle)' as argument at the end.
 * 
 * The caller of a re-entrant coroutine must provide a context
 * variable:
 * 
 * void main(void) {
 *    cco_handle hnd = 0;
 *    for (;;) {
 *       int x = ascending(&hnd);
 *       if (x == 5) cco_stop(&hnd);
 *       if (!hnd) break;
 *       printf("got number %d\n", x);
 *
 *    }
 * }
 */
#ifndef STC_COROUTINE_INCLUDED
#define STC_COROUTINE_INCLUDED

#include <stdlib.h>
#include <assert.h>
/*
 * `c_' macros for re-entrant coroutines.
 */
typedef struct ccoHandle { 
    int cco_line;
} *cco_handle;

#define cco_context(handle, ...) \
    struct ccoContext { \
        int cco_line; \
        __VA_ARGS__ \
    } **_ccoparam = (struct ccoContext **)handle + 0*sizeof((*(handle))->cco_line)

#define cco_routine(ctx, ...) \
    if (!*_ccoparam) { \
        *_ccoparam = (struct ccoContext *)malloc(sizeof **_ccoparam); \
        (*_ccoparam)->cco_line = 0; \
    } \
    struct ccoContext *ctx = *_ccoparam; \
    switch (ctx->cco_line) { \
        case 0:; __VA_ARGS__ break; \
        default: assert(!"missing cco_finish:"); \
    } \
    free(ctx), *_ccoparam = NULL

#define cco_yield(value) \
    do { \
        (*_ccoparam)->cco_line = __LINE__; return value; \
        case __LINE__:; \
    } while (0)

#define cco_finish case -1

static inline cco_handle* cco_stop(cco_handle* handle)
    { (*handle)->cco_line = -1; return handle; }

#endif // STC_COROUTINE_INCLUDED
