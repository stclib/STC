#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stc/cbits.h>
#include <stc/algo/filter.h>
#include <stc/algo/crange.h>


cbits sieveOfEratosthenes(int64_t n)
{
    cbits bits = cbits_with_size(n/2 + 1, true);
    int64_t q = (int64_t)sqrt((double) n) + 1;
    for (int64_t i = 3; i < q; i += 2) {
        int64_t j = i;
        for (; j < n; j += 2) {
            if (cbits_test(&bits, j>>1)) {
                i = j;
                break;
            }
        }
        for (int64_t j = i*i; j < n; j += i*2)
            cbits_reset(&bits, j>>1);
    }
    return bits;
}

int main(void)
{
    int n = 1000000000;
    printf("Computing prime numbers up to %d\n", n);

    clock_t t = clock();
    cbits primes = sieveOfEratosthenes(n + 1);
    int np = (int)cbits_count(&primes);
    t = clock() - t;

    puts("Show all the primes in the range [2, 1000):");
    printf("2");
    c_forrange (i, 3, 1000, 2)
        if (cbits_test(&primes, i>>1)) printf(" %lld", i);
    puts("\n");

    puts("Show the last 50 primes using a temporary crange generator:");
    crange range = crange_init(n - 1, 0, -2);

    c_forfilter (i, crange, range,
        cbits_test(&primes, *i.ref/2) &&
        c_flt_take(i, 50)
    ){
        printf("%lld ", *i.ref);
        if (c_flt_getcount(i) % 10 == 0) puts("");
    }
    printf("Number of primes: %d, time: %.2f\n\n", np, (double)t/CLOCKS_PER_SEC);

    cbits_drop(&primes);
}
