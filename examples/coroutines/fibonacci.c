#include <stdio.h>
#include <stc/coroutine.h>

// Use coroutine to create a fibonacci sequence generator:

cco_task_struct (fibonacci) {
    fibonacci_base base;
    unsigned long long value, b;
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

        cco_finalize:
        puts("done");
    }
    return 0;
}


int main(void) {
    struct fibonacci fib = {{fibonacci}};
    int n = 0;

    //cco_run_task(&fib) {
    while (cco_resume(&fib)) {
        printf("Fib(%d) = %llu\n", n++, fib.value);

        if (fib.value > fib.b)
            cco_stop(&fib);
    }
}
