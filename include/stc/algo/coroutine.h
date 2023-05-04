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
#ifndef STC_COROUTINE_INCLUDED
#define STC_COROUTINE_INCLUDED
/*
#include <stdio.h>
#include <stc/algo/coroutine.h>

struct coroutine {
    int max_x, max_y;
    int x, y;
    int cco_state; // required member
};

bool coroutine(struct coroutine* I) {
    cco_begin(I);
        for (I->x = 0; I->x < I->max_x; I->x++)
            for (I->y = 0; I->y < I->max_y; I->y++)
                cco_yield(false);

    cco_final: // required if there is cleanup code
        puts("final");
    cco_end(true);
}

int main(void) {
    struct coroutine it = {.max_x=3, .max_y=3};
    int n = 0;
    while (!coroutine(&it))
    {
        printf("%d %d\n", it.x, it.y);
        // example of early stop:
        if (++n == 7) cco_stop(&it); // signal to stop at next
    }
    return 0;
}
*/
#include <stc/ccommon.h>

enum {
    cco_state_final = -1,
    cco_state_done = -2,
};

#define cco_initial(ctx) ((ctx)->cco_state == 0)
#define cco_suspended(ctx) ((ctx)->cco_state > 0)
#define cco_done(ctx) ((ctx)->cco_state == cco_state_done)

#define cco_begin(ctx) \
    int *_state = &(ctx)->cco_state; \
    goto _begin; _begin: switch (*_state) { \
        case 0:

#define cco_end(retval) \
    } \
    *_state = cco_state_done; \
    return retval

#define cco_yield(retval) \
    do { \
        *_state = __LINE__; return retval; \
        case __LINE__:; \
    } while (0)

#define cco_await(...) c_MACRO_OVERLOAD(cco_await, __VA_ARGS__)
#define cco_await_1(promise) cco_await_2(promise, )
#define cco_await_2(promise, retval) \
    do { \
        *_state = __LINE__; \
        case __LINE__: if (!(promise)) return retval; \
    } while (0)

#define cco_final \
    case cco_state_final

#define cco_return \
    do { *_state = cco_state_final; goto _begin; } while (0)

#define cco_stop(ctx) \
    do { \
        int* _state = &(ctx)->cco_state; \
        if (*_state > 0) *_state = cco_state_final; \
    } while (0)

#define cco_reset(ctx) \
    do { \
        int* _state = &(ctx)->cco_state; \
        if (*_state == cco_state_done) *_state = 0; \
    } while (0)


typedef struct {
    intptr_t count;
} cco_semaphore;

/**
 * Wait for a semaphore
 *
 * This macro carries out the "wait" operation on the semaphore,
 * and causes the "thread" to block while the counter is zero.
 */
#define cco_await_sem(...) c_MACRO_OVERLOAD(cco_await_sem, __VA_ARGS__)
#define cco_await_sem_1(sem) cco_await_sem_2(sem, )
#define cco_await_sem_2(sem, retval) \
    do { \
        cco_await_2((sem)->count > 0, retval); \
        --(sem)->count; \
    } while (0)

/**
 * Signal a semaphore
 *
 * This macro carries out the "signal" operation on the semaphore,
 * and increments the counter inside the semaphore, which
 * eventually will cause waiting "threads" to continue executing.
 */
#define cco_signal_sem(sem) ++(sem)->count
#define cco_reset_sem(sem, value) ((sem)->count = (value))

#endif
