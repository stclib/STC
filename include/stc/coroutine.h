/* MIT License
 *
 * Copyright (c) 2026 Tyge Løvset
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
    return 0; // cco_DONE
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
    cco_STATE_INIT = 0,
    cco_STATE_DONE = -1,
    cco_STATE_DROP = -2,
};
enum cco_status {
    cco_DONE    = 0,
    cco_YIELD   = 1<<12,
    cco_SUSPEND = 1<<13,
    cco_AWAIT   = 1<<14,
};
#define cco_CANCEL   (1U<<30)
#define cco_SHUTDOWN (1U<<29)
#define cco_NOTIFY   ((1U<<29) - 1)

enum cco_deprecated {
    CCO_DONE       = cco_DONE,       // [deprecated]
    CCO_YIELD      = cco_YIELD,      // [deprecated]
    CCO_SUSPEND    = cco_SUSPEND,    // [deprecated]
    CCO_AWAIT      = cco_AWAIT,      // [deprecated]      
};      
#define CCO_CANCEL cco_CANCEL                        // [deprecated]
#define cco_cancel_group cco_cancel_all              // [deprecated]      
#define cco_await_all_of cco_await_all               // [deprecated]
#define cco_await_cancel_group cco_await_cancel_all  // [deprecated]
#define cco_await_cancel_all_of cco_await_cancel_all // [deprecated]

/*
 * cco_group: waitgroup for tasks/fibers
 */
typedef struct {
    int32_t await_count;
    int32_t spawn_count:24;
    uint8_t on_error:8;
} cco_group;

#define cco_state_struct(Prefix) \
    struct Prefix##_state { \
        int32_t pos:24; \
        bool drop; \
        struct Prefix##_fiber* fib; \
        cco_group* parent_wg; \
    }

#define cco_is_initial(co) ((co)->base.state.pos == cco_STATE_INIT)
#define cco_is_done(co) ((co)->base.state.pos == cco_STATE_DONE)
#define cco_is_active(co) ((co)->base.state.pos != cco_STATE_DONE)

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
    for (_cco_state(co)* _cco_st = (_cco_validate_task_struct(co), (_cco_state(co)*) &(co)->base.state) \
              ; _cco_st->pos != cco_STATE_DONE \
              ; _cco_st->pos = cco_STATE_DONE, \
                (void)(sizeof((co)->base) > sizeof(cco_base) && (_cco_st->parent_wg? --_cco_st->parent_wg->spawn_count : 0))) \
        _resume_lbl: switch (_cco_st->pos) case cco_STATE_INIT: // thanks, @liigo!

#define cco_finalize /* label */ \
    if (0) { goto _resume_lbl; } \
    _cco_st->drop = true; /* FALLTHRU */ \
    case cco_STATE_DROP

#define cco_drop [fix: use cco_finalize:]
#define cco_cleanup [fix: use cco_finalize:]
#define cco_routine [fix: use cco_async]

#define cco_stop(co) \
    do { \
        cco_state* _st = (cco_state*)&(co)->base.state; \
        if (!_st->drop) { _st->pos = cco_STATE_DROP; _st->drop = true; } \
    } while (0)

#define cco_reset_state(co) \
    do { \
        cco_state* _st = (cco_state*)&(co)->base.state; \
        _st->pos = cco_STATE_INIT, _st->drop = false; \
    } while (0)

#define cco_return \
    do { \
        _cco_st->pos = (_cco_st->drop ? cco_STATE_DONE : cco_STATE_DROP); \
        _cco_st->drop = true; \
        goto _resume_lbl; \
    } while (0)

#define cco_exit() \
    do { \
        _cco_st->pos = cco_STATE_DONE; \
        goto _resume_lbl; \
    } while (0)

#define cco_yield_v(status) cco_yield_v_OFFSET(status, 0)
#define cco_yield_v_OFFSET(status, N) \
    do { \
        _cco_st->pos = __LINE__ + N; return status; /* never */ goto _resume_lbl; \
        case __LINE__ + N:; \
    } while (0)

#define cco_yield \
    cco_yield_v(cco_YIELD)

#define cco_suspend \
    cco_yield_v(cco_SUSPEND)

#define cco_await(until) cco_await_OFFSET(until, 0)
#define cco_await_OFFSET(until, N) \
    do { \
        _cco_st->pos = __LINE__ + N; /* FALLTHRU */ \
        case __LINE__ + N: if (!(until)) { return cco_AWAIT; /* never */ goto _resume_lbl; } \
    } while (0)
    
/* cco_await_coroutine(): assumes coroutine returns a status value (int) */
#define cco_await_coroutine(...) c_MACRO_OVERLOAD(cco_await_coroutine, __VA_ARGS__)
#define cco_await_coroutine_1(corocall) cco_await_coroutine_2(corocall, cco_DONE)
#define cco_await_coroutine_2(corocall, awaitbits) \
    do { \
        _cco_st->pos = __LINE__; /* FALLTHRU */ \
        case __LINE__: { \
            int _res = corocall; \
            if (_res & ~(awaitbits)) { return _res; /* never */ goto _resume_lbl; } \
        } \
    } while (0)

/* cco_run_coroutine(): assumes coroutine returns a status value (int) */
#define cco_run_coroutine(corocall) \
    while ((1 ? (corocall) : -1) != cco_DONE)


/*
 * Tasks and Fibers
 */
struct cco_error {
    int32_t code, line;
    const char* file;
    intptr_t info;
};

#define cco_fiber_struct(Prefix, EnvPtr) \
    typedef int Prefix##_env_ptr_check[sizeof((EnvPtr)0 == (void*)0)]; \
    typedef EnvPtr Prefix##_env_ptr; \
    struct Prefix##_fiber { \
        struct cco_task* task; \
        Prefix##_env_ptr env; \
        cco_group* temp_wg; \
        struct cco_task* parent_task; \
        struct cco_task_fiber *next, *parent; \
        struct cco_task_state recover_state; \
        struct cco_error error; \
        int awaitbits, status; \
        cco_base base; /* is a coroutine object itself (but not a task) */ \
    }

/* Define a Task struct */
#define cco_task_struct(...) c_MACRO_OVERLOAD(cco_task_struct, __VA_ARGS__)
#define cco_task_struct_1(Task) \
    cco_task_struct_2(Task, void*)

#define cco_task_struct_2(Task, EnvPtr) \
    cco_fiber_struct(Task, EnvPtr); \
    cco_state_struct(Task); \
    _cco_task_struct(Task)

#define _cco_task_struct(Task) \
    struct Task; \
    typedef struct { \
        int (*func)(struct Task*); \
        int awaitbits; \
        struct Task##_state state; \
        struct cco_task* parent_task; \
        cco_group task_wg; \
    } Task##_base; \
    struct Task

/* Base cco_state type */
typedef cco_state_struct(cco_task) cco_state;
typedef struct { cco_state state; } cco_base;

/* Base cco_task type */
cco_fiber_struct(cco_task, void*);
_cco_task_struct(cco_task) { cco_task_base base; };

typedef struct cco_task_fiber cco_fiber;
typedef struct cco_task cco_task;

#define _cco_taskbase() \
    c_container_of((cco_state*)_cco_st, cco_task_base, state)

#define cco_status() (_cco_st->fib->status + 0)
#define cco_error() (&_cco_st->fib->error)
#define cco_error_code() (_cco_st->fib->error.code + 0)
#define cco_clear_error() (c_assert((uint32_t)_cco_st->fib->error.code & ~cco_SHUTDOWN), _cco_st->fib->error.code = 0)
#define cco_wg() (&_cco_taskbase()->task_wg)
#define cco_parent_wg() (_cco_st->parent_wg + 0)
#define cco_parent_fiber() (_cco_taskbase()->state.fib->parent + 0)
#define cco_task_fiber() ((cco_fiber*)_cco_taskbase()->state.fib + 0)

#define cco_env(a_task) ((a_task)->base.state.fib->env)
#define cco_set_env(a_task, the_env) ((a_task)->base.state.fib->env = the_env)

enum cco_error_action { cco_SET_NOTIFY = 1, cco_SET_SHUTDOWN = 2 };
#define cco_on_child_error(mode, wg) \
    do { uint32_t _m = mode; c_assert(_m <= 2); (wg)->on_error = (uint8_t)_m; } while (0)

#define cco_cast_task(...) \
    ((void)sizeof((__VA_ARGS__)->base.func(__VA_ARGS__)), (cco_task *)(__VA_ARGS__))

#define cco_cancel_fiber(a_fiber) \
    do { \
        cco_fiber* _fb1 = a_fiber; \
        _fb1->error.code = cco_CANCEL; \
        _fb1->error.line = __LINE__; \
        _fb1->error.file = __FILE__; \
        cco_stop(_fb1->task); \
    } while (0)

/* Cancel job/task and unwind await stack; MAY be stopped (recovered) in cco_finalize section */
#define cco_cancel_task(a_task) \
    do { \
        cco_task* _tsk1 = cco_cast_task(a_task); \
        cco_cancel_fiber(_tsk1->base.state.fib); \
        cco_stop(_tsk1); \
        /*if (_tsk1 == _cco_st->fib->task) goto _resume_lbl;*/ \
    } while (0)

#define cco_cancel_all(a_group) \
    _cco_cancel_all((cco_fiber*)_cco_st->fib, a_group)


/* Return with error and unwind await stack; must be recovered in cco_finalize section */
#define cco_throw(...) c_MACRO_OVERLOAD(cco_throw, __VA_ARGS__)
#define cco_throw_1(err_code) cco_throw_2(err_code, 0)
#define cco_throw_2(err_code, info_data) \
    do { \
        cco_fiber* _fib = (cco_fiber*)_cco_st->fib; \
        _fib->error = (struct cco_error){err_code, __LINE__, __FILE__, info_data}; \
        int _err = cco_SHUTDOWN; \
        if (_cco_st->parent_wg && _fib->parent) switch (_cco_st->parent_wg->on_error) { \
            case cco_SET_SHUTDOWN: _cco_cancel_all(_fib, _cco_st->parent_wg); _err = cco_SHUTDOWN; /* FALLTHRU */ \
            case cco_SET_NOTIFY: _fib->parent->error = _fib->error; _fib->parent->error.code = _err; \
        } \
        cco_return; \
    } while (0)

/* Recover the thrown error; to be used in cco_finalize section upon handling cco_error()->code */
#define cco_recover \
    do { \
        cco_fiber* _fib = (cco_fiber*)_cco_st->fib; \
        c_assert((uint32_t)_fib->error.code & ~cco_SHUTDOWN); \
        _fib->error.code = 0; \
        _fib->task->base.state = _fib->recover_state; \
        goto _resume_lbl; \
    } while (0)

/* Asymmetric coroutine await/call */
#define cco_await_task(...) c_MACRO_OVERLOAD(cco_await_task, __VA_ARGS__)
#define cco_await_task_1(a_task) cco_await_task_2(a_task, cco_DONE)
#define cco_await_task_2(a_task, _awaitbits) do { \
    {   (void)sizeof(cco_env(a_task) == _cco_st->fib->env); \
        cco_task* _wt_task = cco_cast_task(a_task); \
        cco_fiber* _fib = (cco_fiber*)_cco_st->fib; \
        _wt_task->base.awaitbits = (_awaitbits); \
        _wt_task->base.parent_task = _fib->task; \
        _wt_task->base.state.fib = _fib; \
        _wt_task->base.state.parent_wg = &_cco_taskbase()->task_wg; \
        _cco_taskbase()->task_wg.spawn_count = 1; \
        _fib->task = _wt_task; \
    } \
    cco_suspend; \
} while (0)

/* Symmetric coroutine flow of control transfer */
#define cco_yield_to(a_task) do { \
    {   (void)sizeof(cco_env(a_task) == _cco_st->fib->env); \
        cco_task* _to_task = cco_cast_task(a_task); \
        cco_fiber* _fib = (cco_fiber*)_cco_st->fib; \
        _to_task->base.awaitbits = _fib->task->base.awaitbits; \
        _to_task->base.parent_task = NULL; \
        _to_task->base.state.fib = _fib; \
        _to_task->base.state.parent_wg = &_cco_taskbase()->task_wg; \
        _cco_taskbase()->task_wg.spawn_count = 1; \
        _fib->task = _to_task; \
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
#define cco_new_fiber_1(a_task) \
    _cco_new_fiber(cco_cast_task(a_task), NULL)
#define cco_new_fiber_2(a_task, env) \
    _cco_new_fiber(cco_cast_task(a_task), ((void)sizeof((env) == cco_env(a_task)), env))

#define cco_reset_group(a_group) ((a_group)->spawn_count = 0)
#define cco_spawn(...) c_MACRO_OVERLOAD(cco_spawn, __VA_ARGS__)
#define cco_spawn_2(a_task, a_group) cco_spawn_4(a_task, a_group, NULL, _cco_st->fib)
#define cco_spawn_3(a_task, a_group, env) cco_spawn_4(a_task, a_group, env, _cco_st->fib)
#define cco_spawn_4(a_task, a_group, env, fiber) \
    _cco_spawn(cco_cast_task(a_task), a_group, ((void)sizeof((env) == cco_env(a_task)), env), \
               (cco_fiber*)((void)sizeof((fiber)->parent_task), fiber))


#define cco_await_cancel_task(a_task) do { \
    cco_cancel_task(a_task); \
    cco_await_task(a_task); \
} while (0)

#define cco_await_n(n, a_group) do { /* does not cancel remaining */ \
    _cco_st->fib->temp_wg = a_group; \
    _cco_st->fib->temp_wg->await_count = _cco_st->fib->temp_wg->spawn_count - (n); \
    cco_await(_cco_st->fib->temp_wg->spawn_count == _cco_st->fib->temp_wg->await_count); \
} while (0)

#define cco_await_all(a_group) do { \
    _cco_st->fib->temp_wg = a_group; \
    cco_await(_cco_st->fib->temp_wg->spawn_count == 0); \
} while (0)

#define cco_await_any(a_group) do { /* await 1; cancel remaining */ \
    cco_await_n(1, a_group); \
    cco_cancel_all(_cco_st->fib->temp_wg); \
    cco_await_OFFSET(_cco_st->fib->temp_wg->spawn_count == 0, 100000); /* await_all() */ \
} while (0)

#define cco_await_cancel_all(a_group) do { \
    cco_group* _grp = a_group; \
    cco_cancel_all(_grp); \
    cco_await_all(_grp); \
} while (0)


#define cco_run_fiber(...) c_MACRO_OVERLOAD(cco_run_fiber, __VA_ARGS__)
#define cco_run_fiber_1(fiber_ref) \
    for (cco_fiber** _it_ref = (cco_fiber**)((void)sizeof((fiber_ref)[0]->env), fiber_ref) \
        ; (*_it_ref = cco_execute_next(*_it_ref)) != NULL; )
#define cco_run_fiber_2(it, fiber) \
    for (cco_fiber* it = (cco_fiber*)((void)sizeof((fiber)->env), fiber) \
        ; (it = cco_execute_next(it)) != NULL; )

#define cco_run_task(...) c_MACRO_OVERLOAD(cco_run_task, __VA_ARGS__)
#define cco_run_task_1(a_task) cco_run_fiber_2(_fibit, cco_new_fiber_1(a_task))
#define cco_run_task_2(a_task, env) cco_run_fiber_2(_fibit, cco_new_fiber_2(a_task, env))
#define cco_run_task_3(it, a_task, env) cco_run_fiber_2(it, cco_new_fiber_2(a_task, env))

extern int        cco_execute(cco_fiber* fib); // is a coroutine itself
extern cco_fiber* cco_execute_next(cco_fiber* fib);  // resume the next fiber and return it

extern cco_fiber* _cco_new_fiber(cco_task* task, void* env);
extern cco_fiber* _cco_spawn(cco_task* task, cco_group* wg, void* env, cco_fiber* fib);
extern void       _cco_cancel_all(cco_fiber* fib, cco_group* wg);

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
    (c_flt_take(n), fltbase.done ? (_it.base.state.pos = cco_STATE_DROP, _it.base.state.drop = 1) : 1)

#define cco_flt_takewhile(pred) \
    (c_flt_takewhile(pred), fltbase.done ? (_it.base.state.pos = cco_STATE_DROP, _it.base.state.drop = 1) : 1)


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

cco_fiber* _cco_spawn(cco_task* task, cco_group* wg, void* env, cco_fiber* fib) {
    cco_fiber* new_fb = fib->next ? c_new(cco_fiber, {.next = fib->next, .parent = fib}) : fib;
    new_fb->task = task;
    new_fb->env = (env ? env : fib->env);
    task->base.state.fib = fib->next = new_fb;
    task->base.state.parent_wg = wg;
    if (wg) wg->spawn_count += 1;
    return new_fb;
}

cco_fiber* _cco_new_fiber(cco_task* task, void* env) {
    cco_fiber* new_fb = c_new(cco_fiber, {.task=task, .env=env});
    task->base.state.fib = new_fb->next = new_fb;
    return new_fb;
}

void _cco_cancel_all(cco_fiber* fib, cco_group* wg) {
    for (cco_fiber *fit = fib->next; fit != fib; fit = fit->next) {
        cco_task* cur = fit->task;
        do {
            if (cur->base.state.parent_wg == wg) {   
                fit->error.code = cco_CANCEL;
                break;
            }
            cur = cur->base.parent_task;
        } while (cur);
    }
}

cco_fiber* cco_execute_next(cco_fiber* fib) {
    cco_fiber *_next = fib->next, *unlinked;
    int ret = cco_execute(_next);

    if (ret == cco_DONE) {
        unlinked = _next;
        _next = (_next == fib ? NULL : _next->next);
        fib->next = _next;
        c_free_n(unlinked, 1);
    }
    return _next;
}

int cco_execute(cco_fiber* fib) {
    cco_async (fib) {
        while (1) {
            fib->parent_task = fib->task->base.parent_task;
            fib->awaitbits = fib->task->base.awaitbits;
            fib->status = cco_resume(fib->task);
            if (fib->error.code) {
                // Note: if fib->status == cco_DONE, fib->task may already be destructed.
                if (fib->status == cco_DONE) {
                    fib->task = fib->parent_task;
                    if (fib->task == NULL)
                        break;
                    fib->recover_state = fib->task->base.state;
                }
                cco_stop(fib->task);
                cco_suspend;
                continue;
            }
            if (!((fib->status & ~fib->awaitbits) || (fib->task = fib->parent_task) != NULL))
                break;
            cco_suspend;
        }
    }

    if ((uint32_t)fib->error.code & ~(cco_CANCEL | cco_SHUTDOWN)) { // Allow cco_CANCEL not to trigger error.
        fprintf(stderr, __FILE__ ": error: unhandled coroutine error '%d'\n"
                        "%s:%d: cco_throw(%d);\n",
                        fib->error.code, fib->error.file, fib->error.line, fib->error.code);
        exit(-128);
    }
    return cco_DONE;
}
#endif // IMPLEMENT
#undef i_implement
#undef i_static
#undef i_header
