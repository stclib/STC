#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stc/cbitset.h>

static inline void sieveOfEratosthenes(size_t n)
{
    CBitset prime = cbitset_make(n + 1, true);
    printf("computing primes up to %zu\n", n);
    cbitset_reset(&prime, 0);
    cbitset_reset(&prime, 1);

    for (size_t i = 2; i <= n; ++i) {
        // If prime[i] is not changed, then it is a prime
        if (cbitset_test(prime, i) && i*i <= n) {
            for (size_t j = i*i; j <= n; j += i) {
                cbitset_reset(&prime, j);
            }
        }
    }
    puts("done");
    // Print all prime numbers
    size_t count = 0;
    for (size_t i = 1; i <= n; ++i)
       if (cbitset_test(prime, i)) ++count; // printf("%zu\n", i);

    printf("number of primes: %zu\n", count);
    cbitset_destroy(&prime);
} 

int main(void)
{
    sieveOfEratosthenes(1000000000);
}