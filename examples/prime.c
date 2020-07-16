#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stc/cvec.h"

static inline void setBit(uint32_t* a, const size_t i) { a[i >> 5] |= 1u << (i & 31); }
static inline void clearBit(uint32_t* a, const size_t i) { a[i >> 5] &= ~(1u << (i & 31)); }
static inline bool testBit(uint32_t* a, const size_t i) { return (a[i >> 5] & (1u << (i & 31))) != 0; }

declare_CVec(i, uint32_t);

static inline void sieveOfEratosthenes(size_t n)
{
    CVec_i prime = cvec_i_make((n >> 5) + 1, 0xffffffff);

    printf("n: %zu, ints: %zu\n", n, cvec_size(prime));
    clearBit(prime.data, 0);
    clearBit(prime.data, 1);

    for (size_t i = 2; i <= n; ++i)
    {
        // If prime[i] is not changed, then it is a prime
        if (testBit(prime.data, i) && i*i <= n)
        {
            for (size_t j = i*i; j <= n; j += i) {
                clearBit(prime.data, j);
            }
        }
    }
    puts("done");
    // Print all prime numbers
    size_t count = 0;
    for (size_t i = 1; i <= n; ++i)
       if (testBit(prime.data, i)) ++count; // printf("%zu\n", i);

    printf("primes: %zu\n", count);
    cvec_i_destroy(&prime);
} 

int main(void)
{
    sieveOfEratosthenes(1000000000);
}