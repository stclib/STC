#include <stc/algo/cco.h>
#include <stdio.h>
#include <stdint.h>

// Use coroutine to create a fibonacci sequence generator:

typedef long long llong;

llong fibonacci_sequence(ccontext* ctx, unsigned n) {
    assert (n < 95);

    cco_context(ctx, 
        llong a, b, idx;
    );

    cco_routine(u,
        u->a = 0;
        u->b = 1;
        for (u->idx = 2; u->idx < n; u->idx++) {
            llong sum = u->a + u->b;
            cco_yield(sum);
            u->a = u->b;
            u->b = sum;
        }
        cco_finish:
    );
    return 0;
}


int main(void) {
    ccontext z = 0;
    printf("Fibonacci numbers:\n");
    for (;;) {
       llong x = fibonacci_sequence(&z, 30);
       if (!z) break;
       printf(" %lld", x);
    }
    puts("");
}
