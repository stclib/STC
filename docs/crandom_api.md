# STC [crand](../include/stc/crand.h): Pseudo Random Number Generator
![Random](pics/random.jpg)

This features a *64-bit PRNG* named **crand64**, and can generate bounded uniform and normal
distributed random numbers.

See [random](https://en.cppreference.com/w/cpp/header/random) for similar c++ functionality.

## Description

**crand64** is a very fast PRNG, suited for parallel usage. It is based on *sfc64*, but has a
different output function and state size. It features a Weyl-sequence as part of its state. 

**crand64** is faster or equally fast as *wyrand*, *xoshiro\*\**, *sfc64*, and *romu_trio*
with both **clang 16.0** and **gcc 13.1** from the [prng_bench.c](../misc/benchmarks/various/prng_bench.cpp)
on windows 11, Ryzen 7 5700X. (clang does not optimize *xoshiro\*\** and *sfc64* as well as gcc does).

**crand64** has no jump *function*, but each odd number Weyl-increment (state[4]) starts a new
unique 2^64 *minimum* length period, i.e. virtually unlimitied number of unique threads.
In contrast, *wyrand* and *sfc64* have only a (total) minimum period of 2^64 (*romu_trio* has
no guarantees), and may therefore not be suited for massive parallel usage (for purists).

**crand64** does not require multiplication or 128-bit integer operations. It has 320 bit state,
where 64-bits are constant per instance.

**crand64** passes *PractRand* (tested up to 8TB output), Vigna's Hamming weight test, and simple
correlation tests. The 16- and 32-bit variants also passes PractRand up to their size limits.

## Header file

All crand definitions and prototypes are available by including a single header file.
```c
#include <stc/crand.h>
```

## Methods

```c
void                csrand(uint64_t seed);                                // seed global crand64 prng
uint64_t            crand(void);                                          // global crand_u64(rng)
double              crandf(void);                                         // global crand_f64(rng)

crand_t             crand_init(uint64_t seed);
uint64_t            crand_u64(crand_t* rng);                              // range [0, 2^64 - 1]
double              crand_f64(crand_t* rng);                              // range [0.0, 1.0)

crand_uniform_t     crand_uniform_init(int64_t low, int64_t high);        // uniform-distribution range
int64_t             crand_uniform(crand_t* rng, crand_uniform_t* dist);

crand_normal_t      crand_normal_init(double mean, double stddev);        // normal-gauss distribution
double              crand_normal(crand_t* rng, crand_normal_t* dist);
```
## Types

| Name               | Type definition                           | Used to represent...         |
|:-------------------|:------------------------------------------|:-----------------------------|
| `crand_t`          | `struct {uint64_t state[4];}`             | The PRNG engine type         |
| `crand_uniform_t`  | `struct {int64_t lower; uint64_t range;}` | Integer uniform distribution |
| `crand_normal_t`   | `struct {double mean, stddev;}`           | Normal distribution type     |

## Example
```c
#include <time.h>
#include <stc/crand.h>
#define i_implement
#include <stc/cstr.h>

// Declare int -> int sorted map. Uses typetag 'i' for ints.
#define i_key int
#define i_val intptr_t
#define i_tag i
#include <stc/csmap.h>

int main(void)
{
    enum {N = 10000000};
    const double Mean = -12.0, StdDev = 6.0, Scale = 74;

    printf("Demo of gaussian / normal distribution of %d random samples\n", N);

    // Setup random engine with normal distribution.
    uint64_t seed = time(NULL);
    crand_t rng = crand_init(seed);
    crand_normal_t dist = crand_normal_init(Mean, StdDev);

    // Create histogram map
    csmap_i mhist = {0};
    c_forrange (N) {
        int index = (int)round(crand_normal(&rng, &dist));
        csmap_i_emplace(&mhist, index, 0).ref->second += 1;
    }

    // Print the gaussian bar chart
    cstr bar = {0};
    c_foreach (i, csmap_i, mhist) {
        int n = (int)(i.ref->second * StdDev * Scale * 2.5 / N);
        if (n > 0) {
            cstr_resize(&bar, n, '*');
            printf("%4d %s\n", i.ref->first, cstr_str(&bar));
        }
    }
    // Cleanup
    cstr_drop(&bar);
    csmap_i_drop(&mhist);
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
