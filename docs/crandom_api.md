# Introduction

UNDER CONSTRUCTION!

This describes the API of module **crandom**.

## Types

| crandom               | Type definition                            | Used to represent...                 |
|:----------------------|:-------------------------------------------|:-------------------------------------|
| `crand_rng32_t`       | `struct {uint64_t state[2];}`              | The crandom type                     |
| `crand_uniform_i32_t` | `struct {int32_t offset; uint32_t range;}` | The crandom element type             |
| `crand_uniform_f32_t` | `struct {float offset, range;}`            | crandom iterator                     |
| `crand_rng64_t`       | `struct {uint64_t state[4];}`              |                                      |
| `crand_uniform_i64_t` | `struct {int64_t offset; uint64_t range;}` |                                      |
| `crand_uniform_f64_t` | `struct {double offset, range;}`           |                                      |
| `crand_normal_f64_t`  | `struct {double mean, stddev, next; bool has_next;}` |                            |

## Header file

All cstr definitions and prototypes may be included in your C source file by including a single header file.
```c
#include "stc/crandom.h"
```

## Methods

### Construction

Interfaces to create 32-bits random numbers:
```c
crand_rng32_t           crand_rng32_with_seq(uint64_t seed, uint64_t seq);
crand_rng32_t           crand_rng32_init(uint64_t seed);

uint32_t                crand_i32(crand_rng32_t* rng); /* int random number generator, range [0, 2^32) */
float                   crand_f32(crand_rng32_t* rng);

crand_uniform_i32_t     crand_uniform_i32_init(int32_t low, int32_t high);  /* int generator in range [low, high] */
int32_t                 crand_uniform_i32(crand_rng32_t* rng, crand_uniform_i32_t* dist);

uint32_t                crand_unbiased_i32(crand_rng32_t* rng, crand_uniform_i32_t* dist); /* https://github.com/lemire/fastrange */
crand_uniform_f32_t     crand_uniform_f32_init(float low, float high); /* float random number in range [low, high). Note: 23 bit resolution. */
float                   crand_uniform_f32(crand_rng32_t* rng, crand_uniform_f32_t* dist);
```
Interfaces to create 64-bits random numbers:
```c
crand_rng64_t           crand_rng64_with_seq(uint64_t seed, uint64_t seq); /* engine initializers */
crand_rng64_t           crand_rng64_init(uint64_t seed);

uint64_t                crand_i64(crand_rng64_t* rng); /* int generator, range [0, 2^64). PRNG copyright Tyge Løvset, NORCE Research, 2020 */
double                  crand_f64(crand_rng64_t* rng); /* double random number in range [low, high). 52 bit resolution. */

crand_uniform_i64_t     crand_uniform_i64_init(int64_t low, int64_t high); /* int generator in range [low, high] */
int64_t                 crand_uniform_i64(crand_rng64_t* rng, crand_uniform_i64_t* dist);

crand_uniform_f64_t     crand_uniform_f64_init(double low, double high);
double                  crand_uniform_f64(crand_rng64_t* rng, crand_uniform_f64_t* dist);

crand_normal_f64_t      crand_normal_f64_init(double mean, double stddev);
double                  crand_normal_f64(crand_rng64_t* rng, crand_normal_f64_t* dist);
```
