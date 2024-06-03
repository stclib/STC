#include "stc/coroutine.h"
#include <stdio.h>
#include <stdint.h>

// Demonstrate to call another coroutine from a coroutine:
// First create prime generator, then call fibonacci sequence:

bool is_prime(long long i) {
    for (long long j=2; j*j <= i; ++j)
        if (i % j == 0) return false;
    return true;
}

struct prime {
    int count;
    long long value;
    int cco_state;
};

enum { YIELD_PRM = 1<<0, YIELD_FIB = 1<<1};

int prime(struct prime* g) {
    cco_scope(g) {
        if (g->value < 2)
            g->value = 2;
        if (g->value == 2) {
            if (g->count-- == 0)
                cco_return;
            cco_yield_v(YIELD_PRM);
        }
        for (g->value |= 1; g->count > 0; g->value += 2) {
            if (is_prime(g->value)) {
                --g->count;
                cco_yield_v(YIELD_PRM);
            }
        }
        cco_final:
        puts("DONE prm");
    }
    return 0;
}


// Use coroutine to create a fibonacci sequence generator:

struct fibonacci {
    int count;
    long long value, b;
    int cco_state;
};

int fibonacci(struct fibonacci* g) {
    assert(g->count < 94);

    cco_scope(g) {
        if (g->value == 0)
            g->b = 1;
        while (true) {
            if (g->count-- == 0)
                cco_return;
            // NB! locals lasts only until next yield/await!
            long long tmp = g->value;
            g->value = g->b;
            g->b += tmp;
            cco_yield_v(YIELD_FIB);
        }
        cco_final:
        puts("DONE fib");
    }
    return 0;
}

// Combine

struct combined {
    struct prime prm;
    struct fibonacci fib;
    int cco_state;
};

int sequenced(struct combined* g) {
    cco_scope(g) {
        cco_await_coroutine( prime(&g->prm) );
        cco_await_coroutine( fibonacci(&g->fib) );

        cco_final:
        puts("DONE sequenced");
    }
    return 0;
}

int parallel(struct combined* g) {
    cco_scope(g) {
        cco_await_coroutine( prime(&g->prm) | fibonacci(&g->fib) );

        cco_final:
        puts("DONE parallel");
    }
    return 0;
}



int main(void)
{
    struct combined c = {.prm={.count=8}, .fib={.count=12}};
    int res;

    puts("SEQUENCED");
    cco_run_coroutine(res = sequenced(&c)) {
        if (res & YIELD_PRM) 
            printf("Prime=%lld\n", c.prm.value);
        if (res & YIELD_FIB)
            printf("Fibon=%lld\n", c.fib.value);
    }
    
    c = (struct combined){.prm={.count=12}, .fib={.count=8}};
    puts("PARALLEL");
    cco_run_coroutine(res = parallel(&c)) {
        if (res & YIELD_PRM) 
            printf("Prime=%lld\n", c.prm.value);
        if (res & YIELD_FIB)
            printf("Fibon=%lld\n", c.fib.value);
    }
}
