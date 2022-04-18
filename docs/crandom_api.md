# STC [crandom](../include/stc/crandom.h): Pseudo Random Number Generator
![Random](pics/random.jpg)

This features a *64-bit PRNG* named **stc64**, and can generate bounded uniform and normal
distributed random numbers.

See [random](https://en.cppreference.com/w/cpp/header/random) for similar c++ functionality.

## Description

**stc64** is a novel, extremely fast PRNG by Tyge Løvset, suited for parallel usage. It features
Weyl-sequences as part of its state. It is inspired on *sfc64*, but has a different output function
and state size.

**sfc64** is the fastest among *pcg*, *xoshiro`**`*, and *lehmer*. It is equally fast as *sfc64* on
most platforms. *wyrand* is faster on platforms with fast 128-bit multiplication, and has 2^64 period
length (https://github.com/lemire/SwiftWyhash/issues/10). However, *wyrand* is not suited for massive
parallel usage due to its limited total minimal period length.

**stc64** does not require multiplication or 128-bit integer operations. It has 320 bit state,
but updates only 256 bit per generated number.

There is no *jump function*, but each odd number Weyl-increment (state[4]) starts a new
unique 2^64 *minimum* length period. For a single thread, a minimum period of 2^127 is generated
when the Weyl-increment is incremented by 2 every 2^64 output.

**stc64** passes *PractRand* (tested up to 8TB output), Vigna's Hamming weight test, and simple
correlation tests, i.e. *n* interleaved streams with only one-bit differences in initial state.
Also 32-bit and 16-bit versions passes PractRand up to their size limits.

For more, see the PRNG shootout by Vigna: http://prng.di.unimi.it and a debate between the authors of
xoshiro and pcg (Vigna/O'Neill) PRNGs: https://www.pcg-random.org/posts/on-vignas-pcg-critique.html

## Header file

All crandom definitions and prototypes are available by including a single header file.
```c
#include <stc/crandom.h>
```

## Methods

```c
void                csrandom(uint64_t seed);                                 // seed global stc64 prng
uint64_t            crandom(void);                                           // global stc64_rand(rng)
double              crandomf(void);                                          // global stc64_randf(rng)

stc64_t             stc64_new(uint64_t seed);                                // stc64_init(s) is deprecated
stc64_t             stc64_with_seq(uint64_t seed, uint64_t seq);             // with unique stream

uint64_t            stc64_rand(stc64_t* rng);                                // range [0, 2^64 - 1]
double              stc64_randf(stc64_t* rng);                               // range [0.0, 1.0)

stc64_uniform_t     stc64_uniform_new(int64_t low, int64_t high);            // uniform-distribution
int64_t             stc64_uniform(stc64_t* rng, stc64_uniform_t* dist);      // range [low, high]
stc64_uniformf_t    stc64_uniformf_new(double lowf, double highf);
double              stc64_uniformf(stc64_t* rng, stc64_uniformf_t* dist);    // range [lowf, highf)

stc64_normalf_t     stc64_normalf_new(double mean, double stddev);           // normal-distribution
double              stc64_normalf(stc64_t* rng, stc64_normalf_t* dist);
```
## Types

| Name               | Type definition                           | Used to represent...         |
|:-------------------|:------------------------------------------|:-----------------------------|
| `stc64_t`          | `struct {uint64_t state[4];}`             | The PRNG engine type         |
| `stc64_uniform_t`  | `struct {int64_t lower; uint64_t range;}` | Integer uniform distribution |
| `stc64_uniformf_t` | `struct {double lower, range;}`           | Real number uniform distr.   |
| `stc64_normalf_t`  | `struct {double mean, stddev;}`           | Normal distribution type     |

## Example
```c
#include <time.h>
#include <stc/crandom.h>
#include <stc/cstr.h>

// Declare int -> int sorted map. Uses typetag 'i' for ints.
#define i_key int
#define i_val size_t
#define i_tag i
#include <stc/csmap.h>

int main()
{
    enum {N = 10000000};
    const double Mean = -12.0, StdDev = 6.0, Scale = 74;

    printf("Demo of gaussian / normal distribution of %d random samples\n", N);

    // Setup random engine with normal distribution.
    uint64_t seed = time(NULL);
    stc64_t rng = stc64_new(seed);
    stc64_normalf_t dist = stc64_normalf_new(Mean, StdDev);

    // Create histogram map
    csmap_i mhist = csmap_i_init();
    c_forrange (N) {
        int index = (int) round( stc64_normalf(&rng, &dist) );
        csmap_i_emplace(&mhist, index, 0).ref->second += 1;
    }

    // Print the gaussian bar chart
    cstr bar = cstr_init();
    c_foreach (i, csmap_i, mhist) {
        size_t n = (size_t) (i.ref->second * StdDev * Scale * 2.5 / N);
        if (n > 0) {
            cstr_resize(&bar, n, '*');
            printf("%4d %s\n", i.ref->first, bar.str);
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
