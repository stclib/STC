# STC [crandom](../include/stc/algo/random.h): Pseudo Random Number Generator
![Random](pics/random.jpg)

This features a 64-bit or 32-bit PRNG, and can generate integers, reals, normal distributed,
and unbiased uniform random numbers. It currently use the romuTrio PRNG.

See [random](https://en.cppreference.com/w/cpp/header/random) for similar c++ functionality.

```c
#include "stc/algo/random.h"
```

## Methods

```c
void                 csrandom(uintptr_t seed);                   // seed global crandom() prng
uintptr_t            crandom(void);                              // global crandom_r(rng)
crandom_real_type    crandom_real(void);                         // global crandom_real_r(rng)
crandom_real_type    crandom_normal(crandom_normal_dist* d);     // global crandom_normal_r(rng, d)
int64_t              crandom_uniform(crandom_uniform_dist* d);   // global crandom_uniform_r(rng, d)

crandom_s            crandom_make(uintptr_t seed);               // create a crandom_s state from a seed value
crandom_uniform_dist crandom_uniform_lowhigh(int64_t low, int64_t high); // create a uniform distribution
uintptr_t            crandom_r(crandom_s* rng);                  // reentrant; return rnd in [0, UINTPTR_MAX]
crandom_real_type    crandom_real_r(crandom_s* rng);             // reentrant; return rnd in [0.0, 1.0)
crandom_real_type    crandom_normal_r(crandom_s* rng, crandom_normal_dist* d); // return normal distributed rnd's
int64_t              crandom_uniform_r(crandom_s* rng, crandom_uniform_dist* d); // return rnd in [low, high]
```
## Types

| Name                   | Type definition                   | Used to represent...         |
|:-----------------------|:----------------------------------|:-----------------------------|
| `crandom_s`            | `struct {uintptr_t data[3];}`     | The PRNG engine type         |
| `crandom_normal_dist`  | `struct {double mean, stddev;}`   | Normal distribution struct     |
| `crandom_uniform_dist` | `struct {...}`                    | Uniform int distribution struct |
| `crandom_real_type`    | `double` or `float`               | result type, depends on target platform |

## Example
```c
#include <time.h>
#include "stc/algo/random.h"
#define i_implement
#include "stc/cstr.h"

// Declare int => int sorted map.
#define i_type SortedMap,int,long
#include "stc/smap.h"

int main(void)
{
    enum {N = 10000000};
    printf("Demo of gaussian / normal distribution of %d random samples\n", N);

    // Setup random engine with normal distribution.
    uint64_t seed = time(NULL);
    crandom_s rng = crandom_make(seed);
    crandom_normal_dist dist = {.mean=-12.0, .stddev=6.0};

    // Create histogram map
    SortedMap mhist = {0};
    c_forrange (N) {
        int index = (int)round(crandom_normal_r(&rng, &dist));
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
