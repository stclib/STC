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
    cco_begin(g);
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
    cco_end(true);
}


// Use coroutine to create a fibonacci sequence generator:

struct fibonacci {
    int count, idx;
    llong result, b;
    int cco_state;
};

bool fibonacci(struct fibonacci* g) {
    assert(g->count < 94);

    cco_begin(g);
        g->idx = 0;
        g->result = 0;
        g->b = 1;
        for (;;) {
            if (g->count-- == 0)
                cco_return;
            if (++g->idx > 1) {
                llong sum = g->result + g->b; // NB! locals lasts only until next cco_yield/cco_await!
                g->result = g->b;
                g->b = sum;
            }
            cco_yield(false);
        }
        cco_final:
            printf("final fib\n");
    cco_end(true);
}

// Combine

struct combined {
    struct prime prm;
    struct fibonacci fib;
    int cco_state;
};


bool combined(struct combined* C) {
    cco_begin(C);
        cco_await_with(prime(&C->prm), false);
        cco_await_with(fibonacci(&C->fib), false);

        // Reuse the C->prm context and extend the count:
        C->prm.count = 8, C->prm.result += 2;
        cco_reset(&C->prm);
        cco_await_with(prime(&C->prm), false);

        cco_final: puts("final comb");
    cco_end(true);
}

int main(void)
{
    struct combined c = {.prm={.count=8}, .fib={14}};

    while (!combined(&c)) {
        printf("Prime(%d)=%lld, Fib(%d)=%lld\n", 
            c.prm.idx, c.prm.result, 
            c.fib.idx, c.fib.result);
    }
}
