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

void Base_del(Base* b) { printf("Base::~Base()\n"); }

#define i_val Base
#define i_del Base_del
#define i_tag base
#include <stc/csptr.h>

void* thr(csptr_base* lp)
{
    sleep(1);
    static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
    c_autoscope (pthread_mutex_lock(&mtx), pthread_mutex_unlock(&mtx))
    {
        printf("local pointer in a thread:\n"
                "  p.get() = %p, p.use_count() = %ld\n", (void*)lp->get, *lp->use_count);
    }
    /* atomically decrease ref. */
    csptr_base_del(lp);
    return NULL;
}

int main()
{
    csptr_base p = csptr_base_new((Base){42});

    printf("Created a Base\n"
           "  p.get() = %p, p.use_count() = %ld\n", (void*)p.get, *p.use_count);
    enum {N = 3};
    pthread_t t[N];
    csptr_base c[N];
    c_forrange (i, N) {
        c[i] = csptr_base_clone(p);
        pthread_create(&t[i], NULL, (void*(*)(void*))thr, &c[i]);
    }

    printf("Shared ownership between %d threads and released\n"
           "ownership from main:\n"
           "  p.get() = %p, p.use_count() = %ld\n", N, (void*)p.get, *p.use_count);
    csptr_base_reset(&p);

    c_forrange (i, N) pthread_join(t[i], NULL);
    printf("All threads completed, the last one deleted Base\n");
}

#else
int main() {}
#endif
