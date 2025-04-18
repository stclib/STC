#include "stc/coroutine.h"
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

cco_task_struct (prime) {
    prime_state cco;
    int count;
    long long result;
};

int prime(struct prime* g, cco_fiber* fb) {
    (void)fb;
    cco_routine (g) {
        if (g->result <= 2) {
            g->result = 2;
            if (g->count-- == 0)
                cco_return;
            cco_yield_v(YIELD_PRM);
        }
        for (g->result |= 1; g->count > 0; g->result += 2) {
            if (is_prime(g->result)) {
                --g->count;
                cco_yield_v(YIELD_PRM);
            }
        }
        cco_finally:
        puts("DONE prm");
    }
    return 0;
}


// Use coroutine to create a fibonacci sequence generator:

cco_task_struct (fibonacci) {
    fibonacci_state cco;
    int count;
    long long result, b;
};

int fibonacci(struct fibonacci* g, cco_fiber* fb) {
    (void)fb;
    assert(g->count < 94);
    cco_routine (g) {
        if (g->result == 0)
            g->b = 1;
        while (true) {
            if (g->count-- == 0)
                cco_return;
            // NB! locals lasts only until next yield/await!
            long long tmp = g->result;
            g->result = g->b;
            g->b += tmp;
            cco_yield_v(YIELD_FIB);
        }
        cco_finally:
        puts("DONE fib");
    }
    return 0;
}

int main(void) {
    struct prime prm;
    struct fibonacci fib;

    puts("SERIAL:");
    prm = (struct prime){{prime}, .count=8};
    fib = (struct fibonacci){{fibonacci}, .count=12};

    cco_run_task(&prm) {
        printf("  Prime=%lld\n", prm.result);
    }
    cco_run_task(&fib) {
        printf("  Fibon=%lld\n", fib.result);
    }
    puts("");


    puts("CONCURRENT:");
    prm = (struct prime){{prime}, .count=8};
    fib = (struct fibonacci){{fibonacci}, .count=12};

    cco_fiber* fb = c_new(cco_fiber, 0);
    cco_spawn(&prm, fb);
    cco_spawn(&fib, fb);

    cco_run_fiber(run, fb) {
        switch (run->result) {
            case YIELD_PRM:
                printf("  Prime=%lld\n", prm.result);
                break;
            case YIELD_FIB:
                printf("  Fibon=%lld\n", fib.result);
                break;
        }
    }
}
