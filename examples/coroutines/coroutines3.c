#include <stc/coroutine.h>
#include <stdio.h>

// Demonstrate calling two coroutine from a coroutine:
// First call them concurrently, then in parallel:


// Suspend yield values
enum { YIELD_PRM = 1<<0, YIELD_FIB = 1<<1};

bool is_prime(long long i) {
    for (long long j=2; j*j <= i; ++j)
        if (i % j == 0) return false;
    return true;
}

cco_task_struct (prime, long long*) {
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
            *cco_env(o) = o->value;
            cco_yield_v(YIELD_PRM);
        }
        for (o->value |= 1; o->count > 0; o->value += 2) {
            if (is_prime(o->value)) {
                --o->count;
                *cco_env(o) = o->value;
                cco_yield_v(YIELD_PRM);
            }
        }

        cco_finalize:
        puts("DONE prm");
    }
    return 0;
}


// Use coroutine to create a fibonacci sequence generator:

cco_task_struct (fibonacci, long long*) {
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
            *cco_env(o) = o->value;
            cco_yield_v(YIELD_FIB);
        }

        cco_finalize:
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

int combined(struct combined* o) {
    cco_async (o) {
        puts("SEQUENTIAL:");
        o->prm = (struct prime){.base={prime}, .count=8};
        o->fib = (struct fibonacci){.base={fibonacci}, .count=12};

        cco_await_task(&o->prm);
        cco_await_task(&o->fib);

        puts("\nCONCURRENT:");
        o->prm = (struct prime){.base={prime}, .count=8};
        o->fib = (struct fibonacci){.base={fibonacci}, .count=12};

        cco_spawn(&o->prm, cco_grp(0));
        cco_spawn(&o->fib, cco_grp(0));
        cco_await_all(cco_grp(0));

        cco_finalize:
        puts("DONE prime and fib");
    }
    return 0;
}


int main(void) {
    struct combined comb = {{combined}};

    long long result;
    cco_run_task(it, &comb, &result) {
        switch (it->status) {
            case YIELD_PRM: printf("  Prime=%lld\n", result); break;
            case YIELD_FIB: printf("  Fibon=%lld\n", result); break;
        }
    }
}
