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

struct iterpair {
    int max_x, max_y;
    int x, y;
    int cco_state; // required member
};

bool iterpair(struct iterpair* I) {
    cco_routine(I) {
        for (I->x = 0; I->x < I->max_x; I->x++)
            for (I->y = 0; I->y < I->max_y; I->y++)
                cco_yield(false);

        cco_final: // required if there is cleanup code
        puts("final");
    }
    return true; // finished
}

int main(void) {
    struct iterpair it = {.max_x=3, .max_y=3};
    int n = 0;
    while (!iterpair(&it))
    {
        printf("%d %d\n", it.x, it.y);
        // example of early stop:
        if (++n == 7) cco_stop(&it); // signal to stop/finalize in next
    }
    return 0;
}
*/
#include <stc/ccommon.h>

enum {
    cco_state_final = -1,
    cco_state_done = -2,
};

#define cco_initial(co) ((co)->cco_state == 0)
#define cco_suspended(co) ((co)->cco_state > 0)
#define cco_done(co) ((co)->cco_state == cco_state_done)

#define cco_routine(co) \
    for (int *_state = &(co)->cco_state, _once=1; _once; *_state = cco_state_done, _once=0) \
        _begin: switch (*_state) case 0: // thanks, @liigo!

#define cco_yield(ret) \
    do { \
        *_state = __LINE__; return ret; goto _begin; \
        case __LINE__:; \
    } while (0)

#define cco_yield_coro(...) c_MACRO_OVERLOAD(cco_yield_coro, __VA_ARGS__)
#define cco_yield_coro_2(co, call) cco_yield_coro_3(co, call, )
#define cco_yield_coro_3(co, call, ret) \
    do { call; if (!cco_done(co)) cco_yield(ret); } while (0)

#define cco_await(...) c_MACRO_OVERLOAD(cco_await, __VA_ARGS__)
#define cco_await_1(promise) cco_await_2(promise, )
#define cco_await_2(promise, ret) \
    do { \
        *_state = __LINE__; \
        case __LINE__: if (!(promise)) {return ret; goto _begin;} \
    } while (0)

#define cco_await_coro(...) c_MACRO_OVERLOAD(cco_await_coro, __VA_ARGS__)
#define cco_await_coro_2(co, call) cco_await_2((call, cco_done(co)), )
#define cco_await_coro_3(co, call, ret) cco_await_2((call, cco_done(co)), ret)

#define cco_run(co, call) while (call, !cco_done(co))

#define cco_final \
    case cco_state_final

#define cco_return \
    do { *_state = cco_state_final; goto _begin; } while (0)

#define cco_return_v(value) \
    return (*_state = cco_state_final, value) 

#define cco_stop(co) \
    do { \
        int* _state = &(co)->cco_state; \
        if (*_state > 0) *_state = cco_state_final; \
    } while (0)

#define cco_reset(co) \
    do { \
        int* _state = &(co)->cco_state; \
        if (*_state == cco_state_done) *_state = 0; \
    } while (0)

/*
 * Semaphore
 */

typedef struct {
    intptr_t count;
} csem;

#define cco_await_sem(...) c_MACRO_OVERLOAD(cco_await_sem, __VA_ARGS__)
#define cco_await_sem_1(sem) cco_await_sem_2(sem, )
#define cco_await_sem_2(sem, ret) \
    do { \
        cco_await_2((sem)->count > 0, ret); \
        --(sem)->count; \
    } while (0)

#define csem_signal(sem) ++(sem)->count
#define csem_set(sem, value) ((sem)->count = (value))

/*
 * Timer
 */

#include <time.h>
#include <sys/time.h>

static inline void csleep_us(int64_t usec) {
    struct timeval tv = {.tv_sec=(int)(usec/1000000), .tv_usec=usec % 1000000};
    select(0, NULL, NULL, NULL, &tv);
}

typedef struct {
    clock_t start;
    clock_t interval;
} ctimer;

#define cco_await_timer(...) c_MACRO_OVERLOAD(cco_await_timer, __VA_ARGS__)
#define cco_await_timer_2(tm, msecs) cco_await_timer_3(tm, msecs, )
#define cco_await_timer_3(tm, msecs, ret) \
    do { \
        ctimer_start(tm, msecs); \
        cco_await_2(ctimer_expired(tm), ret); \
    } while (0)

static inline void ctimer_start(ctimer* tm, long msecs) {
    tm->interval = msecs*(CLOCKS_PER_SEC/1000);
    tm->start = clock();
}

static inline void ctimer_restart(ctimer* tm) {
    tm->start = clock();
}

static inline bool ctimer_expired(ctimer* tm) {
    return clock() - tm->start >= tm->interval;
}

static inline long ctimer_remaining(ctimer* tm) {
    return (long)((double)(tm->start + tm->interval - clock())*(1000.0/CLOCKS_PER_SEC));
}

#endif
