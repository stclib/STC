#include <stc/algo/coroutine.h>
#include <stdio.h>
#include <stdint.h>

// Demonstrate to call another coroutine from a coroutine:
// First create prime generator, then call fibonacci sequence:

bool is_prime(int64_t i) {
    for (int64_t j=2; j*j <= i; ++j)
        if (i % j == 0) return false;
    return true;
}

struct prime {
    int count, idx;
    int64_t result, pos;
    int cco_state;
};

bool prime(struct prime* U) {
    cco_begin(U);
        if (U->result < 2) U->result = 2;
        if (U->result == 2) {
            if (U->count-- == 0) cco_return;
            ++U->idx;
            cco_yield(true);
        }
        U->result += !(U->result & 1);
        for (U->pos = U->result; U->count > 0; U->pos += 2) {
            if (is_prime(U->pos)) {
                --U->count;
                ++U->idx;
                U->result = U->pos;
                cco_yield(true);
            }
        }
        cco_final:
            printf("final prm\n");
    cco_end(false);
}


// Use coroutine to create a fibonacci sequence generator:

struct fibonacci {
    int count, idx;
    int64_t result, b;
    int cco_state;
};

bool fibonacci(struct fibonacci* F) {
    assert(F->count < 94);

    cco_begin(F);
        F->idx = 0;
        F->result = 0;
        F->b = 1;
        for (;;) {
            if (F->count-- == 0)
                cco_return;
            if (++F->idx > 1) {
                int64_t sum = F->result + F->b; // NB! locals only lasts until next cco_yield!
                F->result = F->b;
                F->b = sum;
            }
            cco_yield(true);
        }
        cco_final:
            printf("final fib\n");
    cco_end(false);
}

// Combine

struct combined {
    struct prime prm;
    struct fibonacci fib;
    int cco_state;
};

bool combined(struct combined* C) {
    cco_begin(C);
        cco_yield(prime(&C->prm), &C->prm, true);
        cco_yield(fibonacci(&C->fib), &C->fib, true);

        // Reuse the C->prm context and extend the count:
        C->prm.count = 8; C->prm.result += 2;
        cco_reset(&C->prm);
        cco_yield(prime(&C->prm), &C->prm, true);

        cco_final: puts("final comb");
    cco_end(false);
}

int main(void) {
    struct combined comb = {.prm={.count=8}, .fib={14}};
    if (true)
        while (combined(&comb))
            printf("Prime(%d)=%lld, Fib(%d)=%lld\n", 
                comb.prm.idx, (long long)comb.prm.result, 
                comb.fib.idx, (long long)comb.fib.result);
    else
        while (prime(&comb.prm))
            printf("Prime(%d)=%lld\n", 
                comb.prm.idx, (long long)comb.prm.result);
}
