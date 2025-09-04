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
enum cco_status {
    CCO_DONE = 0,
    CCO_YIELD = 1<<12,
    CCO_SUSPEND = 1<<13,
    CCO_AWAIT = 1<<14,
};
#define CCO_CANCEL (1U<<30)

typedef struct {
    int spawn_count;
    int await_count;
} cco_group; // waitgroup

#define cco_state_struct(Prefix) \
    struct Prefix##_state { \
        int32_t pos:24; \
        bool drop; \
        struct Prefix##_fiber* fb; \
        cco_group* wg; \
    }

#define cco_is_initial(co) ((co)->base.state.pos == CCO_STATE_INIT)
#define cco_is_done(co) ((co)->base.state.pos == CCO_STATE_DONE)
#define cco_is_active(co) ((co)->base.state.pos != CCO_STATE_DONE)

#if defined STC_HAS_TYPEOF && STC_HAS_TYPEOF
    #define _cco_state(co) __typeof__((co)->base.state)
    #define _cco_validate_task_struct(co) \
        c_static_assert(/* error: co->base not first member in task struct */ \
                        sizeof((co)->base) == sizeof(cco_base) || \
                        offsetof(__typeof__(*(co)), base) == 0)
#else
    #define _cco_state(co) cco_state
    #define _cco_validate_task_struct(co) (void)0
#endif

#define cco_async(co) \
    if (0) goto _resume_lbl; \
    else for (_cco_state(co)* _state = (_cco_validate_task_struct(co), (_cco_state(co)*) &(co)->base.state) \
              ; _state->pos != CCO_STATE_DONE \
              ; _state->pos = CCO_STATE_DONE, \
                (void)(sizeof((co)->base) > sizeof(cco_base) && _state->wg ? --_state->wg->spawn_count : 0)) \
        _resume_lbl: switch (_state->pos) case CCO_STATE_INIT: // thanks, @liigo!

#define cco_finalize /* label */ \
    _state->drop = true; /* FALLTHRU */ \
    case CCO_STATE_DROP

#define cco_drop cco_finalize // [deprecated]
#define cco_cleanup [fix: use cco_finalize:]
#define cco_routine [fix: use cco_async]

#define cco_stop(co) \
    do { \
        cco_state* _s = (cco_state*)&(co)->base.state; \
        if (!_s->drop) { _s->pos = CCO_STATE_DROP; _s->drop = true; } \
    } while (0)

#define cco_reset_state(co) \
    do { \
        cco_state* _s = (cco_state*)&(co)->base.state; \
        _s->pos = CCO_STATE_INIT, _s->drop = false; \
    } while (0)

#define cco_return \
    do { \
        _state->pos = (_state->drop ? CCO_STATE_DONE : CCO_STATE_DROP); \
        _state->drop = true; \
        goto _resume_lbl; \
    } while (0)

#define cco_exit() \
    do { \
        _state->pos = CCO_STATE_DONE; \
        goto _resume_lbl; \
    } while (0)

#define cco_yield_v(status) \
    do { \
        _state->pos = __LINE__; return status; \
        case __LINE__:; \
    } while (0)

#define cco_yield \
    cco_yield_v(CCO_YIELD)

#define cco_suspend \
    cco_yield_v(CCO_SUSPEND)

#define cco_await(until) \
    do { \
        _state->pos = __LINE__; /* FALLTHRU */ \
        case __LINE__: if (!(until)) return CCO_AWAIT; \
    } while (0)

/* cco_await_coroutine(): assumes coroutine returns a status value (int) */
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

/* cco_run_coroutine(): assumes coroutine returns a status value (int) */
#define cco_run_coroutine(corocall) \
    while ((1 ? (corocall) : -1) != CCO_DONE)


/*
 * Tasks and Fibers
 */
struct cco_error {
    int32_t code, line;
    const char* file;
};

#define cco_fiber_struct(Prefix, Env) \
    typedef Env Prefix##_env; \
    struct Prefix##_fiber { \
        struct cco_task* task; \
        Prefix##_env* env; \
        cco_group* wgcurr; \
        struct cco_task* parent_task; \
        struct cco_task_fiber* next; \
        struct cco_task_state recover_state; \
        struct cco_error err; \
        int awaitbits, status; \
        cco_base base; /* is a coroutine object itself */ \
    }

/* Define a Task struct */
#define cco_task_struct(...) c_MACRO_OVERLOAD(cco_task_struct, __VA_ARGS__)
#define cco_task_struct_1(Task) \
    cco_task_struct_2(Task, void)

#define cco_task_struct_2(Task, Env) \
    cco_fiber_struct(Task, Env); \
    cco_state_struct(Task); \
    _cco_task_struct(Task)

#define _cco_task_struct(Task) \
    struct Task; \
    typedef struct { \
        int (*func)(struct Task*); \
        int awaitbits; \
        struct Task##_state state; \
        struct cco_task* parent_task; \
    } Task##_base; \
    struct Task

/* Base cco_task type */
typedef cco_state_struct(cco_task) cco_state;
typedef struct { cco_state state; } cco_base;
cco_fiber_struct(cco_task, void);
_cco_task_struct(cco_task) { cco_task_base base; };
typedef struct cco_task_fiber cco_fiber;
typedef struct cco_task cco_task;

#define cco_err() (&_state->fb->err)
#define cco_status() (_state->fb->status + 0)
#define cco_fb(task) ((cco_fiber*)(task)->base.state.fb + 0)
#define cco_env(task) (task)->base.state.fb->env
#define cco_set_env(task, the_env) ((task)->base.state.fb->env = the_env)

#define cco_cast_task(...) \
    ((void)sizeof((__VA_ARGS__)->base.func(__VA_ARGS__)), (cco_task *)(__VA_ARGS__))

/* Return with error and unwind await stack; must be recovered in cco_finalize section */
#define cco_throw(error_code) \
    do { \
        cco_fiber* _fb = (cco_fiber*)_state->fb; \
        _fb->err.code = error_code; \
        _fb->err.line = __LINE__; \
        _fb->err.file = __FILE__; \
        cco_return; \
    } while (0)

#define cco_cancel_fiber(a_fiber) \
    do { \
        cco_fiber* _fb1 = a_fiber; \
        _fb1->err.code = CCO_CANCEL; \
        _fb1->err.line = __LINE__; \
        _fb1->err.file = __FILE__; \
        cco_stop(_fb1->task); \
    } while (0)

/* Cancel job/task and unwind await stack; MAY be stopped (recovered) in cco_finalize section */
/* Equals cco_throw(CCO_CANCEL) if a_task is in current fiber. */
#define cco_cancel_task(a_task) \
    do { \
        cco_task* _tsk1 = cco_cast_task(a_task); \
        cco_cancel_fiber(_tsk1->base.state.fb); \
        cco_stop(_tsk1); \
        if (_tsk1 == _state->fb->task) goto _resume_lbl; \
    } while (0)

#define cco_await_cancel_task(a_task) do { \
    cco_cancel_task(a_task); \
    cco_await_task(a_task); \
} while (0)


#define cco_cancel_group(waitgroup) \
    _cco_cancel_group((cco_fiber*)_state->fb, waitgroup)

#define cco_cancel_all() \
    for (cco_fiber *_fbi = _state->fb->next; _fbi != (cco_fiber*)_state->fb; _fbi = _fbi->next) \
        cco_cancel_fiber(_fbi) \

/* Recover the thrown error; to be used in cco_finalize section upon handling cco_err()->code */
#define cco_recover \
    do { \
        cco_fiber* _fb = (cco_fiber*)_state->fb; \
        c_assert(_fb->err.code); \
        _fb->task->base.state = _fb->recover_state; \
        _fb->err.code = 0; \
        goto _resume_lbl; \
    } while (0)

/* Asymmetric coroutine await/call */
#define cco_await_task(...) c_MACRO_OVERLOAD(cco_await_task, __VA_ARGS__)
#define cco_await_task_1(a_task) cco_await_task_2(a_task, CCO_DONE)
#define cco_await_task_2(a_task, _awaitbits) do { \
    {   cco_task* _await_task = cco_cast_task(a_task); \
        (void)sizeof(cco_env(a_task) == _state->fb->env); \
        cco_fiber* _fb = (cco_fiber*)_state->fb; \
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
        (void)sizeof(cco_env(a_task) == _state->fb->env); \
        cco_fiber* _fb = (cco_fiber*)_state->fb; \
        _to_task->base.awaitbits = _fb->task->base.awaitbits; \
        _to_task->base.parent_task = NULL; \
        _fb->task = _to_task; \
        _to_task->base.state.fb = _fb; \
    } \
    cco_suspend; \
} while (0)

#define cco_resume(a_task) \
    _cco_resume_task(cco_cast_task(a_task))

static inline int _cco_resume_task(cco_task* task)
    { return task->base.func(task); }

/*
 * cco_run_fiber()/cco_run_task(): Run fibers/tasks in parallel
 */
#define cco_new_fiber(...) c_MACRO_OVERLOAD(cco_new_fiber, __VA_ARGS__)
#define cco_new_fiber_1(task) \
    _cco_new_fiber(cco_cast_task(task), NULL, NULL)
#define cco_new_fiber_2(task, env) \
    _cco_new_fiber(cco_cast_task(task), ((void)sizeof((env) == cco_env(task)), env), NULL)

#define cco_reset_group(wg) ((wg)->spawn_count = 0)
#define cco_spawn(...) c_MACRO_OVERLOAD(cco_spawn, __VA_ARGS__)
#define cco_spawn_1(task) cco_spawn_4(task, NULL, NULL, _state->fb)
#define cco_spawn_2(task, wg) cco_spawn_4(task, wg, NULL, _state->fb)
#define cco_spawn_3(task, wg, env) cco_spawn_4(task, wg, env, _state->fb)
#define cco_spawn_4(task, wg, env, fiber) \
    _cco_spawn(cco_cast_task(task), wg, ((void)sizeof((env) == cco_env(task)), env), \
               (cco_fiber*)((void)sizeof((fiber)->parent_task), fiber))

#define cco_await_group(waitgroup) do { \
    cco_task* top = _state->fb->task; \
    while (top->base.parent_task) \
        top = top->base.parent_task; \
    _state->fb->wgcurr = waitgroup; \
    /* wait until spawn_count = 1 to not wait for itself to finish, else until it is 0 */ \
    _state->fb->wgcurr->await_count = (top->base.state.wg == _state->fb->wgcurr); \
    cco_await(_state->fb->wgcurr->spawn_count == _state->fb->wgcurr->await_count); \
} while (0)

#define cco_await_n(waitgroup, n) do { \
    _state->fb->wgcurr = waitgroup; \
    _state->fb->wgcurr->await_count = _state->fb->wgcurr->spawn_count - (n); \
    cco_await(_state->fb->wgcurr->spawn_count == _state->fb->wgcurr->await_count); \
} while (0)

#define cco_await_cancel_group(waitgroup) do { \
    cco_group* wg = waitgroup; \
    cco_cancel_group(wg); \
    cco_await_group(wg); \
} while (0)

#define cco_await_any(waitgroup) do { \
    cco_await_n(waitgroup, 1); \
    cco_cancel_group(_state->fb->wgcurr); \
    cco_await_group(_state->fb->wgcurr); \
} while (0)

#define cco_run_fiber(...) c_MACRO_OVERLOAD(cco_run_fiber, __VA_ARGS__)
#define cco_run_fiber_1(fiber_ref) \
    for (cco_fiber** _it_ref = (cco_fiber**)((void)sizeof((fiber_ref)[0]->env), fiber_ref) \
        ; (*_it_ref = cco_execute_next(*_it_ref)) != NULL; )
#define cco_run_fiber_2(it, fiber) \
    for (cco_fiber* it = (cco_fiber*)((void)sizeof((fiber)->env), fiber) \
        ; (it = cco_execute_next(it)) != NULL; )

#define cco_run_task(...) c_MACRO_OVERLOAD(cco_run_task, __VA_ARGS__)
#define cco_run_task_1(task) cco_run_fiber_2(_it_fb, cco_new_fiber_1(task))
#define cco_run_task_2(task, env) cco_run_fiber_2(_it_fb, cco_new_fiber_2(task, env))
#define cco_run_task_3(it, task, env) cco_run_fiber_2(it, cco_new_fiber_2(task, env))

#define cco_joined() \
    ((cco_fiber*)_state->fb == _state->fb->next)

extern int        cco_execute(cco_fiber* fb); // is a coroutine itself
extern cco_fiber* cco_execute_next(cco_fiber* fb);  // resume and return the next fiber

extern cco_fiber* _cco_new_fiber(cco_task* task, void* env, cco_group* wg);
extern cco_fiber* _cco_spawn(cco_task* task, cco_group* wg, void* env, cco_fiber* fb);
extern void       _cco_cancel_group(cco_fiber* fb, cco_group* waitgroup);

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
    #ifndef _WINDOWS_ // windows.h
      typedef long long LARGE_INTEGER;
      _c_LINKC int __stdcall QueryPerformanceCounter(LARGE_INTEGER*);
      //_c_LINKC int __stdcall QueryPerformanceFrequency(LARGE_INTEGER*);
    #endif
    #define cco_timer_freq() 10000000LL /* 1/10th microseconds */
    //static inline long long cco_timer_freq(void) {
    //    long long quad;
    //    QueryPerformanceFrequency((LARGE_INTEGER*)&quad);
    //    return quad;
    //}

    static inline long long cco_timer_ticks(void) {
        long long quad;
        QueryPerformanceCounter((LARGE_INTEGER*)&quad);
        return quad;
    }
#else
    #include <sys/time.h>
    #define cco_timer_freq() 1000000LL

    static inline long long cco_timer_ticks(void) { /* microseconds */
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_sec*cco_timer_freq() + tv.tv_usec;
    }
#endif

typedef struct { double duration; long long start_time; } cco_timer;

static inline cco_timer cco_make_timer(double sec) {
    cco_timer tm = {.duration=sec, .start_time=cco_timer_ticks()};
    return tm;
}

static inline void cco_start_timer(cco_timer* tm, double sec) {
    tm->duration = sec;
    tm->start_time = cco_timer_ticks();
}

static inline void cco_restart_timer(cco_timer* tm) {
    tm->start_time = cco_timer_ticks();
}

static inline double cco_timer_elapsed(cco_timer* tm) {
    return (double)(cco_timer_ticks() - tm->start_time)*(1.0/cco_timer_freq());
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

/* -------------------------- IMPLEMENTATION ------------------------- */
#if (defined i_implement || defined STC_IMPLEMENT) && !defined STC_COROUTINE_IMPLEMENT
#define STC_COROUTINE_IMPLEMENT
#include <stdio.h>

cco_fiber* _cco_spawn(cco_task* _task, cco_group* wg, void* env, cco_fiber* fb) {
    cco_fiber* new_fb;
    new_fb = fb->next = (fb->next ? c_new(cco_fiber, {.next=fb->next}) : fb);
    new_fb->task = _task;
    new_fb->env = (env ? env : fb->env);
    _task->base.state.fb = new_fb;
    if (wg) wg->spawn_count += 1;
    _task->base.state.wg = wg;
    return new_fb;
}

cco_fiber* _cco_new_fiber(cco_task* _task, void* env, cco_group* wg) {
    cco_fiber* new_fb = c_new(cco_fiber, {.task=_task, .env=env});
    _task->base.state.fb = new_fb;
    _task->base.state.wg = wg;
    return (new_fb->next = new_fb);
}

void _cco_cancel_group(cco_fiber* fb, cco_group* waitgroup) {
    for (cco_fiber *fbi = fb->next; fbi != fb; fbi = fbi->next) {
        cco_task* top = fbi->task;
        while (top->base.parent_task)
            top = top->base.parent_task;
        if (top->base.state.wg == waitgroup)
            cco_cancel_fiber(fbi);
    }
}

cco_fiber* cco_execute_next(cco_fiber* fb) {
    cco_fiber *_next = fb->next, *unlinked;
    int ret = cco_execute(_next);

    if (ret == CCO_DONE) {
        unlinked = _next;
        _next = (_next == fb ? NULL : _next->next);
        fb->next = _next;
        c_free_n(unlinked, 1);
    }
    return _next;
}

int cco_execute(cco_fiber* fb) {
    cco_async (fb) {
        while (1) {
            fb->parent_task = fb->task->base.parent_task;
            fb->awaitbits = fb->task->base.awaitbits;
            fb->status = cco_resume(fb->task);
            if (fb->err.code) {
                // Note: if fb->status == CCO_DONE, fb->task may already be destructed.
                if (fb->status == CCO_DONE) {
                    fb->task = fb->parent_task;
                    if (fb->task == NULL)
                        break;
                    fb->recover_state = fb->task->base.state;
                }
                cco_stop(fb->task);
                cco_suspend;
                continue;
            }
            if (!((fb->status & ~fb->awaitbits) || (fb->task = fb->parent_task) != NULL))
                break;
            cco_suspend;
        }
    }

    if ((uint32_t)fb->err.code & ~CCO_CANCEL) { // Allow CCO_CANCEL not to trigger error.
        fprintf(stderr, __FILE__ ": error: unhandled coroutine error '%d'\n"
                        "%s:%d: cco_throw(%d);\n",
                        fb->err.code, fb->err.file, fb->err.line, fb->err.code);
        exit(fb->err.code);
    }
    return CCO_DONE;
}
#endif // IMPLEMENT
#undef i_implement
#undef i_static
#undef i_header
