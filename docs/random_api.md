# STC [crand64 / crand32](../include/stc/random.h): Pseudo Random Number Generator
![Random](pics/random.jpg)

This features both a 64-bit and a 32-bit PRNG API. It can generate integers, reals, normal distributed,
and unbiased uniform random numbers. It uses the STC64 variant of the extremely fast and excellent SFC64
PRNG, which uses no multiplication. It adds a "stream" parameter and modifies the output function slightly.
The PRNG can generate 2^63 unique streams where each has theoretical minimum 2^64 period lengths.

See [random](https://en.cppreference.com/w/cpp/header/random) for similar c++ functionality.

```c
#include "stc/random.h"
```

## Methods (64-bit)

```c
void                 crand64_seed(uint64_t seed);                        // set global crand64_uint() seed
uint64_t             crand64_uint(void);                                 // global crand64_uint_r(rng)
double               crand64_real(void);                                 // global crand64_real_r(rng)
double               crand64_normal(crand64_normal_dist* d);             // global crand64_normal_r(rng, d)
crand64_uniform_dist crand64_uniform_from(int64_t low, int64_t high);    // create a uniform distribution
int64_t              crand64_uniform(crand64_uniform_dist* d);           // global crand64_uniform_r(rng, d)

crand64              crand64_from(uint64_t seed);                        // create a crand64 state from a seed value
uint64_t             crand64_uint_r(crand64* rng, uint64_t strm);        // reentrant; return rnd in [0, UINT64_MAX]
double               crand64_real_r(crand64* rng, uint64_t strm);        // reentrant; return rnd in [0.0, 1.0)
double               crand64_normal_r(crand64* rng, uint64_t strm, crand64_normal_dist* d);   // return normal distributed rnd's
int64_t              crand64_uniform_r(crand64* rng, uint64_t strm, crand64_uniform_dist* d); // return rnd in [low, high]
```
Note that `strm` must be an odd number.
## Types

| Name                   | Type definition                   | Used to represent...         |
|:-----------------------|:----------------------------------|:-----------------------------|
| `crand64`              | `struct {uint64_t data[3];}`      | The PRNG engine type         |
| `crand64_normal_dist`  | `struct {double mean, stddev;}`   | Normal distribution struct     |
| `crand64_uniform_dist` | `struct {...}`                    | Uniform int distribution struct |

## Methods (32-bit)
```c
void                 crand32_seed(uint32_t seed);                        // set global crand32_uint() seed
uint32_t             crand32_uint(void);                                 // global crand32_uint_r(rng)
float                crand32_real(void);                                 // global crand32_real_r(rng)
float                crand32_normal(crand64_normal_dist* d);             // global crand32_normal_r(rng, d)
crand32_uniform_dist crand32_uniform_from(int32_t low, int32_t high);    // create a uniform distribution
int32_t              crand32_uniform(crand64_uniform_dist* d);           // global crand32_uniform_r(rng, d)

crand32              crand32_from(uint32_t seed);                        // create a crand32 state from a seed value
uint32_t             crand32_uint_r(crand32* rng, uint32_t strm);        // reentrant; return rnd in [0, UINT32_MAX]
float                crand32_real_r(crand32* rng, uint32_t strm);        // reentrant; return rnd in [0.0, 1.0)
float                crand32_normal_r(crand32* rng, uint32_t strm, crand32_normal_dist* d);   // return normal distributed rnd's
int32_t              crand32_uniform_r(crand32* rng, uint32_t strm, crand32_uniform_dist* d); // return rnd in [low, high]
```

| Name                   | Type definition                   | Used to represent...         |
|:-----------------------|:----------------------------------|:-----------------------------|
| `crand32`              | `struct {uint32_t data[3];}`      | The PRNG engine type         |
| `crand32_normal_dist`  | `struct {float mean, stddev;}`    | Normal distribution struct     |
| `crand32_uniform_dist` | `struct {...}`                    | Uniform int distribution struct |

## Example
```c
#include <time.h>
#include "stc/random.h"
#define i_implement
#include "stc/cstr.h"

// Declare int => int sorted map.
#define i_type SortedMap,int,long
#include "stc/smap.h"

int main(void)
{
    enum {N = 10000000};
    printf("Demo of gaussian / normal distribution of %d random samples\n", N);

    // Setup a reentrant random number engine with normal distribution.
    uint64_t seed = time(NULL);
    crand64 rng = crand64_from(seed);
    crand64_normal_dist dist = {.mean=-12.0, .stddev=6.0};

    // Create histogram map
    SortedMap mhist = {0};
    c_forrange (N) {
        int index = (int)round(crand64_normal_r(&rng, 1, &dist));
        SortedMap_insert(&mhist, index, 0).ref->second += 1;
    }

    // Print the gaussian bar chart
    cstr bar = {0};
    const double scale = 74;
    c_foreach (i, SortedMap, mhist) {
        int n = (int)(i.ref->second * dist.stddev * scale * 2.5 / N);
        if (n > 0) {
            cstr_resize(&bar, n, '*');
            printf("%4d %s\n", i.ref->first, cstr_str(&bar));
        }
    }
    // Cleanup
    cstr_drop(&bar);
    SortedMap_drop(&mhist);
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
