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
    int64_t num, result;
    int cco_state;
};

bool prime(struct prime* U) {
    cco_begin(U);
        if (U->num < 2) U->num = 2;
        if (U->num == 2) {
            ++U->idx;
            U->result = U->num;
            cco_yield(true);
        }
        U->num += !(U->num & 1);
        for (; U->idx < U->count; U->num += 2)
            if (is_prime(U->num)) {
                ++U->idx;
                U->result = U->num;
                cco_yield(true);
            }
        cco_final:
    cco_end(false);
}


// Use coroutine to create a fibonacci sequence generator:

struct fibonacci {
    int count, idx;
    int64_t result, b;
    int cco_state;
};

int64_t fibonacci(struct fibonacci* F) {
    assert (F->count < 94);

    cco_begin(F);
        F->idx = 0;
        F->result = 0;
        F->b = 1;
        for (F->idx = 1; F->idx < F->count; F->idx++) {
            cco_yield(F->result);
            int64_t sum = F->result + F->b; // NB! locals only lasts until next cco_yield!
            F->result = F->b;
            F->b = sum;
        }
        cco_final:
    cco_end(-1);
}

// Combine

struct combine {
    struct prime prm;
    struct fibonacci fib;
    int cco_state;
};

bool combine(struct combine* C) {
    cco_begin(C);
        cco_coroutine(prime(&C->prm), &C->prm, true);
        cco_coroutine(fibonacci(&C->fib), &C->fib, true);

        // Reuse the C->prm context and extend the count:
        C->prm.count = 20;
        cco_coroutine(prime(&C->prm), &C->prm, true);

        cco_final: puts("final");
    cco_end(false);
}

int main(void) {
    struct combine comb = {.prm={.count=10}, .fib={14}};
    while (combine(&comb))
        if (cco_done(&comb.prm))
            cco_reset(&comb.prm);
        else
            printf("Prime(%d)=%lld, Fib(%d)=%lld\n", 
                comb.prm.idx, (long long)comb.prm.result, 
                comb.fib.idx, (long long)comb.fib.result);
}
