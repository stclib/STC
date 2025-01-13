#include <stdio.h>
#include <math.h>
#include <time.h>
#include "stc/cbits.h"
#include "stc/algorithm.h"

cbits sieveOfEratosthenes(isize n)
{
    cbits bits = cbits_with_size(n/2 + 1, true);
    isize q = (isize)sqrt((double) n) + 1;
    for (isize i = 3; i < q; i += 2) {
        for (isize j = i; j < n; j += 2) {
            if (cbits_test(&bits, j/2)) {
                i = j;
                break;
            }
        }
        for (isize j = i*i; j < n; j += i*2)
            cbits_reset(&bits, j/2);
    }
    return bits;
}

int main(void)
{
    int n = 100000000;
    printf("Computing prime numbers up to %d\n", n);

    clock_t t = clock();
    cbits primes = sieveOfEratosthenes(n + 1);

    int np = (int)cbits_count(&primes);
    t = clock() - t;

    printf("Number of primes: %d, time: %f\n\n", np, (double)t/CLOCKS_PER_SEC);

    puts("Show all the primes in the range [2, 1000):");
    printf("2");
    for (c_range(i, 3, 1000, 2))
        if (cbits_test(&primes, i/2)) printf(" %d", (int)i);
    puts("\n");

    puts("Show the last 50 primes using a temporary crange generator, 10 per line:");

    c_filter(crange, c_iota(n - 1, 1, -2), true
        && cbits_test(&primes, *value/2)
        && printf("%d ", (int) *value)
        && c_flt_take(50)
        && c_flt_getcount() % 10 == 0
        && printf("\n")
    );
    cbits_drop(&primes);
}
