#include <stc/coroutine.h>
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
    int count, idx;
    long long result, pos;
    int cco_state;
};

int prime(struct prime* g) {
    cco_routine(g) {
        if (g->result < 2) g->result = 2;
        if (g->result == 2) {
            if (g->count-- == 0) cco_return;
            ++g->idx;
            cco_yield();
        }
        g->result += !(g->result & 1);
        for (g->pos = g->result; g->count > 0; g->pos += 2) {
            if (is_prime(g->pos)) {
                --g->count;
                ++g->idx;
                g->result = g->pos;
                cco_yield();
            }
        }
        cco_final:
        printf("final prm\n");
    }
    return 0;
}


// Use coroutine to create a fibonacci sequence generator:

struct fibonacci {
    int count, idx;
    long long result, b;
    int cco_state;
};

int fibonacci(struct fibonacci* g) {
    assert(g->count < 94);

    long long sum;
    cco_routine(g) {
        g->idx = 0;
        g->result = 0;
        g->b = 1;
        for (;;) {
            if (g->count-- == 0)
                cco_return;
            if (++g->idx > 1) {
                // NB! locals lasts only until next yield/await!
                sum = g->result + g->b;
                g->result = g->b;
                g->b = sum;
            }
            cco_yield();
        }
        cco_final:
        printf("final fib\n");
    }
    return 0;
}

// Combine

struct combined {
    struct prime prm;
    struct fibonacci fib;
    int cco_state;
};

int combined(struct combined* g) {
    cco_routine(g) {
        cco_await_call(prime(&g->prm));
        cco_await_call(fibonacci(&g->fib));

        // Reuse the g->prm context and extend the count:
        g->prm.count = 8, g->prm.result += 2;
        cco_reset(&g->prm);
        cco_await_call(prime(&g->prm));

        cco_final:
        puts("final combined");
    }
    return 0;
}

int main(void)
{
    struct combined c = {.prm={.count=8}, .fib={14}};
    int res;

    cco_blocking_call(res = combined(&c)) {
        if (res == CCO_YIELD)
            printf("Prime(%d)=%lld, Fib(%d)=%lld\n", 
                c.prm.idx, c.prm.result, 
                c.fib.idx, c.fib.result);
    }
}
