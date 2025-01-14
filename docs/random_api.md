# STC [crand64 / crand32](../include/stc/random.h): Pseudo Random Number Generator
![Random](pics/random.jpg)

A high quality, very fast 32- and 64-bit Pseudo Random Number Geneator (PRNG). It features
uniform and normal distributed random numbers and float/doubles conversions.

<details>
<summary>A comparison with xoshiro256**</summary>

Several programming languages uses xoshiro256\*\* as the default PRNG. Let's compare.

### Comparison of crand64 with [xoshiro256\*\*](https://prng.di.unimi.it/)
- **crand64** is based on **SFC64**, which along with **xoshiro256\*\*** both have excellent results
from currently available random test-suites. **SFC64** has a minimum period length of 2^64.
- **crand64** uses a modified output function that incorporate a "stream" parameter value.
  It can generate 2^63 unique streams, where each has 2^64 minimum period lengths. This is
  adequate even for large-scale experiments using random numbers.
- **xoshiro256\*\*** has the full 2^256 period length. This however has some disadvantages:
    - Trivially predictable and invertible: previous outputs along with all future ones can trivially be computed from four
      output samples.
    - Requires *jump-functions*, which the user must call in order to split up the output ranges before parallel execution.
    - Overkill: Even to create "as few as" 2^64 random numbers in one thread at 1ns per number takes 584 years.
    - The generator may end up in "zeroland" or "oneland" states (nearly all bits 0s or 1s for multiple outputs in a row), and will
      generate low quality output. See [A Quick Look at Xoshiro256\*\*](https://www.pcg-random.org/posts/a-quick-look-at-xoshiro256.html).
- **crand64** does not need jump-functions. Instead one can simply pass a unique odd id/number to each stream/thread as argument.
- **crand64** is 10-20% faster than **xoshiro256\*\***. Unlike **xoshiro**, it does not rely on fast hardware multiplication support.
- **crand64** has a 256 bits state, 192 bits are "chaotic". 64 bits are used to ensure a long minimum period length. The output
  function result is fed back into the state, resulting in the partially chaotic random state. It also combines XOR, SHIFT ***and ADD***
  state modifying bit-operations to ensure excellent state randomness.
- **xoshiro256\*\***'s output is not fed back into its state, instead every possible bit-state is iterated over by applying XOR and
SHIFT bit-operations exclusively. Like with Mersenne Twister, the extreme period length has a cost: Because of the highly regulated
state changes, a relative expensive output function with two multiplications is needed to achieve high quality output.
</details>

## Header file
```c++
#include "stc/random.h"
```

## Methods (64-bit)

```c++
                // Use global state
void            crand64_seed(uint64_t seed);                        // seed global rng64 state
uint64_t        crand64_uint(void);                                 // global crand64_uint_r(rng64, 1)
double          crand64_real(void);                                 // global crand64_real_r(rng64, 1)
crand64_uniform_dist
                crand64_make_uniform(int64_t low, int64_t high);    // create an unbiased uniform distribution
int64_t         crand64_uniform(crand64_uniform_dist* d);           // global crand64_uniform_r(rng64, 1, d)
                // requires linking with stc lib.
double          crand64_normal(crand64_normal_dist* d);             // global crand64_normal_r(rng64, 1, d)
```
```c++
                // Use local state
crand64         crand64_from(uint64_t seed);                        // create a crand64 state from a seed value
uint64_t        crand64_uint_r(crand64* rng, uint64_t strm);        // reentrant; return rnd in [0, UINT64_MAX]
double          crand64_real_r(crand64* rng, uint64_t strm);        // reentrant; return rnd in [0.0, 1.0)
int64_t         crand64_uniform_r(crand64* rng, uint64_t strm, crand64_uniform_dist* d); // return rnd in [low, high]
double          crand64_normal_r(crand64* rng, uint64_t strm, crand64_normal_dist* d);   // return normal distributed rnd's
```
```c++
                // Generic algorithms (uses 64 or 32 bit depending on word size):
void            c_shuffle_seed(size_t seed);                        // calls crand64_seed() or crand32_seed()
void            c_shuffle(TYPE CntType, CntType* cnt);              // shuffle a cspan, vec, stack, queue or deque type.
void            c_shuffle_array(T* array, isize n);                 // shuffle an array of elements.
```
Note that `strm` must be an odd number.
## Types

| Name                   | Type definition                   | Used to represent...         |
|:-----------------------|:----------------------------------|:-----------------------------|
| `crand64`              | `struct {uint64_t data[3];}`      | The PRNG engine type         |
| `crand64_uniform_dist` | `struct {...}`                    | Uniform int distribution struct |
| `crand64_normal_dist`  | `struct {double mean, stddev;}`   | Normal distribution struct     |

## Methods (32-bit)
```c++
void                 crand32_seed(uint32_t seed);                        // seed global rng32 state
uint32_t             crand32_uint(void);                                 // global crand32_uint_r(rng32, 1)
float                crand32_real(void);                                 // global crand32_real_r(rng32, 1)
crand32_uniform_dist crand32_make_uniform(int32_t low, int32_t high);    // create an unbiased uniform distribution
int32_t              crand32_uniform(crand64_uniform_dist* d);           // global crand32_uniform_r(rng32, 1, d)

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

[ [Run this code](https://godbolt.org/z/3PGadv5zz) ]
```c++
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "stc/random.h"

#define i_type SortedMap, int, long
#include "stc/smap.h" // sorted map.

int main(void)
{
    enum {N = 5000000};
    printf("Demo of gaussian / normal distribution of %d random samples\n", N);

    // Setup a reentrant random number engine with normal distribution.
    crand64_seed((uint64_t)time(NULL));
    crand64_normal_dist dist = {.mean=-12.0, .stddev=4.0};

    // Create histogram map
    SortedMap mhist = {0};
    for (c_range(N)) {
        int index = (int)round(crand64_normal(&dist));
        SortedMap_insert(&mhist, index, 0).ref->second += 1;
    }

    // Print the gaussian bar chart
    const double scale = 50;
    for (c_each(i, SortedMap, mhist)) {
        int n = (int)((double)i.ref->second * dist.stddev * scale * 2.5 / N);
        if (n > 0) {
            printf("%4d ", i.ref->first);
            for (c_range(n)) printf("*");
            puts("");
        }
    }
    SortedMap_drop(&mhist);
}
```
