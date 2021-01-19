#include <stdio.h>
#include <stc/cbits.h>

static inline cbits_t sieveOfEratosthenes(size_t n)
{
    cbits_t pbits = cbits_with_size(n + 1, true);
    cbits_reset(&pbits, 0);
    cbits_reset(&pbits, 1);

    c_forrange (i, size_t, 2, n+1) {
        // If pbits[i] is not changed, then it is a prime
        if (cbits_test(pbits, i) && i*i <= n) {
            c_forrange (j, size_t, i*i, n+1, i) {
                cbits_reset(&pbits, j);
            }
        }
    }
    return pbits;
} 


int main(void)
{
    int n = 100000000;
    printf("computing prime numbers up to %u\n", n);
    
    cbits_t primes = sieveOfEratosthenes(n);
    puts("done");
    
    size_t np = cbits_count(primes);
    printf("number of primes: %zu\n", np);

    printf("2 ");
    c_forrange (i, int, 3, 1001, 2) {
       if (cbits_test(primes, i)) printf("%d ", i);
    }
    puts("");
    cbits_del(&primes);
}