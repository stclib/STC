#include <stdio.h>
#include <stc/cbitset.h>

static inline cbitset_t sieveOfEratosthenes(size_t n)
{
    cbitset_t pbits = cbitset_with_size(n + 1, true);
    cbitset_reset(&pbits, 0);
    cbitset_reset(&pbits, 1);

    c_forrange (i, size_t, 2, n+1) {
        // If pbits[i] is not changed, then it is a prime
        if (cbitset_test(pbits, i) && i*i <= n) {
            c_forrange (j, size_t, i*i, n+1, i) {
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

    printf("2 ");
    c_forrange (i, int, 3, 1001, 2) {
       if (cbitset_test(primes, i)) printf("%d ", i);
    }
    puts("");
    cbitset_del(&primes);
}