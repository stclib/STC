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
#ifndef STC_COROUTINE_H_INCLUDED
#define STC_COROUTINE_H_INCLUDED
/*
#include <stdio.h>
#include "stc/coroutine.h"

struct iterpair {
    int max_x, max_y;
    int x, y;
    int cco_state; // required member
};

int iterpair(struct iterpair* I) {
    cco_routine(I) {
        for (I->x = 0; I->x < I->max_x; I->x++)
            for (I->y = 0; I->y < I->max_y; I->y++)
                cco_yield();

        cco_final: // required if there is cleanup code
        puts("final");
    }
    return 0; // CCO_DONE
}

int main(void) {
    struct iterpair it = {.max_x=3, .max_y=3};
    int n = 0;
    while (iterpair(&it))
    {
        printf("%d %d\n", it.x, it.y);
        // example of early stop:
        if (++n == 7) cco_stop(&it); // signal to stop/finalize in next
    }
    return 0;
}
*/
#include "common.h"

enum {
    CCO_STATE_FINAL = -1,
    CCO_STATE_DONE = -2,
};
typedef enum {
    CCO_DONE = 0,
    CCO_AWAIT = 1<<0,
    CCO_YIELD = 1<<1,
} cco_result;

#define cco_initial(co) ((co)->cco_state == 0)
#define cco_suspended(co) ((co)->cco_state > 0)
#define cco_done(co) ((co)->cco_state == CCO_STATE_DONE)

// Use with c_filter:
#define cco_take(n) (c_flt_take(n), _fl.done ? _it.cco_state = CCO_STATE_FINAL : 1)
#define cco_takewhile(pred) (c_flt_takewhile(pred), _fl.done ? _it.cco_state = CCO_STATE_FINAL : 1)

#define cco_routine(co) \
    for (int* _state = &(co)->cco_state; *_state != CCO_STATE_DONE; *_state = CCO_STATE_DONE) \
        _resume: switch (*_state) case 0: // thanks, @liigo!

#define cco_yield() cco_yield_v(CCO_YIELD)
#define cco_yield_v(ret) \
    do { \
        *_state = __LINE__; return ret; goto _resume; \
        case __LINE__:; \
    } while (0)

#define cco_await(promise) cco_await_and_return(promise, CCO_AWAIT)
#define cco_await_v(promise) cco_await_and_return(promise, )
#define cco_await_and_return(promise, ret) \
    do { \
        *_state = __LINE__; \
        /* fall through */ \
        case __LINE__: if (!(promise)) {return ret; goto _resume;} \
    } while (0)

/* cco_await_call(): assumes coroutine returns a cco_result value (int) */
#define cco_await_call(...) c_MACRO_OVERLOAD(cco_await_call, __VA_ARGS__)
#define cco_await_call_1(corocall) cco_await_call_2(corocall, CCO_DONE)
#define cco_await_call_2(corocall, awaitbits) \
    do { \
        *_state = __LINE__; \
        /* fall through */ \
        case __LINE__: { int _r = corocall; if (_r & ~(awaitbits)) {return _r; goto _resume;} } \
    } while (0)

/* cco_blocking_call(): assumes coroutine returns a cco_result value (int) */
#define cco_blocking_call(corocall) while ((corocall) != CCO_DONE)

#define cco_cleanup cco_final // [deprecated]
#define cco_final \
    *_state = CCO_STATE_FINAL; \
    /* fall through */ \
    case CCO_STATE_FINAL

#define cco_return \
    do { \
        *_state = *_state >= 0 ? CCO_STATE_FINAL : CCO_STATE_DONE; \
        goto _resume; \
    } while (0)

#define cco_cancel \
    do { *_state = CCO_STATE_DONE; goto _resume; } while (0)

#define cco_stop(co) \
    ((co)->cco_state = (co)->cco_state >= 0 ? CCO_STATE_FINAL : CCO_STATE_DONE)

#define cco_reset(co) \
    (void)((co)->cco_state = 0)

#define cco_yield_final() cco_yield_final_v(CCO_YIELD)
#define cco_yield_final_v(value) \
    do { \
        *_state = *_state >= 0 ? CCO_STATE_FINAL : CCO_STATE_DONE; \
        return value; \
    } while (0)

/*
 * Iterator (for generators)
 * User define: Gen must be an existing typedef struct:
 *     Gen_iter Gen_begin(Gen* g);      // function
 *     int      Gen_next(Gen_iter* it); // coroutine
 */

#define cco_iter_struct(Gen, ...) \
    typedef Gen Gen##_value; \
    typedef struct Gen##_iter { \
        Gen##_value* ref; \
        int cco_state; \
        __VA_ARGS__ \
    } Gen##_iter

/*
 * Tasks
 */

struct cco_runtime;

#define cco_task_struct(Task, ...) \
    struct Task { \
        int (*cco_func)(struct Task*, struct cco_runtime*); \
        int cco_state, cco_expect; \
        __VA_ARGS__ \
    }

typedef cco_task_struct(cco_task, /**/) cco_task;

typedef struct cco_runtime {
    int result, top; cco_task* stack[];
} cco_runtime;

#define cco_cast_task(task) \
    ((cco_task *)(task) + 0*sizeof((task)->cco_func(task, (cco_runtime*)0) + ((int*)0 == &(task)->cco_state)))

#define cco_resume_task(task, rt) \
    (task)->cco_func(task, rt)

#define cco_await_task(...) c_MACRO_OVERLOAD(cco_await_task, __VA_ARGS__)
#define cco_await_task_2(task, rt) cco_await_task_3(task, rt, CCO_DONE)
#define cco_await_task_3(task, rt, awaitbits) \
    do { \
        ((rt)->stack[++(rt)->top] = cco_cast_task(task))->cco_expect = (awaitbits); \
        cco_yield_v(CCO_AWAIT); \
    } while (0)

#define cco_blocking_task(...) c_MACRO_OVERLOAD(cco_blocking_task, __VA_ARGS__)
#define cco_blocking_task_1(task) cco_blocking_task_3(task, _rt, 16)
#define cco_blocking_task_3(task, rt, STACKDEPTH) \
    for (struct { int result, top; cco_task* stack[STACKDEPTH]; } rt = {.stack={cco_cast_task(task)}}; \
         (((rt.result = cco_resume_task(rt.stack[rt.top], (cco_runtime*)&rt)) & \
           ~rt.stack[rt.top]->cco_expect) || --rt.top >= 0); )

/*
 * Semaphore
 */

typedef struct { intptr_t count; } cco_sem;

#define cco_await_sem(sem) cco_await_sem_and_return(sem, CCO_AWAIT)
#define cco_await_sem_v(sem) cco_await_sem_and_return(sem, )
#define cco_await_sem_and_return(sem, ret) \
    do { \
        cco_await_and_return((sem)->count > 0, ret); \
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
    #if 1 // _WIN32_WINNT < _WIN32_WINNT_WIN8 || defined __TINYC__
      #define _c_getsystime GetSystemTimeAsFileTime
    #else
      #define _c_getsystime GetSystemTimePreciseAsFileTime
    #endif
    struct _FILETIME;
    _c_LINKC void _c_getsystime(struct _FILETIME*);
    _c_LINKC void Sleep(unsigned long);

    static inline double cco_time(void) { /* seconds since epoch */
        unsigned long long quad;          /* 64-bit value representing 1/10th usecs since Jan 1 1601, 00:00 UTC */
        _c_getsystime((struct _FILETIME*)&quad);
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

#define cco_await_timer(tm, sec) cco_await_timer_v_3(tm, sec, CCO_AWAIT)
#define cco_await_timer_v(...) c_MACRO_OVERLOAD(cco_await_timer_v, __VA_ARGS__)
#define cco_await_timer_v_2(tm, sec) cco_await_timer_v_3(tm, sec, )
#define cco_await_timer_v_3(tm, sec, ret) \
    do { \
        cco_timer_start(tm, sec); \
        cco_await_and_return(cco_timer_expired(tm), ret); \
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

#endif // STC_COROUTINE_H_INCLUDED
