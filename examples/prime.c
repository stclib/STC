#include <stdio.h>
#include <stc/cbitset.h>
#include <stc/cvec.h>

declare_cvec(ux, uint64_t);

static inline cvec_ux sieveOfEratosthenes(size_t n)
{
    cbitset_t pbits = cbitset_make(n + 1, true);
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
    puts("count:");
    size_t np = cbitset_count(pbits);
    puts("done");
    cvec_ux primes = cvec_init;
    cvec_ux_reserve(&primes, np);
    for (uint32_t i = 2; i <= n; ++i)
       if (cbitset_test(pbits, i)) cvec_ux_push_back(&primes, i);
    
    cbitset_destroy(&pbits);
    return primes;
} 

int main(void)
{
    int n = 1000000000;
    printf("computing prime numbers up to %u\n", n);
    
    cvec_ux primes = sieveOfEratosthenes(n);
    printf("number of primes: %zu\n", cvec_size(primes));
    for (size_t i = 0; i < 100; ++i)
       printf("%zu ", primes.data[i]);

    cvec_ux_destroy(&primes);
}