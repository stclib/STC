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
#include "../ccommon.h"

enum {
    cco_state_final = -1,
    cco_state_done = -2,
};

#define cco_initial(co) ((co)->cco_state == 0)
#define cco_done(co) ((co)->cco_state == cco_state_done)

/* Emulate switch in coro: always use { } after cco_case(val) and cco_default. */
#define cco_switch(x) for (long long _sw = (long long)(x), _b=0; !_b; _b=1)
#define cco_case(val) if (_b |= _sw == (val))
#define cco_default

#define cco_routine(co) \
    for (int *_state = &(co)->cco_state; *_state != cco_state_done; *_state = cco_state_done) \
        _resume: switch (*_state) case 0: // thanks, @liigo!

#define cco_yield(ret) \
    do { \
        *_state = __LINE__; return ret; goto _resume; \
        case __LINE__:; \
    } while (0)

#define cco_await(...) c_MACRO_OVERLOAD(cco_await, __VA_ARGS__)
#define cco_await_1(promise) cco_await_2(promise, )
#define cco_await_2(promise, ret) \
    do { \
        *_state = __LINE__; \
        case __LINE__: if (!(promise)) {return ret; goto _resume;} \
    } while (0)

#define cco_closure(Ret, Closure, ...) \
    struct Closure { \
        Ret (*cco_fn)(struct Closure*); \
        int cco_state; \
        __VA_ARGS__ \
    }

typedef struct cco_base { 
    void (*cco_fn)(struct cco_base*);
    int cco_state;
} cco_base;

#define cco_base_cast(closure) \
    ((cco_base *)(closure) + 0*sizeof((cco_resume(closure), (int*)0 == &(closure)->cco_state)))

#define cco_resume(closure) (closure)->cco_fn(closure)
#define cco_await_on(...) c_MACRO_OVERLOAD(cco_await_on, __VA_ARGS__)
#define cco_await_on_1(closure) cco_await_2((cco_resume(closure), cco_done(closure)), )
#define cco_await_on_2(co, func) cco_await_2((func(co), cco_done(co)), )

#define cco_block_on(...) c_MACRO_OVERLOAD(cco_block_on, __VA_ARGS__)
#define cco_block_on_1(closure) while (cco_resume(closure), !cco_done(closure))
#define cco_block_on_2(co, func) while (func(co), !cco_done(co))

#define cco_final \
    *_state = cco_state_final; case cco_state_final

#define cco_return \
    do { \
        *_state = *_state >= 0 ? cco_state_final : cco_state_done; \
        goto _resume; \
    } while (0)

#define cco_return_v(value) \
    do { \
        *_state = *_state >= 0 ? cco_state_final : cco_state_done; \
        return value; \
    } while (0)

#define cco_stop(co) \
    do { \
        int* _s = &(co)->cco_state; \
        if (*_s > 0) *_s = cco_state_final; \
        else if (*_s == 0) *_s = cco_state_done; \
    } while (0)

#define cco_reset(co) \
    (void)((co)->cco_state = 0)

/*
 * Semaphore
 */

typedef struct { intptr_t count; } cco_sem;

#define cco_sem_await(...) c_MACRO_OVERLOAD(cco_sem_await, __VA_ARGS__)
#define cco_sem_await_1(sem) cco_sem_await_2(sem, )
#define cco_sem_await_2(sem, ret) \
    do { \
        cco_await_2((sem)->count > 0, ret); \
        --(sem)->count; \
    } while (0)

#define cco_sem_release(sem) ++(sem)->count
#define cco_sem_from(value) ((cco_sem){value})
#define cco_sem_set(sem, value) ((sem)->count = value)

/*
 * Timer
 */

#ifdef _WIN32
    #ifdef __cplusplus
    #define _c_LINKC extern "C" __declspec(dllimport) 
    #else
    #define _c_LINKC __declspec(dllimport) 
    #endif
    struct _FILETIME;
    _c_LINKC void GetSystemTimePreciseAsFileTime(struct _FILETIME*);
    _c_LINKC void Sleep(unsigned long);

    static inline double cco_time(void) { /* seconds since epoch */
        unsigned long long quad;          /* 64-bit value representing 1/10th usecs since Jan 1 1601, 00:00 UTC */
        GetSystemTimePreciseAsFileTime((struct _FILETIME*)&quad);
        return (double)(quad - 116444736000000000ULL)*1e-7;  /* time diff Jan 1 1601-Jan 1 1970 in 1/10th usecs */
    }

    static inline void cco_sleep(double sec) {
        Sleep((unsigned long)(sec*1000.0));
    }
#else
    #include <sys/time.h>
    static inline double cco_time(void) { /* seconds since epoch */
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return (double)tv.tv_sec + (double)tv.tv_usec*1e-6;
    }

    static inline void cco_sleep(double sec) {
        struct timeval tv;
        tv.tv_sec = (time_t)sec;
        tv.tv_usec = (suseconds_t)((sec - (double)(long)sec)*1e6);
        select(0, NULL, NULL, NULL, &tv);
    }
#endif

typedef struct { double interval, start; } cco_timer;

#define cco_timer_await(...) c_MACRO_OVERLOAD(cco_timer_await, __VA_ARGS__)
#define cco_timer_await_2(tm, sec) cco_timer_await_3(tm, sec, )
#define cco_timer_await_3(tm, sec, ret) \
    do { \
        cco_timer_start(tm, sec); \
        cco_await_2(cco_timer_expired(tm), ret); \
    } while (0)

static inline void cco_timer_start(cco_timer* tm, double sec) {
    tm->interval = sec;
    tm->start = cco_time();
}

static inline cco_timer cco_timer_from(double sec) {
    cco_timer tm = {.interval=sec, .start=cco_time()};
    return tm;
}

static inline void cco_timer_restart(cco_timer* tm) {
    tm->start = cco_time();
}

static inline bool cco_timer_expired(cco_timer* tm) {
    return cco_time() - tm->start >= tm->interval;
}

static inline double cco_timer_elapsed(cco_timer* tm) {
    return cco_time() - tm->start;
}

static inline double cco_timer_remaining(cco_timer* tm) {
    return tm->start + tm->interval - cco_time();
}

#endif
