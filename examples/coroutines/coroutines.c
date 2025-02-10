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

struct prime {
    int count;
    long long result;
    cco_state cco;
};

int prime(struct prime* g) {
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

struct fibonacci {
    int count;
    long long result, b;
    cco_state cco;
};

int fibonacci(struct fibonacci* g) {
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

// Combine

struct combined {
    struct prime prm;
    struct fibonacci fib;
    cco_state cco;
};

int combined(struct combined* g) {
    cco_routine (g) {
        puts("SEQUENCED:");
        g->prm = (struct prime){.count = 8}, g->fib = (struct fibonacci){.count = 12};
        cco_await_coroutine( prime(&g->prm) );
        cco_await_coroutine( fibonacci(&g->fib) );

        puts("\nPARALLEL, AWAIT ALL:");
        g->prm = (struct prime){.count = 8}, g->fib = (struct fibonacci){.count = 12};
        cco_await_coroutine( prime(&g->prm) |
                             fibonacci(&g->fib) );
        cco_finally:
        puts("DONE prime and fib");
    }
    return 0;
}


int main(void) {
    struct combined c = {0};
    int res;

    cco_run_coroutine(res = combined(&c)) {
        if (res & YIELD_PRM)
            printf("Prime=%lld\n", c.prm.result);
        if (res & YIELD_FIB)
            printf("Fibon=%lld\n", c.fib.result);
    }
}
