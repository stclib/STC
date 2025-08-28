#include <stc/coroutine.h>
#include <stdio.h>
#include <stdint.h>

// Demonstrate calling two coroutine from a coroutine:
// First call them concurrently, then in parallel:


// Suspend yield values
enum { YIELD_PRM = 1<<0, YIELD_FIB = 1<<1};

bool is_prime(long long i) {
    for (long long j=2; j*j <= i; ++j)
        if (i % j == 0) return false;
    return true;
}

cco_task_struct (prime) {
    prime_base base;
    int count;
    long long value;
};

int prime(struct prime* o) {
    cco_async (o) {
        if (o->value <= 2) {
            o->value = 2;
            if (o->count-- == 0)
                cco_return;
            cco_yield_v(YIELD_PRM);
        }
        for (o->value |= 1; o->count > 0; o->value += 2) {
            if (is_prime(o->value)) {
                --o->count;
                cco_yield_v(YIELD_PRM);
            }
        }

        cco_finalize:
        puts("DONE prm");
    }
    return 0;
}


// Use coroutine to create a fibonacci sequence generator:

cco_task_struct (fibonacci) {
    fibonacci_base base;
    int count;
    long long value, b;
};

int fibonacci(struct fibonacci* o) {
    cco_async (o) {
        assert(o->count < 94);
        if (o->value == 0)
            o->b = 1;

        while (true) {
            if (o->count-- == 0)
                cco_return;
            // NB! locals lasts only until next yield/await!
            long long tmp = o->value;
            o->value = o->b;
            o->b += tmp;
            cco_yield_v(YIELD_FIB);
        }

        cco_finalize:
        puts("DONE fib");
    }
    return 0;
}


int main(void) {
    struct prime prm;
    struct fibonacci fib;

    puts("SERIAL:");
    prm = (struct prime){{prime}, .count=8};
    fib = (struct fibonacci){{fibonacci}, .count=12};

    cco_run_task(&prm) {
        printf("  Prime=%lld\n", prm.value);
    }
    cco_run_task(&fib) {
        printf("  Fibon=%lld\n", fib.value);
    }


    puts("\nCONCURRENT:");
    prm = (struct prime){{prime}, .count=8};
    fib = (struct fibonacci){{fibonacci}, .count=12};

    cco_fiber* fb = c_new(cco_fiber, 0);
    cco_spawn(&prm, NULL, NULL, fb);
    cco_spawn(&fib, NULL, NULL, fb);

    cco_run_fiber(&fb) {
        switch (fb->status) {
            case YIELD_PRM:
                printf("  Prime=%lld\n", prm.value);
                break;
            case YIELD_FIB:
                printf("  Fibon=%lld\n", fib.value);
                break;
        }
    }
    puts("DONE prime and fib");
}
