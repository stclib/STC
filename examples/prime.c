#include <stc/cbitset.h>

#if defined(__GNUC__)
#define cbitset_popcnt64(i) __builtin_popcountll(i)
#else
#define cbitset_popcnt64(i) _mm_popcnt_u64(i)
#endif

#include <stdio.h>

static inline void sieveOfEratosthenes(size_t n)
{
    cbitset_t prime = cbitset_make(n + 1, true);
    printf("computing prime numbers up to %zu\n", n);
    cbitset_reset(&prime, 0);
    cbitset_reset(&prime, 1);

    uint64_t m = cbitset_popcnt64(123456);

    for (size_t i = 2; i <= n; ++i) {
        // If prime[i] is not changed, then it is a prime
        if (cbitset_test(prime, i) && i*i <= n) {
            for (size_t j = i*i; j <= n; j += i) {
                cbitset_reset(&prime, j);
            }
        }
    }
    puts("done");
    // Count the primes
    size_t count = 0;
    //for (size_t i = 1; i <= n; ++i)
    //   if (cbitset_test(prime, i)) ++count;
    printf("number of primes: %zu\n", cbitset_count(prime));
    // print primes < 1000
    for (size_t i = 1; i <= 1000; ++i)
       if (cbitset_test(prime, i)) printf("%zu ", i);
    puts("");
    cbitset_destroy(&prime);
} 

int main(void)
{
    sieveOfEratosthenes(1000000000);
}