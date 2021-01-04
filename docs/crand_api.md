# STC Module [crand](../stc/crand.h): Pseudo Random Number Generators
![Random](pics/random.jpg)

This describes the API of module **crand**. It contains **stc64**, a *64-bit PRNG*, and can generate
bounded uniform and normal distributed random numbers. See [random](https://en.cppreference.com/w/cpp/header/random)
for similar c++ functionality.

**stc64** is an extremely fast PRNG by Tyge Løvset, suited for parallel usage. It features a
Weyl-sequence as part of the state. It is faster than *sfc64*, *wyhash64*, *pcg64*, and *xoshiro256\*\**
on common platforms. It does not require fast multiplication or 128-bit integer operations. It has a
256 bit state, but updates only 192 bit per generated number.

There is no *jump function*, but by incrementing the Weyl-increment by 2, it starts a new
unique 2^64 *minimum* length period. Note that for each Weyl-increment (state[3]), the period
length is about 2^126 with a high probability. For a single thread, a minimum period of 2^127
is generated when the Weyl-increment is incremented by 2 every 2^64 output.

**stc64** passes *PractRand*, tested up to 8TB output, Vigna's Hamming weight test, and simple
correlation tests, i.e. *n* interleaved streams with only one-bit differences in initial state.

## Types

| Name               | Type definition                           | Used to represent...         |
|:-------------------|:------------------------------------------|:-----------------------------|
| `stc64_t`          | `struct {uint64_t state[4];}`             | The PRNG engine type         |
| `stc64_uniform_t`  | `struct {int64_t lower; uint64_t range;}` | Integer uniform distribution |
| `stc64_uniformf_t` | `struct {double lower, range;}`           | Real number uniform distr.   |
| `stc64_normalf_t`  | `struct {double mean, stddev;}`           | Normal distribution type     |

## Header file

All cstr definitions and prototypes may be included in your C source file by including a single header file.
```c
#include "stc/crand.h"
```

## Methods

```c
 1)     stc64_t             stc64_init(uint64_t seed);
 2)     stc64_t             stc64_with_seq(uint64_t seed, uint64_t seq);
 3)     uint64_t            stc64_rand(stc64_t* rng);
 4)     double              stc64_randf(stc64_t* rng);
 5)     stc64_uniform_t     stc64_uniform_init(int64_t low, int64_t high);
 6)     int64_t             stc64_uniform(stc64_t* rng, stc64_uniform_t* dist);
 7)     stc64_uniformf_t    stc64_uniformf_init(double low, double high);
 8)     double              stc64_uniformf(stc64_t* rng, stc64_uniformf_t* dist);
 9)     stc64_normalf_t     stc64_normalf_init(double mean, double stddev);
10)     double              stc64_normalf(stc64_t* rng, stc64_normalf_t* dist);
```
`1-2)` PRNG 64-bit engine initializers. `3)` Integer generator, range \[0, 2^64).
`4)` Double RNG with range \[0, 1). `5-6)` Uniform integer RNG with range \[*low*, *high*].
`7-8)` Uniform double RNG with range \[*low*, *high*). `9-10)` Normal-distributed double
RNG, around 68% of the values fall within the range [*mean* - *stddev*, *mean* + *stddev*].

## Example
```c
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "stc/crand.h"
#include "stc/cstr.h"
#include "stc/cmap.h"
#include "stc/cvec.h"

// Declare unordered map: int -> int with typetag 'i'.
using_cmap(i, int, size_t);

// Comparison of map keys.
static int compare(cmap_i_entry_t *a, cmap_i_entry_t *b) {
    return c_default_compare(&a->first, &b->first);
}
// Declare vector of map entries, with comparison function.
using_cvec(e, cmap_i_entry_t, c_default_del, compare);


int main()
{
    enum {N = 10000000};
    const double Mean = -12.0, StdDev = 6.0, Scale = 74;

    printf("Demo of gaussian / normal distribution of %d random samples\n", N);

    // Setup random engine with normal distribution.
    uint64_t seed = time(NULL);
    stc64_t rng = stc64_init(seed);
    stc64_normalf_t dist = stc64_normalf_init(Mean, StdDev);

    // Create histogram map
    cmap_i mhist = cmap_i_init();
    for (size_t i = 0; i < N; ++i) {
        int index = (int) round( stc64_normalf(&rng, &dist) );
        cmap_i_emplace(&mhist, index, 0).first->second += 1;
    }

    // Transfer map to vec and sort it by map entry keys.
    cvec_e vhist = cvec_e_init();
    c_foreach (i, cmap_i, mhist)
        cvec_e_push_back(&vhist, *i.ref);
    cvec_e_sort(&vhist);

    // Print the gaussian bar chart
    cstr_t bar = cstr_init();
    c_foreach (i, cvec_e, vhist) {
        size_t n = (size_t) (i.ref->second * StdDev * Scale * 2.5 / N);
        if (n > 0) {
            cstr_resize(&bar, n, '*');
            printf("%4d %s\n", i.ref->first, bar.str);
        }
    }

    // Cleanup
    cstr_del(&bar);
    cmap_i_del(&mhist);
    cvec_e_del(&vhist);
}
```
Output:
```
Demo of gaussian / normal distribution of 10000000 random samples
 -29 *
 -28 **
 -27 ***
 -26 ****
 -25 *******
 -24 *********
 -23 *************
 -22 ******************
 -21 ***********************
 -20 ******************************
 -19 *************************************
 -18 ********************************************
 -17 ****************************************************
 -16 ***********************************************************
 -15 *****************************************************************
 -14 *********************************************************************
 -13 ************************************************************************
 -12 *************************************************************************
 -11 ************************************************************************
 -10 *********************************************************************
  -9 *****************************************************************
  -8 ***********************************************************
  -7 ****************************************************
  -6 ********************************************
  -5 *************************************
  -4 ******************************
  -3 ***********************
  -2 ******************
  -1 *************
   0 *********
   1 *******
   2 ****
   3 ***
   4 **
   5 *
```
