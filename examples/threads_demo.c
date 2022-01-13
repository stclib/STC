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
    thrd_sleep(&(struct timespec){.tv_sec=1}, NULL); // sleep 1 sec
    BaseArc lp = BaseArc_clone(*p);  // thread-safe, even though the
                                     // shared use_count is incremented

    c_autoscope (mtx_lock(&mtx), mtx_unlock(&mtx))
    {
        printf("local pointer in a thread:\n"
               "  p.get() = %p, p.use_count() = %ld\n", (void*)lp.get, BaseArc_use_count(lp));
        /* safe to modify base here */
        lp.get->value += 1;
    }
    /* atomically decrease ref. */
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
    thrd_t t1, t2, t3;
    thrd_create(&t1, (thrd_start_t)thr, (BaseArc[]){BaseArc_clone(p)});
    thrd_create(&t2, (thrd_start_t)thr, (BaseArc[]){BaseArc_clone(p)});
    thrd_create(&t3, (thrd_start_t)thr, (BaseArc[]){BaseArc_clone(p)});

    BaseArc_reset(&p);
    printf("Shared ownership between 3 threads and released\n"
           "ownership from main:\n"
           "  p.get() = %p, p.use_count() = %ld\n", (void*)p.get, BaseArc_use_count(p));

    thrd_join(t1, NULL); thrd_join(t3, NULL); thrd_join(t3, NULL);
    printf("All threads completed, the last one deleted Base\n");
}

#ifdef STC_TINYCTHREAD_H_
#include "../src/threads.c"
#endif
