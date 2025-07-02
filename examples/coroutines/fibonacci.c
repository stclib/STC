#include <stdio.h>
#include <stc/coroutine.h>

// Use coroutine to create a fibonacci sequence generator:

struct fibonacci {
    unsigned long long value, b;
    cco_base base;
};

int fibonacci(struct fibonacci* o) {
    unsigned long long tmp;
    cco_async (o) {
        o->value = 0;
        o->b = 1;
        cco_yield;

        while (true) {
            tmp = o->value;
            o->value = o->b;
            o->b += tmp;
            cco_yield;
        }

        cco_drop:
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
