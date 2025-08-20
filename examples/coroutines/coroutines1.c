#include <stc/coroutine.h>
#include <stdio.h>
#include <stdint.h>

// Demonstrate calling two coroutine from a coroutine:
// First call them concurrently, then in parallel:


// Suspend yield values
enum { YIELD_PRM = 1<<0, YIELD_FIB = 1<<1};

bool is_prime(long long i) {
    for (long long j=2; j*j <= i; ++j)
        if (i % j == 0) return false;
    return true;
}

struct prime {
    cco_base base;
    int count;
    long long value;
};

int prime(struct prime* o) {
    cco_async (o) {
        if (o->value <= 2) {
            o->value = 2;
            if (o->count-- == 0)
                cco_return;
            cco_yield_v(YIELD_PRM);
        }
        for (o->value |= 1; o->count > 0; o->value += 2) {
            if (is_prime(o->value)) {
                --o->count;
                cco_yield_v(YIELD_PRM);
            }
        }
        cco_finalize:
        puts("DONE prm");
    }
    return 0;
}


// Use coroutine to create a fibonacci sequence generator:

struct fibonacci {
    cco_base base;
    int count;
    long long value, b;
};

int fibonacci(struct fibonacci* o) {
    cco_async (o) {
        assert(o->count < 94);
        if (o->value == 0)
            o->b = 1;

        while (true) {
            if (o->count-- == 0)
                cco_return;
            // NB! locals lasts only until next yield/await!
            long long tmp = o->value;
            o->value = o->b;
            o->b += tmp;
            cco_yield_v(YIELD_FIB);
        }

        cco_finalize:
        puts("DONE fib");
    }
    return 0;
}


struct combined {
    struct prime prm;
    struct fibonacci fib;
    cco_base base;
};

int combined(struct combined* o) {
    cco_async (o) {
        puts("SERIAL:");
        o->prm = (struct prime){.count = 8};
        o->fib = (struct fibonacci){.count = 12};

        cco_await_coroutine( prime(&o->prm) );
        cco_await_coroutine( fibonacci(&o->fib) );

        puts("\nCONCURRENT:");
        o->prm = (struct prime){.count = 8};
        o->fib = (struct fibonacci){.count = 12};

        cco_await_coroutine( prime(&o->prm) | fibonacci(&o->fib) );

        cco_finalize:
        puts("DONE prime and fib");
    }
    return 0;
}


int main(void) {
    struct combined comb = {0};
    int res;

    cco_run_coroutine(res = combined(&comb)) {
        if (res & YIELD_PRM)
            printf("  Prime=%lld\n", comb.prm.value);
        if (res & YIELD_FIB)
            printf("  Fibon=%lld\n", comb.fib.value);
    }
}
