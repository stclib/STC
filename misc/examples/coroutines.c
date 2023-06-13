#include <stc/algo/coroutine.h>
#include <stdio.h>
#include <stdint.h>

// Demonstrate to call another coroutine from a coroutine:
// First create prime generator, then call fibonacci sequence:
typedef long long llong;

bool is_prime(int64_t i) {
    for (llong j=2; j*j <= i; ++j)
        if (i % j == 0) return false;
    return true;
}

struct prime {
    int count, idx;
    llong result, pos;
    int cco_state;
};

bool prime(struct prime* g) {
    cco_routine(g) {
        if (g->result < 2) g->result = 2;
        if (g->result == 2) {
            if (g->count-- == 0) cco_return;
            ++g->idx;
            cco_yield(false);
        }
        g->result += !(g->result & 1);
        for (g->pos = g->result; g->count > 0; g->pos += 2) {
            if (is_prime(g->pos)) {
                --g->count;
                ++g->idx;
                g->result = g->pos;
                cco_yield(false);
            }
        }
    cco_final:
        printf("final prm\n");
    }
    return true;
}


// Use coroutine to create a fibonacci sequence generator:

struct fibonacci {
    int count, idx;
    llong result, b;
    int cco_state;
};

bool fibonacci(struct fibonacci* g) {
    assert(g->count < 94);

    cco_routine(g) {
        g->idx = 0;
        g->result = 0;
        g->b = 1;
        for (;;) {
            if (g->count-- == 0)
                cco_return;
            if (++g->idx > 1) {
                // NB! locals lasts only until next cco_yield/cco_await!
                llong sum = g->result + g->b;
                g->result = g->b;
                g->b = sum;
            }
            cco_yield(false);
        }
    cco_final:
        printf("final fib\n");
    }
    return true;
}

// Combine

struct combined {
    struct prime prm;
    struct fibonacci fib;
    int cco_state;
};


void combined(struct combined* g) {
    cco_routine(g) {
        cco_await(prime(&g->prm));
        cco_await(fibonacci(&g->fib));

        // Reuse the g->prm context and extend the count:
        g->prm.count = 8, g->prm.result += 2;
        cco_reset(&g->prm);
        cco_await(prime(&g->prm));

    cco_final:
        puts("final combined");
    }
}

int main(void)
{
    struct combined c = {.prm={.count=8}, .fib={14}};

    cco_block_on(&c, combined) {
        printf("Prime(%d)=%lld, Fib(%d)=%lld\n", 
            c.prm.idx, c.prm.result, 
            c.fib.idx, c.fib.result);
    }
}
