#include <stc/algo/coroutine.h>
#include <stdio.h>
#include <stdint.h>

// Demonstrate to call another coroutine from a coroutine:
// First create a 2D iterator, then call fibonacci sequence:

struct iterate {
    int max_x, max_y;
    int cco_state;
    int x, y;
};

bool iterate(struct iterate* I) {
    cco_begin(I);
        for (I->x = 0; I->x < I->max_x; I->x++)
            for (I->y = 0; I->y < I->max_y; I->y++)
                cco_yield(true);
        cco_final:
    cco_end();
    return false;
}

// Use coroutine to create a fibonacci sequence generator:

struct fibonacci {
    int n; 
    int cco_state;
    int64_t a, b, idx;
};

int64_t fibonacci(struct fibonacci* F) {
    assert (F->n < 95);

    cco_begin(F);
        F->a = 0;
        F->b = 1;
        for (F->idx = 0; F->idx < F->n; F->idx++) {
            cco_yield(F->a);
            int64_t sum = F->a + F->b; // NB! locals only lasts until next cco_yield!
            F->a = F->b;
            F->b = sum;
        }
        cco_final:
    cco_end();

    return -1;
}

// Combine

struct combine {
    struct iterate it;
    struct fibonacci fib;
    int cco_state;
};

bool combine(struct combine* C) {
    cco_begin(C);
        cco_yield_coroutine(&C->it, iterate(&C->it), true);
        cco_yield_coroutine(&C->fib, fibonacci(&C->fib), true);
        // May reuse the C->it context; state has been reset to 0.
        cco_yield_coroutine(&C->it, iterate(&C->it), true);
        cco_final: puts("final");
    cco_end();
    return false;
}

int main(void) {
    struct combine comb = {.it={3, 3}, .fib={14}};
    while (combine(&comb))
        printf("Iter=(%d, %d). Fib=%lld\n", comb.it.x, comb.it.y, (long long)comb.fib.a);
}
