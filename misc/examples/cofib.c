#include <stc/algo/coroutine.h>
#include <stdio.h>
#include <stdint.h>

// Use coroutine to create a fibonacci sequence generator:

typedef long long llong;

llong fibonacci_sequence(cco_handle* z, unsigned n) {
    assert (n < 95);

    cco_context(z,
        llong a, b, idx;
    );
    cco_routine(U,
        U->a = 0;
        U->b = 1;
        for (U->idx = 0; U->idx < n; U->idx++) {
            cco_yield (U->a);
            llong sum = U->a + U->b; // NB! locals only lasts until next cco_yield!
            U->a = U->b;
            U->b = sum;
        }
        cco_finish:
    );

    return -1;
}


int main(void) {
    cco_handle z = 0;
    printf("Fibonacci numbers:\n");
    for (;;) {
       llong x = fibonacci_sequence(&z, 14);
       if (!z) break;
       printf(" %lld", x);
    }
    puts("");
}
