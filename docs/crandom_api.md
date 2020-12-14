# Module crandom: Pseudo Random Number Generators

This describes the API of module **crand**. It contains *pcg32* created by Melissa O'Neill, and an
*64-bit PRNG* created by Tyge Løvset. The PRNG can generate uniform and normal distributions.

## Types

| Name                | Type definition                             | Used to represent...         |
|:--------------------|:--------------------------------------------|:-----------------------------|
| `cstc64_t`          | `struct {uint64_t state[4];}`               | The PRNG engine type         |
| `cstc64_uniform_t`  | `struct {int64_t offset; uint64_t range;}`  | Integer uniform distribution |
| `cstc64_uniformf_t` | `struct {double offset, range;}`            | Real number uniform distr.   |
| `cstc64_normalf_t`  | `struct {double mean, stddev;}`             | Normal distribution type     |

## Header file

All cstr definitions and prototypes may be included in your C source file by including a single header file.
```c
#include "stc/crandom.h"
```

## Methods

```c
 1)     cstc64_t            cstc64_with_seq(uint64_t seed, uint64_t seq);
 2)     cstc64_t            cstc64_init(uint64_t seed);
 3)     uint64_t            cstc64_rand(cstc64_t* rng);
 4)     double              cstc64_randf(cstc64_t* rng);
 5)     cstc64_uniform_t    cstc64_uniform_init(int64_t low, int64_t high);
 6)     int64_t             cstc64_uniform(cstc64_t* rng, cstc64_uniform_t* dist);
 7)     cstc64_uniformf_t   cstc64_uniformf_init(double low, double high);
 8)     double              cstc64_uniformf(cstc64_t* rng, cstc64_uniformf_t* dist);
 9)     cstc64_normalf_t    cstc64_normalf_init(double mean, double stddev);
10)     double              cstc64_normalf(cstc64_t* rng, cstc64_normalf_t* dist);
```
`1-2)` PRNG 64-bit engine initializers. `3)` Integer generator, range \[0, 2^64).
`4)` Double RNG with range \[0, 1). `5-6)` Uniform integer RNG with range \[*low*, *high*].
`7-8)` Uniform double RNG with range \[*low*, *high*). `9-10)` Normal-distributed double
RNG, around 68% of the values are within the range [*mean* - *stddev, *mean* + *stddev*].

The method `cstc64_rand(cstc64_t* rng)` is an extremely fast PRNG by Tyge Løvset, suited for parallel usage.
It features a Weyl-sequence as part of the state. It is faster than *sfc64*, *wyhash64*, *pcg64*, and almost 
50% faster than *xoshiro256\*\** on GCC. It does not require fast multiplication or 128-bit integer operations.
It has a 256 bit state, but updates only 192 bit per generated number. There is no *jump function*, but incrementing
the Weyl-increment by 2, initializes a new unique 2^64 *minimum* length period. Passes *PractRand*, tested up to
8TB output, Vigna's Hamming weight test, and simple correlation tests, i.e. interleaved streams with one-bit diff state.

## Example
```c
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "stc/crandom.h"
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
    cstc64_t rng = cstc64_init(seed);
    cstc64_normalf_t dist = cstc64_normalf_init(Mean, StdDev);

    // Create histogram map
    cmap_i mhist = cmap_i_init();
    for (size_t i = 0; i < N; ++i) {
        int index = (int) round( cstc64_normalf(&rng, &dist) );
        cmap_i_emplace(&mhist, index, 0).first->second += 1;
    }

    // Transfer map to vec and sort it by map entry keys.
    cvec_e vhist = cvec_e_init();
    c_foreach (i, cmap_i, mhist)
        cvec_e_push_back(&vhist, *i.val);
    cvec_e_sort(&vhist);

    // Print the gaussian bar chart
    cstr_t bar = cstr_init();
    c_foreach (i, cvec_e, vhist) {
        size_t n = (size_t) (i.val->second * StdDev * Scale * 2.5 / N);
        if (n > 0) {
            cstr_resize(&bar, n, '*');
            printf("%4d %s\n", i.val->first, bar.str);
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
