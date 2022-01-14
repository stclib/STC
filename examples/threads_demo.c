// Example translated to C from https://en.cppreference.com/w/cpp/memory/shared_ptr

#include <stdio.h>
#include <threads.h>
#include <time.h>

struct Base
{
    int value;
} typedef Base;

#define i_type BaseArc
#define i_val Base
#define i_valdrop(x) printf("Drop Base: %d\n", (x)->value)
#define i_opt c_no_cmp
#include <stc/carc.h>

mtx_t mtx;

void* thr(BaseArc* p)
{
    struct timespec one_sec = {.tv_sec=1};
    thrd_sleep(&one_sec, NULL);
    BaseArc lp = BaseArc_clone(*p);  // thread-safe, even though the
                                     // shared use_count is incremented
    c_autoscope (mtx_lock(&mtx), mtx_unlock(&mtx))
    {
        printf("local pointer in a thread:\n"
               "  p.get() = %p, p.use_count() = %ld\n", (void*)lp.get, BaseArc_use_count(lp));
        /* safe to modify base here */
        lp.get->value += 1;
    }
    BaseArc_drop(&lp);
    BaseArc_drop(p);
    return NULL;
}

int main()
{
    BaseArc p = BaseArc_from((Base){0});

    mtx_init(&mtx, mtx_plain);
    printf("Created a shared Base\n"
           "  p.get() = %p, p.use_count() = %ld\n", (void*)p.get, BaseArc_use_count(p));
    enum {N = 3};
    struct { thrd_t t; BaseArc p; } task[N];
    c_forrange (i, N) { 
        task[i].p = BaseArc_clone(p);
        thrd_create(&task[i].t, (thrd_start_t)thr, &task[i].p);
    }

    BaseArc_reset(&p);
    printf("Shared ownership between %d threads and released\n"
           "ownership from main:\n"
           "  p.get() = %p, p.use_count() = %ld\n", N, (void*)p.get, BaseArc_use_count(p));

    c_forrange (i, N) thrd_join(task[i].t, NULL);
    printf("All threads completed, the last one deleted Base\n");
}

#ifdef STC_TINYCTHREAD_H_
#include "../src/threads.c"
#endif
