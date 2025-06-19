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
    CCO_STATE_CLEANUP = -1, // [deprecated]
    CCO_STATE_DONE = -2,
};
typedef enum {
    CCO_DONE = 0,
    CCO_YIELD_FINAL = 1<<27,
    CCO_YIELD = 1<<28,
    CCO_AWAIT = 1<<29,
    CCO_NOOP = 1<<30,
} cco_result;

typedef struct {
    int state;
} cco_base, cco_state; // [deprecated] cco_state
#define cco base       // [deprecated] cco

#define cco_initial(co) ((co)->base.state == CCO_STATE_INIT)
#define cco_suspended(co) ((co)->base.state > CCO_STATE_INIT)
#define cco_done(co) ((co)->base.state == CCO_STATE_DONE)
#define cco_active(co) ((co)->base.state != CCO_STATE_DONE)

#if defined STC_HAS_TYPEOF && STC_HAS_TYPEOF
    #define _cco_validate_task_struct(co) \
    c_static_assert(/* error: co->base not first member in task struct */ \
                    sizeof((co)->base) == sizeof(cco_base) || \
                    offsetof(__typeof__(*(co)), base) == 0)
#else
    #define _cco_validate_task_struct(co) (void)0
#endif

#define cco_async(co) \
    for (int *_state = (_cco_validate_task_struct(co), &(co)->base.state) \
           ; *_state != CCO_STATE_DONE ; *_state = CCO_STATE_DONE) \
        _resume: switch (*_state) case CCO_STATE_INIT: // thanks, @liigo!

#define cco_routine(co) cco_async(co) // [deprecated] cco_routine

/* Throw an error "exception"; can be catched up in the cco_await_task call tree */
#define cco_throw_error(error_code, fiber) \
    do {cco_fiber* _fb = fiber; \
        _fb->error = error_code; \
        _fb->error_line = __LINE__; \
        cco_return; \
    } while (0)

#define cco_recover_error(fiber) \
    do {cco_fiber* _fb = fiber; \
        c_assert(_fb->task->base.state == CCO_STATE_DONE); \
        _fb->task->base.state = _fb->recover_state; \
        _fb->error = 0; \
        goto _resume; \
    } while (0)

#define cco_cleanup /* [deprecated] */ \
    *_state = CCO_STATE_CLEANUP; /* FALLTHRU */ \
    case CCO_STATE_CLEANUP
#define cco_finally cco_cleanup // [deprecated]

#define cco_return \
    do { \
        *_state = *_state >= CCO_STATE_INIT ? CCO_STATE_CLEANUP : CCO_STATE_DONE; \
        goto _resume; \
    } while (0)

#define cco_yield cco_yield_v(CCO_YIELD)
#define cco_yield_v(value) \
    do { \
        *_state = __LINE__; return value; goto _resume; \
        case __LINE__:; \
    } while (0)

#define cco_yield_final cco_yield_final_v(CCO_YIELD)
#define cco_yield_final_v(value) \
    do { \
        *_state = *_state >= CCO_STATE_INIT ? CCO_STATE_CLEANUP : CCO_STATE_DONE; \
        return value; \
    } while (0)

#define cco_await(until) \
    do { \
        *_state = __LINE__; /* FALLTHRU */ \
        case __LINE__: if (!(until)) {return CCO_AWAIT; goto _resume;} \
    } while (0)

/* cco_await_coroutine(): assumes coroutine returns a cco_result value (int) */
#define cco_await_coroutine(...) c_MACRO_OVERLOAD(cco_await_coroutine, __VA_ARGS__)
#define cco_await_coroutine_1(corocall) cco_await_coroutine_2(corocall, CCO_DONE)
#define cco_await_coroutine_2(corocall, awaitbits) \
    do { \
        *_state = __LINE__; /* FALLTHRU */ \
        case __LINE__: { \
            int _res = corocall; \
            if (_res & ~(awaitbits)) { return _res; goto _resume; } \
        } \
    } while (0)

/* cco_run_coroutine(): assumes coroutine returns a cco_result value (int) */
#define cco_run_coroutine(corocall) \
    while ((1 ? (corocall) : -1) != CCO_DONE)

#define cco_stop(co) \
    do { \
        int* _st = &(co)->base.state; \
        *_st = *_st >= CCO_STATE_INIT ? CCO_STATE_CLEANUP : CCO_STATE_DONE; \
    } while (0)

#define cco_reset_state(co) \
    (void)((co)->base.state = CCO_STATE_INIT)


/*
 * Tasks and Fibers
 */

typedef struct cco_fiber {
    struct cco_task* task;
    void* env;
    struct cco_task* parent_task;
    struct cco_fiber* next;
    int recover_state, awaitbits, result;
    int error, error_line;
    cco_base base;
} cco_fiber, cco_runtime; // [deprecated] cco_runtime

/* Define a Task struct */
#define cco_task_struct(Task) \
    struct Task; \
    typedef struct { \
        int (*func)(struct Task*, cco_fiber*); \
        int state, awaitbits; \
        struct cco_task* parent_task; \
    } Task##_base, Task##_state; /* [deprecated] Task##_state */ \
    struct Task

/* Base cco_task type */
cco_task_struct(cco_task) { cco_task_base base; };
typedef struct cco_task cco_task;

#define cco_cast_task(...) \
    ((cco_task *)(__VA_ARGS__) + (1 ? 0 : sizeof((__VA_ARGS__)->base.func(__VA_ARGS__, (cco_fiber*)0))))

#define cco_resume_task(task, fiber) \
    _cco_resume_task(cco_cast_task(task), fiber)

/* Stop and immediate cleanup */
#define cco_cancel_task(task, fiber) \
    _cco_cancel_task(cco_cast_task(task), fiber)

static inline int _cco_resume_task(cco_task* task, cco_fiber* fiber)
    { return task->base.func(task, fiber); }
static inline int _cco_cancel_task(cco_task* task, cco_fiber* fiber)
    { cco_stop(task); return task->base.func(task, fiber); }

/* Asymmetric coroutine await/call */
#define cco_await_task(...) c_MACRO_OVERLOAD(cco_await_task, __VA_ARGS__)
#define cco_await_task_2(a_task, fiber) cco_await_task_3(a_task, fiber, CCO_DONE)
#define cco_await_task_3(a_task, fiber, _awaitbits) do { \
    {   cco_task* _await_task = cco_cast_task(a_task); \
        cco_fiber* _fb = fiber; \
        _await_task->base.awaitbits = (_awaitbits); \
        _await_task->base.parent_task = _fb->task; \
        _fb->task = _await_task; \
    } \
    cco_yield_v(CCO_NOOP); \
} while (0)

/* Symmetric coroutine flow of control transfer */
#define cco_yield_to(a_task, fiber) do { \
    {   cco_task* _to_task = cco_cast_task(a_task); \
        cco_fiber* _fb = fiber; \
        _to_task->base.awaitbits = _fb->task->base.awaitbits; \
        _to_task->base.parent_task = _fb->task->base.parent_task; \
        _fb->task = _to_task; \
    } \
    cco_yield_v(CCO_NOOP); \
} while (0)

/*
 * cco_run_fiber()/cco_run_task(): Run fibers/tasks in parallel
 */
#define cco_new_task(Task, ...) \
    (c_new(struct Task, {{.func=Task}, __VA_ARGS__}))

#define cco_new_fiber(...) c_MACRO_OVERLOAD(cco_new_fiber, __VA_ARGS__)
#define cco_new_fiber_1(task) cco_new_fiber_2(task, NULL)
#define cco_new_fiber_2(task, env) _cco_new_fiber(cco_cast_task(task), env)

#define cco_spawn(...) c_MACRO_OVERLOAD(cco_spawn, __VA_ARGS__)
#define cco_spawn_2(task, fiber) cco_spawn_3(task, fiber, NULL)
#define cco_spawn_3(task, fiber, env) _cco_spawn(cco_cast_task(task), fiber, env)

#define cco_run_fiber(...) c_MACRO_OVERLOAD(cco_run_fiber, __VA_ARGS__)
#define cco_run_fiber_1(fiber_ref) \
    cco_run_fiber_2(_it_fb, *(fiber_ref))
#define cco_run_fiber_2(it_fiber, fiber) \
    for (cco_fiber* it_fiber = fiber; (it_fiber = cco_resume_next(it_fiber)) != NULL; )

#define cco_run_task(...) c_MACRO_OVERLOAD(cco_run_task, __VA_ARGS__)
#define cco_run_task_1(task) cco_run_task_2(task, NULL)
#define cco_run_task_2(task, env) cco_run_fiber_2(_it_fb, cco_new_fiber_2(task, env))
#define cco_run_task_3(it_fiber, task, env) cco_run_fiber_2(it_fiber, cco_new_fiber_2(task, env))

static inline bool cco_joined(const cco_fiber* fiber)
    { return fiber == fiber->next; }
#define cco_is_joined(fb) cco_joined(fb) // [deprecated]

extern cco_fiber* _cco_new_fiber(cco_task* task, void* env);
extern cco_fiber* _cco_spawn(cco_task* task, cco_fiber* fb, void* env);
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
            fb->result = cco_resume_task(fb->task, fb);
            if (fb->error) {
                fb->task = fb->parent_task;
                if (fb->task == NULL)
                    break;
                fb->recover_state = fb->task->base.state;
                cco_stop(fb->task);
                continue;
            }
            if (!((fb->result & ~fb->awaitbits) || (fb->task = fb->parent_task) != NULL))
                break;
            cco_yield_v(CCO_NOOP);
        }
    }

    if (fb->error != 0) {
        fprintf(stderr, __FILE__ ":%d: error: unhandled error '%d' in a coroutine task at line %d.\n",
                        __LINE__, fb->error, fb->error_line);
        exit(fb->error);
    }
    return 0;
}

cco_fiber* cco_resume_next(cco_fiber* prev) {
    cco_fiber *curr = prev->next, *unlink;
    int ret = cco_resume_current(curr);

    if (ret == CCO_DONE) {
        unlink = curr;
        curr = (curr == prev ? NULL : curr->next);
        prev->next = curr;
        free(unlink);
    }
    return curr;
}

cco_fiber* _cco_new_fiber(cco_task* _task, void* env) {
    cco_fiber* new_fb = c_new(cco_fiber, {.task=_task, .env=env});
    return (new_fb->next = new_fb);
}

cco_fiber* _cco_spawn(cco_task* _task, cco_fiber* fb, void* env) {
    cco_fiber* new_fb;
    new_fb = fb->next = (fb->next == NULL ? fb : c_new(cco_fiber, {.next=fb->next}));
    new_fb->task = _task;
    new_fb->env = (env == NULL ? fb->env : env);
    return new_fb;
}

#undef i_implement
#endif

/* // Iterators for coroutine generators
 *
 * typedef struct { // A generator data struct
 *     Data data; ...
 * } Gen, Gen_value;
 *
 * // Define the iterator coroutine struct
 * typedef struct {
 *     cco_base base; // or a task base state.
 *     Gen* ref;
 *     ...
 * } Gen_iter;
 *
 * // Define the iterator coroutine func; produce the next value:
 * int Gen_next(Gen_iter* it) {
 *     cco_async (it) {
 *        ... it->ref->data ...
 *        cco_yield; // suspend exec, gen with value ready
 *        ...
 *     }
 *     it->ref = NULL; // stops the iteration
 *     return 0;
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
 * for (c_each(i, Gen, gen))
 *     printf("%d ", *i.ref);
 */

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
    (c_flt_take(n), fltbase.done ? _it.base.state = CCO_STATE_CLEANUP : 1)

#define cco_flt_takewhile(pred) \
    (c_flt_takewhile(pred), fltbase.done ? _it.base.state = CCO_STATE_CLEANUP : 1)


/*
 * Semaphore
 */

typedef struct { ptrdiff_t count; } cco_semaphore;

#define cco_make_semaphore(value) (c_literal(cco_semaphore){value})
#define cco_set_semaphore(sem, value) ((sem)->count = value)
#define cco_acquire_semaphore(sem) (--(sem)->count)
#define cco_release_semaphore(sem) (++(sem)->count)

#define cco_await_semaphore(sem) \
    do { \
        cco_await((sem)->count > 0); \
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
// [deprecated]:
#define cco_sleep_sec cco_sleep
#define cco_make_timer_sec cco_make_timer
#define cco_timer_elapsed_sec cco_timer_elapsed
#define cco_timer_remaining_sec cco_timer_remaining
#define cco_await_timer_sec cco_await_timer

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
