#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stc/cbits.h>

cbits sieveOfEratosthenes(size_t n)
{
    cbits bits = cbits_with_size(n/2 + 1, true);
    size_t q = (size_t) sqrt((double) n) + 1;
    for (size_t i = 3; i < q; i += 2) {
        size_t j = i;
        for (; j < n; j += 2) {
            if (cbits_test(bits, j>>1)) {
                i = j;
                break;
            }
        }
        for (size_t j = i*i; j < n; j += i*2)
            cbits_reset(&bits, j>>1);
    }
    return bits;
}

int main(void)
{
    size_t n = 1000000000;
    printf("computing prime numbers up to %" PRIuMAX "\n", n);

    clock_t t1 = clock();
    c_autovar (cbits primes = sieveOfEratosthenes(n + 1), cbits_drop(&primes)) {
        puts("done");
        size_t np = cbits_count(primes);
        clock_t t2 = clock();

        printf("number of primes: %" PRIuMAX ", time: %f\n", np, (t2 - t1) / (float)CLOCKS_PER_SEC);
        printf("2");
        for (size_t i = 3; i < 1000; i += 2)
            if (cbits_test(primes, i>>1)) printf(" %" PRIuMAX "", i);
        puts("");
    }
}
