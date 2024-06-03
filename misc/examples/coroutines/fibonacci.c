#include <stdio.h>
#include "stc/coroutine.h"

// Use coroutine to create a fibonacci sequence generator:

struct fibonacci {
    unsigned long long value, b;
    int cco_state;
};

int fibonacci(struct fibonacci* g) {
    cco_scope (g) {
        g->value = 0;
        g->b = 1;
        cco_yield;

        while (true) {
            unsigned long long tmp = g->value;
            g->value = g->b;
            g->b += tmp;
            cco_yield;
        }

        cco_final:
        puts("done");
    }
    return 0;
}


int main(void) {
    struct fibonacci fib = {0};
    int n = 0;

    cco_run_coroutine( fibonacci(&fib) ) {
        printf("Fib(%d) = %llu\n", n++, fib.value);

        if (fib.value > fib.b)
            cco_stop(&fib);
    }
}
