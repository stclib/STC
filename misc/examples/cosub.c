#include <stc/algo/ccoro.h>
#include <stdio.h>
#include <stdint.h>

// Use coroutine to create a fibonacci sequence generator:

typedef long long intll_t;

struct fibonacci {
    int n; 
    int ccoro_state;
    intll_t a, b, idx;
};

intll_t fibonacci(struct fibonacci* F) {
    assert (F->n < 95);

    ccoro_execute(F,
        F->a = 0;
        F->b = 1;
        for (F->idx = 0; F->idx < F->n; F->idx++) {
            ccoro_yield (F->a);
            intll_t sum = F->a + F->b; // NB! locals only lasts until next ccoro_yield!
            F->a = F->b;
            F->b = sum;
        }
        ccoro_final:
    );

    return -1;
}

// Demonstrate to call another coroutine from a coroutine:
// Create a 2D iterator, and call fibonacci sequence when x,y = 1,1:

struct iterate {
    int max_x, max_y; 
    int ccoro_state; 
    int x, y;
};

bool iterate(struct iterate* I, struct fibonacci* F) {
    ccoro_execute(I,
        for (I->x = 0; I->x < I->max_x; I->x++)
            for (I->y = 0; I->y < I->max_y; I->y++)
                if (I->x == 1 && I->y == 1)
                    ccoro_yield_call (F, fibonacci(F), true);
                else
                    ccoro_yield (true);
        ccoro_final:
    );
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
