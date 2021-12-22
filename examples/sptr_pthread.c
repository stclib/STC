// example based on https://en.cppreference.com/w/cpp/memory/shared_ptr
#if defined __GNUC__ || defined __linux__

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
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

void* thr(BaseRc* lp)
{
    sleep(1);
    static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
    c_autoscope (pthread_mutex_lock(&mtx), pthread_mutex_unlock(&mtx))
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

    printf("Created a Base\n"
           "  p.get() = %p, p.use_count() = %ld\n", (void*)p.get, *p.use_count);
    enum {N = 3};
    pthread_t t[N];
    BaseRc c[N];
    c_forrange (i, N) {
        c[i] = BaseRc_clone(p);
        pthread_create(&t[i], NULL, (void*(*)(void*))thr, &c[i]);
    }

    printf("Shared ownership between %d threads and released\n"
           "ownership from main:\n"
           "  p.get() = %p, p.use_count() = %ld\n", N, (void*)p.get, *p.use_count);
    BaseRc_reset(&p);

    c_forrange (i, N) pthread_join(t[i], NULL);
    printf("All threads completed, the last one deleted Base\n");
}

#else
int main() {}
#endif
