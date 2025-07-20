/* MIT License
 *
 * Copyright (c) 2025 Tyge Løvset
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
#include <stc/coroutine.h>

struct iterpair {
    cco_base base; // required member
    int max_x, max_y;
    int x, y;
};

int iterpair(struct iterpair* I) {
    cco_async (I) {
        for (I->x = 0; I->x < I->max_x; I->x++)
            for (I->y = 0; I->y < I->max_y; I->y++)
                cco_yield; // suspend
    }

    puts("done");
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
    CCO_STATE_DONE = -1,
    CCO_STATE_DROP = -2,
};
typedef enum {
    CCO_DONE = 0,
    CCO_YIELD = 1<<12,
    CCO_SUSPEND = 1<<13,
    CCO_AWAIT = 1<<14,
} cco_result;
#define CCO_CANCEL (1U<<30)

typedef struct {
    int launch_count;
    int await_count;
} cco_group; // waitgroup

typedef struct {
    struct cco_state {
        int32_t pos:24;
        bool drop;
        struct cco_fiber* fb;
        cco_group* wg;
    } state;
} cco_base;

#define cco_is_initial(co) ((co)->base.state.pos == CCO_STATE_INIT)
#define cco_is_done(co) ((co)->base.state.pos == CCO_STATE_DONE)
#define cco_is_active(co) ((co)->base.state.pos != CCO_STATE_DONE)

#if defined STC_HAS_TYPEOF && STC_HAS_TYPEOF
    #define _cco_validate_task_struct(co) \
        c_static_assert(/* error: co->base not first member in task struct */ \
                        sizeof((co)->base) == sizeof(cco_base) || \
                        offsetof(__typeof__(*(co)), base) == 0)
#else
    #define _cco_validate_task_struct(co) (void)0
#endif

#define cco_async(co) \
    if (0) goto _resume; \
    else for (struct cco_state *_state = (_cco_validate_task_struct(co), &(co)->base.state) \
              ; _state->pos != CCO_STATE_DONE \
              ; _state->pos = CCO_STATE_DONE, \
                (void)(sizeof((co)->base) > sizeof(cco_base) && _state->wg ? --_state->wg->launch_count : 0)) \
        _resume: switch (_state->pos) case CCO_STATE_INIT: // thanks, @liigo!

#define cco_drop /* label */ \
    _state->drop = true; /* FALLTHRU */ \
    case CCO_STATE_DROP
#define cco_cleanup [fix: use cco_drop:]
#define cco_finally [fix: use cco_drop:]
#define cco_routine [fix: use cco_async]

#define cco_stop(co) \
    do { \
        struct cco_state* _s = &(co)->base.state; \
        if (!_s->drop) { _s->pos = CCO_STATE_DROP; _s->drop = true; } \
    } while (0)

#define cco_reset_state(co) \
    do { \
        struct cco_state* _s = &(co)->base.state; \
        _s->pos = CCO_STATE_INIT, _s->drop = false; \
    } while (0)

#define cco_return \
    do { \
        _state->pos = (_state->drop ? CCO_STATE_DONE : CCO_STATE_DROP); \
        _state->drop = true; \
        goto _resume; \
    } while (0)

#define cco_exit() \
    do { \
        _state->pos = CCO_STATE_DONE; \
        goto _resume; \
    } while (0)

#define cco_yield \
    cco_yield_v(CCO_YIELD)

#define cco_suspend \
    cco_yield_v(CCO_SUSPEND)

#define cco_yield_v(status) \
    do { \
        _state->pos = __LINE__; return status; \
        case __LINE__:; \
    } while (0)

#define cco_await(until) \
    do { \
        _state->pos = __LINE__; /* FALLTHRU */ \
        case __LINE__: if (!(until)) return CCO_AWAIT; \
    } while (0)

/* cco_await_coroutine(): assumes coroutine returns a cco_result value (int) */
#define cco_await_coroutine(...) c_MACRO_OVERLOAD(cco_await_coroutine, __VA_ARGS__)
#define cco_await_coroutine_1(corocall) cco_await_coroutine_2(corocall, CCO_DONE)
#define cco_await_coroutine_2(corocall, awaitbits) \
    do { \
        _state->pos = __LINE__; /* FALLTHRU */ \
        case __LINE__: { \
            int _res = corocall; \
            if (_res & ~(awaitbits)) return _res; \
        } \
    } while (0)

/* cco_run_coroutine(): assumes coroutine returns a cco_result value (int) */
#define cco_run_coroutine(corocall) \
    while ((1 ? (corocall) : -1) != CCO_DONE)


/*
 * Tasks and Fibers
 */
typedef struct {
    int32_t code, line;
    const char* file;
} cco_error;

typedef struct cco_fiber {
    struct cco_task* task;
    void* env;
    struct cco_task* parent_task;
    struct cco_fiber* next;
    struct cco_state recover_state;
    cco_error err;
    int awaitbits, status;
    cco_base base; /* is a coroutine object itself */
} cco_fiber;

/* Define a Task struct */
#define cco_task_struct(Task) \
    struct Task; \
    typedef struct { \
        int (*func)(struct Task*); \
        int awaitbits; \
        struct cco_state state; \
        struct cco_task* parent_task; \
    } Task##_base; \
    struct Task

/* Base cco_task type */
cco_task_struct(cco_task) { cco_task_base base; };
typedef struct cco_task cco_task;

#define cco_env(Tp) ((Tp)_state->fb->env)
#define cco_err() (*(const cco_error*)&_state->fb->err)
#define cco_status() (_state->fb->status + 0)


#define cco_cast_task(...) \
    ((void)sizeof((__VA_ARGS__)->base.func(__VA_ARGS__)), (cco_task *)(__VA_ARGS__))

/* Throw an "exception"; can be catched up in the cco_await_task call tree */
#define cco_task_throw(error_code) \
    do { \
        cco_fiber* _fb = _state->fb; \
        _fb->err.code = error_code; \
        _fb->err.line = __LINE__; \
        _fb->err.file = __FILE__; \
        cco_return; \
    } while (0)

#define cco_cancel_fiber(a_task) \
    do { \
        cco_fiber* _fb = cco_cast_task(a_task)->base.state.fb; \
        _fb->err.code = CCO_CANCEL; \
        _fb->err.line = __LINE__; \
        _fb->err.file = __FILE__; \
        cco_stop(_fb->task); \
        if (_fb->task == _state->fb->task) goto _resume; \
    } while (0)

#define cco_recover_task() \
    do { \
        cco_fiber* _fb = _state->fb; \
        c_assert(_fb->err.code); \
        _fb->task->base.state = _fb->recover_state; \
        _fb->err.code = 0; \
        goto _resume; \
    } while (0)

/* Asymmetric coroutine await/call */
#define cco_await_task(...) c_MACRO_OVERLOAD(cco_await_task, __VA_ARGS__)
#define cco_await_task_1(a_task) cco_await_task_2(a_task, CCO_DONE)
#define cco_await_task_2(a_task, _awaitbits) do { \
    {   cco_task* _await_task = cco_cast_task(a_task); \
        cco_fiber* _fb = _state->fb; \
        _await_task->base.awaitbits = (_awaitbits); \
        _await_task->base.parent_task = _fb->task; \
        _fb->task = _await_task; \
        _await_task->base.state.fb = _fb; \
    } \
    cco_suspend; \
} while (0)

/* Symmetric coroutine flow of control transfer */
#define cco_yield_to(a_task) do { \
    {   cco_task* _to_task = cco_cast_task(a_task); \
        cco_fiber* _fb = _state->fb; \
        _to_task->base.awaitbits = _fb->task->base.awaitbits; \
        _to_task->base.parent_task = NULL; \
        _fb->task = _to_task; \
        _to_task->base.state.fb = _fb; \
    } \
    cco_suspend; \
} while (0)

#define cco_resume_task(a_task) \
    _cco_resume_task(cco_cast_task(a_task))

static inline int _cco_resume_task(cco_task* task)
    { return task->base.func(task); }

/*
 * cco_run_fiber()/cco_run_task(): Run fibers/tasks in parallel
 */
#define cco_new_fiber(...) c_MACRO_OVERLOAD(cco_new_fiber, __VA_ARGS__)
#define cco_new_fiber_1(task) cco_new_fiber_2(task, NULL)
#define cco_new_fiber_2(task, env) _cco_new_fiber(cco_cast_task(task), env, NULL)

#define cco_spawn(...) c_MACRO_OVERLOAD(cco_spawn, __VA_ARGS__)
#define cco_spawn_1(task) _cco_spawn(cco_cast_task(task), NULL, _state->fb, NULL)
#define cco_spawn_2(task, env) _cco_spawn(cco_cast_task(task), env, _state->fb, NULL)
#define cco_spawn_3(task, env, fiber) _cco_spawn(cco_cast_task(task), env, fiber, NULL)

#define cco_reset_group(waitgroup) ((waitgroup)->launch_count = 0)
#define cco_launch(...) c_MACRO_OVERLOAD(cco_launch, __VA_ARGS__)
#define cco_launch_2(task, waitgroup) cco_launch_3(task, waitgroup, NULL)
#define cco_launch_3(task, waitgroup, env) do { \
    cco_group* _wg = waitgroup; _wg->launch_count += 1; \
    _cco_spawn(cco_cast_task(task), env, _state->fb, _wg); \
} while (0)

#define cco_await_all(waitgroup) cco_await((waitgroup)->launch_count == 0)
#define cco_await_any(waitgroup) cco_await_n(waitgroup, 1)
#define cco_await_n(waitgroup, n) do { \
    (waitgroup)->await_count = (waitgroup)->launch_count - (n); \
    if ((waitgroup)->await_count < 0) cco_task_throw(1); \
    cco_await((waitgroup)->launch_count == (waitgroup)->await_count); \
} while (0)

#define cco_await_cancel(waitgroup) do { \
    for (cco_fiber *_ifb = _state->fb->next; _ifb != _state->fb; _ifb = _ifb->next) { \
        cco_task* _top = _ifb->task; \
        while (_top->base.parent_task) \
            _top = _top->base.parent_task; \
        if (_top->base.state.wg == (waitgroup)) \
            cco_cancel_fiber(_top); \
    } \
    cco_await_all(waitgroup); \
} while (0)

#define cco_run_fiber(...) c_MACRO_OVERLOAD(cco_run_fiber, __VA_ARGS__)
#define cco_run_fiber_1(fiber_ref) \
    for (cco_fiber** _it_ref = fiber_ref; (*_it_ref = cco_resume_next(*_it_ref)) != NULL; )
#define cco_run_fiber_2(it, fiber) \
    for (cco_fiber* it = fiber; (it = cco_resume_next(it)) != NULL; )

#define cco_run_task(...) c_MACRO_OVERLOAD(cco_run_task, __VA_ARGS__)
#define cco_run_task_1(task) cco_run_task_2(task, NULL)
#define cco_run_task_2(task, env) cco_run_fiber_2(_it_fb, cco_new_fiber_2(task, env))
#define cco_run_task_3(it, task, env) cco_run_fiber_2(it, cco_new_fiber_2(task, env))

#define cco_joined() \
    (_state->fb == _state->fb->next)

extern cco_fiber* _cco_new_fiber(cco_task* task, void* env, cco_group* wg);
extern cco_fiber* _cco_spawn(cco_task* task, void* env, cco_fiber* fb, cco_group* wg);
extern cco_fiber* cco_resume_next(cco_fiber* prev);
extern int        cco_resume_current(cco_fiber* co); /* coroutine */

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined i_implement || defined STC_IMPLEMENT
#include <stdio.h>

int cco_resume_current(cco_fiber* fb) {
    cco_async (fb) {
        while (1) {
            fb->parent_task = fb->task->base.parent_task;
            fb->awaitbits = fb->task->base.awaitbits;
            fb->status = fb->task->base.func(fb->task); // resume
            // Note: if fb->status == CCO_DONE, fb->task may already be destructed.
            if (fb->err.code && (fb->status == CCO_DONE || !fb->task->base.state.drop)) {
                fb->task = fb->parent_task;
                if (fb->task == NULL)
                    break;
                fb->recover_state = fb->task->base.state;
                cco_stop(fb->task);
                continue;
            }
            if (!((fb->status & ~fb->awaitbits) || (fb->task = fb->parent_task) != NULL))
                break;
            cco_suspend;
        }
    }

    if ((uint32_t)fb->err.code & ~CCO_CANCEL) { // Allow CCO_CANCEL not to trigger error.
        fprintf(stderr, __FILE__ ": error: unhandled coroutine exception '%d'\n"
                        "%s:%d: unhandled: cco_task_throw(%d)\n",
                        fb->err.code, fb->err.file, fb->err.line, fb->err.code);
        exit(fb->err.code);
    }
    return CCO_DONE;
}

cco_fiber* cco_resume_next(cco_fiber* prev) {
    cco_fiber *curr = prev->next, *unlink;
    int ret = cco_resume_current(curr);

    if (ret == CCO_DONE) {
        unlink = curr;
        curr = (curr == prev ? NULL : curr->next);
        prev->next = curr;
        c_free_n(unlink, 1);
    }
    return curr;
}

cco_fiber* _cco_new_fiber(cco_task* _task, void* env, cco_group* wg) {
    cco_fiber* new_fb = c_new(cco_fiber, {.task=_task, .env=env});
    _task->base.state.fb = new_fb;
    _task->base.state.wg = wg;
    return (new_fb->next = new_fb);
}

cco_fiber* _cco_spawn(cco_task* _task, void* env, cco_fiber* fb, cco_group* wg) {
    cco_fiber* new_fb;
    new_fb = fb->next = (fb->next == NULL ? fb : c_new(cco_fiber, {.next=fb->next}));
    new_fb->task = _task;
    new_fb->env = (env == NULL ? fb->env : env);
    _task->base.state.fb = new_fb;
    _task->base.state.wg = wg;
    return new_fb;
}

#undef i_implement
#endif

/*
 * Iterate containers with already defined iterator (prefer to use in coroutines only):
 */
#define cco_each(existing_it, C, cnt) \
    existing_it = C##_begin(&cnt); (existing_it).ref; C##_next(&existing_it)

#define cco_each_reverse(existing_it, C, cnt) \
    existing_it = C##_rbegin(&cnt); (existing_it).ref; C##_rnext(&existing_it)

/*
 * Using c_filter with coroutine iterators:
 */
#define cco_flt_take(n) \
    (c_flt_take(n), fltbase.done ? (_it.base.state.pos = CCO_STATE_DROP, _it.base.state.drop = 1) : 1)

#define cco_flt_takewhile(pred) \
    (c_flt_takewhile(pred), fltbase.done ? (_it.base.state.pos = CCO_STATE_DROP, _it.base.state.drop = 1) : 1)


/*
 * Semaphore
 */

typedef struct { ptrdiff_t acq_count; } cco_semaphore;

#define cco_make_semaphore(value) (c_literal(cco_semaphore){value})
#define cco_set_semaphore(sem, value) ((sem)->acq_count = value)
#define cco_acquire_semaphore(sem) (--(sem)->acq_count)
#define cco_release_semaphore(sem) (++(sem)->acq_count)

#define cco_await_semaphore(sem) \
    do { \
        cco_await((sem)->acq_count > 0); \
        cco_acquire_semaphore(sem); \
    } while (0)


/*
 * Timer
 */

#ifdef _WIN32
    #ifdef __cplusplus
      #define _c_LINKC extern "C" __declspec(dllimport)
    #else
      #define _c_LINKC __declspec(dllimport)
    #endif
    struct _FILETIME; struct _LARGE_INTEGER;
    _c_LINKC void __stdcall GetSystemTimePreciseAsFileTime(struct _FILETIME*);
    _c_LINKC void __stdcall Sleep(unsigned long);
    _c_LINKC int __stdcall QueryPerformanceCounter(struct _LARGE_INTEGER*);
    #define cco_timer_res 1.0E-7

    static inline double cco_time(void) { /* seconds since epoch */
        unsigned long long quad;
        /* 64-bit value representing 1/10th usecs since Jan 1 1601 */
        GetSystemTimePreciseAsFileTime((struct _FILETIME*)&quad);
        /* subtract time diff to Jan 1 1970 in 1/10th usecs */
        return (double)(quad - 116444736000000000ULL)*cco_timer_res;
    }

    static inline long long cco_ticks(void) { /* 1/10th microseconds */
        long long quad;
        QueryPerformanceCounter((struct _LARGE_INTEGER*)&quad);
        return quad;
    }

    static inline void cco_sleep(double sec) {
        Sleep((unsigned long)(sec*1000.0));
    }
#else
    #include <sys/time.h>
    #define cco_timer_freq 1000000LL
    #define cco_timer_res (1.0/cco_timer_freq)

    static inline double cco_time(void) { /* seconds since epoch */
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return (double)tv.tv_sec + (double)tv.tv_usec*cco_timer_res;
    }

    static inline long long cco_ticks(void) { /* microseconds */
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_sec*cco_timer_freq + tv.tv_usec;
    }

    static inline void cco_sleep(double sec) {
        struct timeval tv;
        tv.tv_sec = (time_t)sec;
        tv.tv_usec = (suseconds_t)((sec - (double)(long)sec)*cco_timer_freq);
        select(0, NULL, NULL, NULL, &tv);
    }
#endif

typedef struct { double duration; long long start_time; } cco_timer;

static inline cco_timer cco_make_timer(double sec) {
    cco_timer tm = {.duration=sec, .start_time=cco_ticks()};
    return tm;
}

static inline void cco_start_timer(cco_timer* tm, double sec) {
    tm->duration = sec;
    tm->start_time = cco_ticks();
}

static inline void cco_restart_timer(cco_timer* tm) {
    tm->start_time = cco_ticks();
}

static inline double cco_timer_elapsed(cco_timer* tm) {
    return (double)(cco_ticks() - tm->start_time)*cco_timer_res;
}

static inline bool cco_timer_expired(cco_timer* tm) {
    return cco_timer_elapsed(tm) >= tm->duration;
}

static inline double cco_timer_remaining(cco_timer* tm) {
    return tm->duration - cco_timer_elapsed(tm);
}

#define cco_await_timer(tm, sec) \
    do { \
        cco_start_timer(tm, sec); \
        cco_await(cco_timer_expired(tm)); \
    } while (0)

#endif // STC_COROUTINE_H_INCLUDED
