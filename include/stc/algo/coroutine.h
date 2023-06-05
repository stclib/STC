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
#define cco_done(co) ((co)->cco_state == cco_state_done)

#define cco_routine(co) \
    for (int *_state = &(co)->cco_state, _once=1; _once; *_state = cco_state_done, _once=0) \
        _begin: switch (*_state) case 0: // thanks, @liigo!

#define cco_yield(ret) \
    do { \
        *_state = __LINE__; return ret; goto _begin; \
        case __LINE__:; \
    } while (0)

#define cco_await(...) c_MACRO_OVERLOAD(cco_await, __VA_ARGS__)
#define cco_await_1(promise) cco_await_2(promise, )
#define cco_await_2(promise, ret) \
    do { \
        *_state = __LINE__; \
        case __LINE__: if (!(promise)) {return ret; goto _begin;} \
    } while (0)

#define cco_run(co, call) while (call, !cco_done(co))

#define cco_final \
    case cco_state_final

#define cco_return \
    do { *_state = cco_state_final; goto _begin; } while (0)

#define cco_return_v(value) \
    return (*_state = cco_state_final, value) 

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

typedef struct {
    intptr_t count;
} cco_sem;

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
    struct _FILETIME; struct _SECURITY_ATTRIBUTES; union _LARGE_INTEGER;
    _c_LINKC void GetSystemTimePreciseAsFileTime(struct _FILETIME*);
    _c_LINKC void* CreateWaitableTimerW(struct _SECURITY_ATTRIBUTES*, int, const wchar_t*);
    _c_LINKC int SetWaitableTimer(void*, const union _LARGE_INTEGER*, long, void(*)(void*, unsigned long, unsigned long), void*, int);
    _c_LINKC unsigned long WaitForSingleObject(void*, unsigned long);
    _c_LINKC int CloseHandle(void*);

    static inline long long cco_utime(void) {
        static const long long epoch_offset = 11644473600000000LL; /* microseconds betweeen Jan 1,1601 - Jan 1,1970 */
        unsigned long long quad;          /* 64-bit value, 100-nanosecond intervals since January 1, 1601 00:00 UTC */
        GetSystemTimePreciseAsFileTime((struct _FILETIME*)&quad);
        return (long long)quad/10 - epoch_offset; /* microseconds since epoch */
    }

    static inline void cco_usleep(long long usec) {
        unsigned long long ft = -10*usec;
        void* timer = CreateWaitableTimerW(NULL, true, NULL);
        SetWaitableTimer(timer, (const union _LARGE_INTEGER*)&ft, 0, NULL, NULL, 0);
        WaitForSingleObject(timer, ~0UL);
        CloseHandle(timer);
    }    
#else
    #include <sys/time.h>
    static inline long long cco_utime(void) {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_sec*1000000LL + tv.tv_usec;
    }

    static inline void cco_usleep(long long usec) {
        struct timeval tv = {.tv_sec=(time_t)(usec/1000000), .tv_usec=(suseconds_t)(usec % 1000000)};
        select(0, NULL, NULL, NULL, &tv);
    }
#endif

typedef struct { long long interval, start; } cco_timer;

#define cco_timer_await(...) c_MACRO_OVERLOAD(cco_timer_await, __VA_ARGS__)
#define cco_timer_await_2(tm, usec) cco_timer_await_3(tm, usec, )
#define cco_timer_await_3(tm, usec, ret) \
    do { \
        cco_timer_start(tm, usec); \
        cco_await_2(cco_timer_expired(tm), ret); \
    } while (0)

static inline void cco_timer_start(cco_timer* tm, long long usec) {
    tm->interval = usec;
    tm->start = cco_utime();
}

static inline cco_timer cco_timer_from(long long usec) {
    cco_timer tm = {.interval=usec, .start=cco_utime()};
    return tm;
}

static inline void cco_timer_restart(cco_timer* tm) {
    tm->start = cco_utime();
}

static inline bool cco_timer_expired(cco_timer* tm) {
    return cco_utime() - tm->start >= tm->interval;
}

static inline long long cco_timer_remaining(cco_timer* tm) {
    return tm->start + tm->interval - cco_utime();
}

#endif
