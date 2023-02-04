#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stc/cbits.h>
#include <stc/algo/filter.h>
#include <stc/algo/crange.h>


cbits sieveOfEratosthenes(intptr_t n)
{
    cbits bits = cbits_with_size(n/2 + 1, true);
    intptr_t q = (intptr_t)sqrt((double) n) + 1;
    for (intptr_t i = 3; i < q; i += 2) {
        intptr_t j = i;
        for (; j < n; j += 2) {
            if (cbits_test(&bits, j>>1)) {
                i = j;
                break;
            }
        }
        for (intptr_t j = i*i; j < n; j += i*2)
            cbits_reset(&bits, j>>1);
    }
    return bits;
}

int main(void)
{
    intptr_t n = 1000000000;
    printf("computing prime numbers up to %" c_ZI "\n", n);

    clock_t t1 = clock();
    c_WITH (cbits primes = sieveOfEratosthenes(n + 1), cbits_drop(&primes)) {
        puts("done");
        intptr_t np = cbits_count(&primes);
        clock_t t2 = clock();

        printf("number of primes: %" c_ZI ", time: %f\n", np, (float)(t2 - t1) / (float)CLOCKS_PER_SEC);
        puts("Show all the primes in the range [2, 1000):");
        printf("2");
        c_FORRANGE (i, 3, 1000, 2)
            if (cbits_test(&primes, i>>1)) printf(" %lld", i);
        puts("");

        puts("Show the last 50 primes using a temporary crange generator:");
        crange R = crange_make(n - 1, 0, -2);
        c_FORFILTER (i, crange, R
                      , cbits_test(&primes, *i.ref>>1)
                      , c_flt_take(i, 50)) {
            printf("%lld ", *i.ref);
            if (i.count % 10 == 0) puts("");
        }
    }
}
