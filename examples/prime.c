#include <stdio.h>
#include <stc/cbitset.h>

static inline cbitset_t sieveOfEratosthenes(size_t n)
{
    cbitset_t pbits = cbitset_with_size(n + 1, true);
    cbitset_reset(&pbits, 0);
    cbitset_reset(&pbits, 1);

    for (size_t i = 2; i <= n; ++i) {
        // If pbits[i] is not changed, then it is a prime
        if (cbitset_test(pbits, i) && i*i <= n) {
            for (size_t j = i*i; j <= n; j += i) {
                cbitset_reset(&pbits, j);
            }
        }
    }
    return pbits;
} 


int main(void)
{
    int n = 100000000;
    printf("computing prime numbers up to %u\n", n);
    
    cbitset_t primes = sieveOfEratosthenes(n);
    puts("done");
    
    size_t np = cbitset_count(primes);
    printf("number of primes: %zu\n", np);

    for (uint32_t i = 2; i <= 1000; ++i)
       if (cbitset_test(primes, i)) printf("%u ", i);
    puts("");
    cbitset_destroy(&primes);
}