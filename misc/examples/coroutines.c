#include <stc/algo/coroutine.h>
#include <stdio.h>
#include <stdint.h>

// Use coroutine to create a fibonacci sequence generator:

typedef long long intll_t;

struct fibonacci {
    int n; 
    int cco_state;
    intll_t a, b, idx;
};

intll_t fibonacci(struct fibonacci* F) {
    assert (F->n < 95);

    cco_begin(F);
        F->a = 0;
        F->b = 1;
        for (F->idx = 0; F->idx < F->n; F->idx++) {
            cco_yield(F->a);
            intll_t sum = F->a + F->b; // NB! locals only lasts until next cco_yield!
            F->a = F->b;
            F->b = sum;
        }
        cco_final:
    cco_end();

    return -1;
}

// Demonstrate to call another coroutine from a coroutine:
// Create a 2D iterator, and call fibonacci sequence when x,y = 1,1:

struct iterate {
    int max_x, max_y; 
    int cco_state; 
    int x, y;
};

bool iterate(struct iterate* I, struct fibonacci* F) {
    cco_begin(I);
        for (I->x = 0; I->x < I->max_x; I->x++) {
            for (I->y = 0; I->y < I->max_y; I->y++) {
                if (I->x == 1 && I->y == 1)
                    cco_yield_coroutine(F, fibonacci(F), true);
                else
                    cco_yield(true);
            }
        }
        cco_final:
    cco_end();
    return false;
}


int main(void) {
    printf("Fibonacci numbers:\n");
    struct fibonacci fib = {.n = 14};
    struct iterate iter = {3, 3};

    while (iterate(&iter, &fib))
        printf("%d %d. Fib: %lld\n", iter.x, iter.y, fib.a);

    puts("");
}
