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
    int64_t n = 1000000000;
    printf("Computing prime numbers up to %" c_ZI "\n", n);

    clock_t t1 = clock();
    cbits primes = sieveOfEratosthenes(n + 1);
    int64_t np = cbits_count(&primes);
    clock_t t2 = clock();

    printf("Number of primes: %" c_ZI ", time: %f\n\n", np, (float)(t2 - t1) / (float)CLOCKS_PER_SEC);
    puts("Show all the primes in the range [2, 1000):");
    printf("2");
    c_forrange (i, 3, 1000, 2)
        if (cbits_test(&primes, i>>1)) printf(" %lld", i);
    puts("\n");

    puts("Show the last 50 primes using a temporary crange generator:");
    c_forfilter (i, crange, crange_obj(n - 1, 0, -2),
        cbits_test(&primes, *i.ref/2) &&
        c_flt_take(i, 50)
    ){
        printf("%lld ", *i.ref);
        if (c_flt_getcount(i) % 10 == 0) puts("");
    }

    cbits_drop(&primes);
}
