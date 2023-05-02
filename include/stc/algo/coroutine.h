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

        cco_final:
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

enum cco_states {
    cco_state_final = -1,
    cco_state_done = -2,
};

#define cco_suspended(ctx) ((ctx)->cco_state > 0)
#define cco_alive(ctx) ((ctx)->cco_state != cco_state_done)
#define cco_done(ctx) ((ctx)->cco_state == cco_state_done)

#define cco_begin(ctx) \
    int *_state = &(ctx)->cco_state; \
    switch (*_state) { \
        case 0:

#define cco_end(retval) \
        *_state = cco_state_done; break; \
        case -99: goto _cco_final_; \
    } \
    return retval

#define cco_yield(retval) \
    do { \
        *_state = __LINE__; return retval; \
        case __LINE__:; \
    } while (0)

#define cco_await_while(cond, retval) \
    do { \
        *_state = __LINE__; \
        case __LINE__: if (cond) return retval; \
    } while (0)

#define cco_await(promise) cco_await_while(!(promise), false)

#define cco_final \
    case cco_state_final: \
    _cco_final_

#define cco_return \
    goto _cco_final_

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
    int count;
} cco_semaphore;

/**
 * Wait for a semaphore
 *
 * This macro carries out the "wait" operation on the semaphore. The
 * wait operation causes the "thread" to block while the counter is
 * zero. When the counter reaches a value larger than zero, the
 * protothread will continue.
 */
#define cco_await_sem(sem) \
  do { \
    cco_await((sem)->count > 0); \
    --(sem)->count; \
  } while (0)

/**
 * Signal a semaphore
 *
 * This macro carries out the "signal" operation on the semaphore. The
 * signal operation increments the counter inside the semaphore, which
 * eventually will cause waiting "threads" to continue executing.
 */
#define cco_signal_sem(sem) ++(sem)->count
#define cco_reset_sem(sem, value) ((sem)->count = value)

#endif
