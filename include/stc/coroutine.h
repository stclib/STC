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
#ifndef STC_COROUTINE_H_INCLUDED
#define STC_COROUTINE_H_INCLUDED
/*
#include <stdio.h>
#include "stc/coroutine.h"

struct iterpair {
    cco_state cco; // required member
    int max_x, max_y;
    int x, y;
};

int iterpair(struct iterpair* I) {
    cco_routine (I) {
        for (I->x = 0; I->x < I->max_x; I->x++)
            for (I->y = 0; I->y < I->max_y; I->y++)
                cco_yield; // suspend

        cco_cleanup: // required if there is cleanup code
        puts("done");
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
#include <stdlib.h>
#include "common.h"

enum {
    CCO_STATE_INIT = 0,
    CCO_STATE_CLEANUP = -1,
    CCO_STATE_DONE = -2,
};
typedef enum {
    CCO_DONE = 0,
    CCO_YIELD = 1<<29,
    CCO_AWAIT = 1<<30,
} cco_result;

typedef struct {
    int state;
} cco_state;

#define cco_initial(co) ((co)->cco.state == CCO_STATE_INIT)
#define cco_suspended(co) ((co)->cco.state > CCO_STATE_INIT)
#define cco_done(co) ((co)->cco.state == CCO_STATE_DONE)
#define cco_active(co) ((co)->cco.state != CCO_STATE_DONE)

#if defined __GNUC__ || _MSC_VER >= 1939
  #define _cco_check_task_struct(co) \
    c_static_assert(/* error: co->cco not first member in task struct */ \
                    sizeof((co)->cco) == sizeof(cco_state) || \
                    offsetof(__typeof__(*(co)), cco) == 0)
#else
  #define _cco_check_task_struct(co) 0
#endif

#define cco_routine(co) \
    for (int *_state = &(co)->cco.state + _cco_check_task_struct(co) \
           ; *_state != CCO_STATE_DONE ; *_state = CCO_STATE_DONE) \
        _resume: switch (*_state) case CCO_STATE_INIT: // thanks, @liigo!

#define cco_cleanup \
    *_state = CCO_STATE_CLEANUP; \
    /* fall through */ \
    case CCO_STATE_CLEANUP

#define cco_scope cco_routine // [deprecated]
#define cco_final cco_cleanup // [deprecated]

#define cco_return \
    do { \
        *_state = *_state >= CCO_STATE_INIT ? CCO_STATE_CLEANUP : CCO_STATE_DONE; \
        goto _resume; \
    } while (0)

#define cco_yield cco_yield_v(CCO_YIELD)
#define cco_yield_v(suspendval) \
    do { \
        *_state = __LINE__; return suspendval; goto _resume; \
        case __LINE__:; \
    } while (0)

#define cco_yield_final cco_yield_final_v(CCO_YIELD)
#define cco_yield_final_v(suspendval) \
    do { \
        *_state = *_state >= CCO_STATE_INIT ? CCO_STATE_CLEANUP : CCO_STATE_DONE; \
        return suspendval; \
    } while (0)

#define cco_await(until) cco_await_v(until, CCO_AWAIT)
#define cco_await_v(until, suspendval) \
    do { \
        *_state = __LINE__; \
        /* fall through */ \
        case __LINE__: if (!(until)) {return suspendval; goto _resume;} \
    } while (0)

/* cco_await_coroutine(): assumes coroutine returns a cco_result value (int) */
#define cco_await_coroutine(...) c_MACRO_OVERLOAD(cco_await_coroutine, __VA_ARGS__)
#define cco_await_coroutine_1(corocall) cco_await_coroutine_2(corocall, CCO_DONE)
#define cco_await_coroutine_2(corocall, awaitbits) \
    do { \
        *_state = __LINE__; \
        /* fall through */ \
        case __LINE__: { \
            int _r = corocall; \
            if (_r & ~(awaitbits)) { return _r; goto _resume; } \
        } \
    } while (0)

/* cco_run_coroutine(): assumes coroutine returns a cco_result value (int) */
#define cco_run_coroutine(corocall) \
    while ((1 ? (corocall) : -1) != CCO_DONE)

#define cco_stop(co) \
    do { \
        int* _s = &(co)->cco.state; \
        *_s = *_s >= CCO_STATE_INIT ? CCO_STATE_CLEANUP : CCO_STATE_DONE; \
    } while (0)

#define cco_cancel(co, func) \
    do { \
        cco_stop(co); func(co); \
    } while (0)

#define cco_reset(co) \
    (void)((co)->cco.state = 0)


/*
 * Tasks: typesafe coroutine function objects
 */

struct cco_task;
typedef struct {
    int result;
    struct cco_task* curr;
} cco_runtime;

#define cco_task_struct(Task) \
    struct Task; \
    typedef struct { \
        int (*func)(struct Task*, cco_runtime*); \
        int state, await; \
        struct cco_task* prev; \
    } Task##_state; \
    struct Task

cco_task_struct(cco_task) { cco_task_state cco; }; /* Define base Task struct type */
typedef struct cco_task cco_task;

#define cco_cast_task(task) \
    ((cco_task *)(task) + 0*sizeof((task)->cco.func(task, (cco_runtime*)0)))

#define cco_resume_task(task, rt) \
    (task)->cco.func(task, rt)

#define cco_cancel_task(task, rt) \
    do { \
        cco_task* _task = cco_cast_task(task); \
        cco_stop(_task); _task->cco.func(_task, rt); \
    } while (0)

#define cco_await_task(...) c_MACRO_OVERLOAD(cco_await_task, __VA_ARGS__)
#define cco_await_task_2(task, rt) cco_await_task_3(task, rt, CCO_DONE)
#define cco_await_task_3(task, rt, awaitbits) cco_await_task_v(task, rt, awaitbits, CCO_AWAIT)
#define cco_await_task_v(task, rt, awaitbits, suspendval) \
    do {{cco_task* _prev = (rt)->curr; \
        (rt)->curr = cco_cast_task(task); \
        (rt)->curr->cco.await = (awaitbits); \
        (rt)->curr->cco.prev = _prev;} \
        cco_yield_v(suspendval); \
    } while (0)

#define cco_yield_task(task, rt) cco_yield_task_v(task, rt, CCO_AWAIT)
#define cco_yield_task_v(task, rt, suspendval) \
    do {{cco_task* _task = cco_cast_task(task); \
        _task->cco.await = (rt)->curr->cco.await; \
        _task->cco.prev = (rt)->curr->cco.prev; \
        (rt)->curr = _task;} \
        cco_yield_v(suspendval); \
    } while (0)

#define cco_run_task(...) c_MACRO_OVERLOAD(cco_run_task, __VA_ARGS__)
#define cco_run_task_1(task) cco_run_task_2(task, _rt)
#define cco_run_task_2(task, rt) \
    for (cco_runtime rt = {.curr = cco_cast_task(task)} \
         ; rt.result = cco_resume_task(rt.curr, &rt), \
            (rt.result & ~rt.curr->cco.await) || (rt.curr = rt.curr->cco.prev) != NULL \
         ; )

struct cco_taskrunner {
    cco_runtime rt;
    cco_state cco;
};

#define cco_make_taskrunner(task) \
    ((struct cco_taskrunner){.rt = {.curr = cco_cast_task(task)}})

static inline int cco_taskrunner(struct cco_taskrunner* co) {
    cco_routine (co) {
        while (1) {{
            cco_task* curr = co->rt.curr;
            co->rt.result = cco_resume_task(curr, &co->rt);
            if (!((co->rt.result & ~curr->cco.await) || (co->rt.curr = curr->cco.prev) != NULL)) {
                break;
            }}
            cco_yield;
        }
    }
    return 0;
}


/* // Iterators for coroutine generators
 *
 * typedef struct { // A generator data struct
 *     Data data; ...
 * } Gen, Gen_value;
 *
 * // Define the iterator coroutine struct
 * typedef struct {
 *     cco_state cco; // or a task state cco.
 *     Gen* ref;
 *     ...
 * } Gen_iter;
 *
 * // Define the iterator coroutine func; produce the next value:
 * int Gen_next(Gen_iter* it) {
 *     cco_routine (it) {
 *        ... it->ref->data ...
 *        cco_yield; // suspend exec, gen with value ready
 *        ...
 *        cco_cleanup:
 *        it->ref = NULL; // stops the iteration
 *     }
 * }
 *
 * // Return the coroutine iter; advance to the first yield:
 * Gen_iter Gen_begin(Gen* g) {
 *     Gen_iter it = {.ref = g};
 *     ...
 *     Gen_next(&it);
 *     return it;
 * }
 *
 * ...
 * c_foreach (i, Gen, gen)
 *     printf("%d ", *i.ref);
 */

/*
 * Iterate containers with already defined iterator (prefer to use in coroutines only):
 */
#define cco_foreach(existing_it, C, cnt) \
    for (existing_it = C##_begin(&cnt); (existing_it).ref; C##_next(&existing_it))

#define cco_foreach_reverse(existing_it, C, cnt) \
    for (existing_it = C##_rbegin(&cnt); (existing_it).ref; C##_rnext(&existing_it))

/*
 * Using c_filter with coroutine iterators:
 */
#define cco_flt_take(n) \
    (c_flt_take(n), _base.done ? _it.cco.state = CCO_STATE_CLEANUP : 1)

#define cco_flt_takewhile(pred) \
    (c_flt_takewhile(pred), _base.done ? _it.cco.state = CCO_STATE_CLEANUP : 1)


/*
 * Semaphore
 */

typedef struct { ptrdiff_t count; } cco_semaphore;

#define cco_await_semaphore(sem) cco_await_semaphore_v(sem, CCO_AWAIT)
#define cco_await_semaphore_v(sem, suspendval) \
    do { \
        cco_await_v((sem)->count > 0, suspendval); \
        --(sem)->count; \
    } while (0)

#define cco_semaphore_release(sem) ++(sem)->count
#define cco_semaphore_from(value) ((cco_semaphore){value})
#define cco_semaphore_set(sem, value) ((sem)->count = value)


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

static inline cco_timer cco_timer_make(double sec) {
    cco_timer tm = {.interval=sec, .start=cco_time()};
    return tm;
}

#define cco_await_timer(tm, sec) cco_await_timer_v(tm, sec, CCO_AWAIT)
#define cco_await_timer_v(tm, sec, suspendval) \
    do { \
        cco_timer_start(tm, sec); \
        cco_await_v(cco_timer_expired(tm), suspendval); \
    } while (0)

static inline void cco_timer_start(cco_timer* tm, double sec) {
    tm->interval = sec;
    tm->start = cco_time();
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
