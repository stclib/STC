# STC [crand64 / crand32](../include/stc/random.h): Pseudo Random Number Generator
![Random](pics/random.jpg)

This features an excellent 32- and 64-bit Pseudo Random Number Geneator (PRNG).

### Comparison of crand64 with [xoshiro256\*\*](https://prng.di.unimi.it/)
Several programming languages uses xoshiro256\*\* as the default PRNG, let's compare.

- **crand64** is based on the highly rated **SFC64**, which along with **xoshiro256\*\*** both have excellent results from currently
available random test-suites. **SFC64** has minimum period length of 2^64.
- **crand64** uses a modified output function that incorporate a "stream" parameter value. This makes it suitable for mass-parallel deployment.
- **crand64** can generate 2^63 unique streams, where each has 2^64 minimum period length. This is adequate even for large-scale
experiments using random numbers.
- **xoshiro256\*\*** has the full 2^256 period length. This is overkill, but it also has several disadvantages:
    - Requires *jump-functions*, which the user must call in order to split up the output ranges before parallel execution.
    - Generator may end up in "zeroland" or "oneland" states (nearly all bits 0s or 1s for multiple outputs in a row), and will
generate bad quality output. See [A Quick Look at Xoshiro256\*\*](https://www.pcg-random.org/posts/a-quick-look-at-xoshiro256.html).
    - Trivial predictablity: previous outputs along with all future ones can trivially be computed from four output samples.
- **crand64** does not need jump-functions. Instead, one can simply pass an odd unique id/number to each stream/thread as argument.
- **crand64** is 10-20% faster than **xoshiro256\*\***. Unlike **xoshiro**, it does not require (fast hardware) multiplication support.
- **crand64** has 256 bits state, 192 of them are "chaotic". 64 bits are used to ensure long minimum period lengths.
- **crand64**'s output function result is fed back into the state, resulting in a chaotic random state.
It combines XOR, SHIFT ***and*** ADD state modifying bit-operations to ensure excellent bit-mixing.
- **xoshiro256\*\***'s output is not fed back into its state, instead every bit-state possible is visited in sequence, using a rule-based
iterator with XOR and SHIFT bit-operations only. To compensate for the regularity in the state change between each generated number,
it uses a fairly expensive output function (but trivially invertible, see ref) involving two multiplications.

See [random](https://en.cppreference.com/w/cpp/header/random) for similar c++ functionality.

```c
#define i_implement // implement normal distribution functions
#include "stc/random.h"
```

## Methods (64-bit)

```c
void                 crand64_seed(uint64_t seed);                        // set global crand64_uint() seed
uint64_t             crand64_uint(void);                                 // global crand64_uint_r(rng)
double               crand64_real(void);                                 // global crand64_real_r(rng)
crand64_uniform_dist crand64_uniform_from(int64_t low, int64_t high);    // create an unbiased uniform distribution
int64_t              crand64_uniform(crand64_uniform_dist* d);           // global crand64_uniform_r(rng, d)
double               crand64_normal(crand64_normal_dist* d);             // global crand64_normal_r(rng, d)

crand64              crand64_from(uint64_t seed);                        // create a crand64 state from a seed value
uint64_t             crand64_uint_r(crand64* rng, uint64_t strm);        // reentrant; return rnd in [0, UINT64_MAX]
double               crand64_real_r(crand64* rng, uint64_t strm);        // reentrant; return rnd in [0.0, 1.0)
int64_t              crand64_uniform_r(crand64* rng, uint64_t strm, crand64_uniform_dist* d); // return rnd in [low, high]
double               crand64_normal_r(crand64* rng, uint64_t strm, crand64_normal_dist* d);   // return normal distributed rnd's
```
Note that `strm` must be an odd number.
## Types

| Name                   | Type definition                   | Used to represent...         |
|:-----------------------|:----------------------------------|:-----------------------------|
| `crand64`              | `struct {uint64_t data[3];}`      | The PRNG engine type         |
| `crand64_uniform_dist` | `struct {...}`                    | Uniform int distribution struct |
| `crand64_normal_dist`  | `struct {double mean, stddev;}`   | Normal distribution struct     |

## Methods (32-bit)
```c
void                 crand32_seed(uint32_t seed);                        // set global crand32_uint() seed
uint32_t             crand32_uint(void);                                 // global crand32_uint_r(rng)
float                crand32_real(void);                                 // global crand32_real_r(rng)
crand32_uniform_dist crand32_uniform_from(int32_t low, int32_t high);    // create an unbiased uniform distribution
int32_t              crand32_uniform(crand64_uniform_dist* d);           // global crand32_uniform_r(rng, d)

crand32              crand32_from(uint32_t seed);                        // create a crand32 state from a seed value
uint32_t             crand32_uint_r(crand32* rng, uint32_t strm);        // reentrant; return rnd in [0, UINT32_MAX]
float                crand32_real_r(crand32* rng, uint32_t strm);        // reentrant; return rnd in [0.0, 1.0)
int32_t              crand32_uniform_r(crand32* rng, uint32_t strm, crand32_uniform_dist* d); // return rnd in [low, high]
```

| Name                   | Type definition                   | Used to represent...         |
|:-----------------------|:----------------------------------|:-----------------------------|
| `crand32`              | `struct {uint32_t data[3];}`      | The PRNG engine type         |
| `crand32_uniform_dist` | `struct {...}`                    | Uniform int distribution struct |

## Example
```c
#include <time.h>
#define STC_IMPLEMENT
#include "stc/random.h"
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
