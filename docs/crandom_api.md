# STC [crandom](../include/stc/algo/random.h): Pseudo Random Number Generator
![Random](pics/random.jpg)

This features a *64-bit or 32-bit PRNG* named **crandom**, and can generate integers and normal
distributed random numbers.

See [random](https://en.cppreference.com/w/cpp/header/random) for similar c++ functionality.

```c
#include "stc/algo/random.h"
```

## Methods

```c
void        csrandom(uintptr_t seed);                   // seed global crand64 prng
uintptr_t   crandom(void);                              // global crandom_r(rng)
cranreal    crandom_real(void);                         // global crandom_real_r(rng)
cranreal    crandom_normal(crandom_normal_distr* d);    // global crandom_normal_r(rng, d)

crandom_s   crandom_make(uintptr_t seed);               // make a crandom_s from a seed value
uintptr_t   crandom_r(crandom_s* rng);                  // range [0, UINTPTR_MAX]
cranreal    crandom_real_r(crandom_s* rng);             // range [0.0, 1.0)
cranreal    crandom_normal_r(crandom_s* rng, crandom_normal_distr* d);
```
## Types

| Name                   | Type definition                   | Used to represent...         |
|:-----------------------|:----------------------------------|:-----------------------------|
| `crandom_s`            | `struct {uintptr_t data[3];}`     | The PRNG engine type         |
| `crandom_normal_distr` | `struct {double mean, stddev;}`   | Normal distribution type     |

## Example
```c
#include <time.h>
#include "stc/algo/random.h"
#define i_implement
#include "stc/cstr.h"

// Declare int => int sorted map. Use typetag 'i' for ints.
#define i_type smap_i, int, isize
#include "stc/smap.h"

int main(void)
{
    enum {N = 10000000};
    const double Mean = -12.0, StdDev = 6.0, Scale = 74;

    printf("Demo of gaussian / normal distribution of %d random samples\n", N);

    // Setup random engine with normal distribution.
    uint64_t seed = time(NULL);
    crandom_s rng = crandom_make(seed);
    crandom_normal_distr dist = {.mean=Mean, .stddev=StdDev};

    // Create histogram map
    smap_i mhist = {0};
    c_forrange (N) {
        int index = (int)round(crandom_normal_r(&rng, &dist));
        smap_i_emplace(&mhist, index, 0).ref->second += 1;
    }

    // Print the gaussian bar chart
    cstr bar = {0};
    c_foreach (i, smap_i, mhist) {
        int n = (int)(i.ref->second * StdDev * Scale * 2.5 / N);
        if (n > 0) {
            cstr_resize(&bar, n, '*');
            printf("%4d %s\n", i.ref->first, cstr_str(&bar));
        }
    }
    // Cleanup
    cstr_drop(&bar);
    smap_i_drop(&mhist);
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
