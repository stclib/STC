// example based on https://en.cppreference.com/w/cpp/memory/shared_ptr

#include <stdio.h>
#include <unistd.h>
#include <threads.h>
#include <time.h>

struct Base
{
    int value;
} typedef Base;

#define i_type BaseRc
#define i_val Base
#define i_valdrop(x) printf("Drop Base: %d\n", (x)->value)
#define i_opt c_no_cmp
#include <stc/carc.h>

mtx_t mtx;

void* thr(BaseRc* lp)
{
    sleep(1);
    c_autoscope (mtx_lock(&mtx), mtx_unlock(&mtx))
    {
        printf("local pointer in a thread:\n"
               "  p.get() = %p, p.use_count() = %ld\n", (void*)lp->get, *lp->use_count);
        /* safe to modify base here */
        lp->get->value += 1;
    }
    /* atomically decrease ref. */
    BaseRc_drop(lp);
    return NULL;
}

int main()
{
    BaseRc p = BaseRc_from((Base){0});

    mtx_init(&mtx, mtx_plain);
    printf("Created a Base\n"
           "  p.get() = %p, p.use_count() = %ld\n", (void*)p.get, *p.use_count);
    enum {N = 3};
    thrd_t t[N];
    BaseRc c[N];
    c_forrange (i, N) {
        c[i] = BaseRc_clone(p);
        thrd_create(&t[i], (thrd_start_t)thr, &c[i]);
    }

    printf("Shared ownership between %d threads and released\n"
           "ownership from main:\n"
           "  p.get() = %p, p.use_count() = %ld\n", N, (void*)p.get, *p.use_count);
    BaseRc_reset(&p);

    c_forrange (i, N) thrd_join(t[i], NULL);
    printf("All threads completed, the last one deleted Base\n");
}

#ifdef STC_TINYCTHREAD_H_
#include "../src/threads.c"
#endif
