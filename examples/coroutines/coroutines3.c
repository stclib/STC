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

int prime(struct prime* g) {
    cco_async (g) {
        if (g->value <= 2) {
            g->value = 2;
            if (g->count-- == 0)
                cco_return;
            cco_yield_v(YIELD_PRM);
        }
        for (g->value |= 1; g->count > 0; g->value += 2) {
            if (is_prime(g->value)) {
                --g->count;
                cco_yield_v(YIELD_PRM);
            }
        }

        cco_drop:
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

int fibonacci(struct fibonacci* g) {
    cco_async (g) {
        assert(g->count < 94);
        if (g->value == 0)
            g->b = 1;

        while (true) {
            if (g->count-- == 0)
                cco_return;
            // NB! locals lasts only until next yield/await!
            long long tmp = g->value;
            g->value = g->b;
            g->b += tmp;
            cco_yield_v(YIELD_FIB);
        }

        cco_drop:
        puts("DONE fib");
    }
    return 0;
}

// Combine

cco_task_struct (combined) {
    combined_base base;
    struct prime prm;
    struct fibonacci fib;
};

int combined(struct combined* g) {
    cco_async (g) {
        puts("SERIAL:");
        g->prm = (struct prime){.base={prime}, .count=8};
        g->fib = (struct fibonacci){.base={fibonacci}, .count=12};

        cco_await_task(&g->prm);
        cco_await_task(&g->fib);

        puts("\nCONCURRENT:");
        g->prm = (struct prime){.base={prime}, .count=8};
        g->fib = (struct fibonacci){.base={fibonacci}, .count=12};
        cco_spawn(&g->prm);
        cco_spawn(&g->fib);

        cco_await(cco_joined());

        cco_drop:
        puts("DONE prime and fib");
    }
    return 0;
}


int main(void) {
    struct combined comb = {{combined}};

    cco_run_task(it, &comb, NULL) {
        if (it->result & YIELD_PRM)
            printf("  Prime=%lld\n", comb.prm.value);
        if (it->result & YIELD_FIB)
            printf("  Fibon=%lld\n", comb.fib.value);
    }
}
