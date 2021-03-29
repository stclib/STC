#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stc/cbits.h>

cbits sieveOfEratosthenes(size_t n)
{
    cbits bits = cbits_with_pattern(n, 0xAAAAAAAAAAAAAAAA);
    size_t q = sqrt(n);
    cbits_reset(&bits, 1);
    cbits_set(&bits, 2);

    for (size_t j, i = 3; i <= q; i += 2) {
        for (j = i; j < n; j += 2) {
            if (cbits_test(bits, j)) {
                i = j;
                break;
            }
        }
        for (j = i*i; j < n; j += i*2)
            cbits_reset(&bits, j);
    }
    return bits;
}


int main(void)
{
    size_t n = 100000000;
    printf("computing prime numbers up to %u\n", n);

    clock_t t1 = clock();
    cbits primes = sieveOfEratosthenes(n + 1);
    size_t np = cbits_count(primes);
    clock_t t2 = clock();

    printf("number of primes: %zu, time: %f\n", np, (t2 - t1)/(float)CLOCKS_PER_SEC);

    for (size_t i=0; i<=1000; ++i)
       if (cbits_test(primes, i)) printf("%d ", i);
    puts("");

    cbits_del(&primes);
}