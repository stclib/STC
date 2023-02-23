#include <stc/algo/ccoro.h>
#include <stdio.h>
#include <stdint.h>

// Use coroutine to create a fibonacci sequence generator:

typedef long long llong;
struct fibonacci {
    int n; 
    int ccoro_state;
    llong a, b, idx;
};

llong fibonacci(struct fibonacci* U) {
    assert (U->n < 95);

    ccoro_execute(U,
        U->a = 0;
        U->b = 1;
        for (U->idx = 0; U->idx < U->n; U->idx++) {
            ccoro_yield (U->a);
            llong sum = U->a + U->b; // NB! locals only lasts until next ccoro_yield!
            U->a = U->b;
            U->b = sum;
        }
        ccoro_final:
    );

    return -1;
}


int main(void) {
    printf("Fibonacci numbers:\n");
    struct fibonacci fib = {.n = 14};

    for (;;) {
       llong x = fibonacci(&fib);
       if (!ccoro_alive(&fib)) break;
       printf(" %lld", x);
    }
    puts("");
}
