# STC [cbits](../stc/cbits.h): Bitset
![Bitset](pics/bitset.jpg)

A **cbits** represents a set of bits. It provides accesses to the value of individual bits via *cbits_test()* and provides the bitwise operators that one can apply to builtin integers. The number of bits in the set is specified at runtime via a parameter to the constructor *cbits_with_size()* or by *cbits_resize()*. A **cbits** bitset can be manipulated by standard logic operators and converted to and from strings.

See the c++ class [std::bitset](https://en.cppreference.com/w/cpp/utility/bitset) or
[boost::dynamic_bitset](https://www.boost.org/doc/libs/release/libs/dynamic_bitset/dynamic_bitset.html)
for a functional description.

## Header file

All cstr definitions and prototypes may be included in your C source file by including a single header file.

```c
#include "stc/cbits.h"
```
## Methods

```c
cbits            cbits_init(void);
cbits            cbits_with_size(size_t size, bool value);
cbits            cbits_from_str(const char* str);

cbits            cbits_clone(cbits_t other);
void             cbits_resize(cbits_t* self, size_t size, bool value);

cbits            cbits_intersect(cbits_t s1, cbits_t s2);
cbits            cbits_union(cbits_t s1, cbits_t s2);
cbits            cbits_xor(cbits_t s1, cbits_t s2);
cbits            cbits_not(cbits_t s1);

void             cbits_del(cbits_t* self);

cbits*           cbits_assign(cbits_t* self, cbits_t other);
cbits*           cbits_take(cbits_t* self, cbits_t other);
cbits            cbits_move(cbits_t* self);

size_t           cbits_size(cbits_t set);
size_t           cbits_count(cbits_t set);
bool             cbits_is_disjoint(cbits_t set, cbits_t other);
bool             cbits_is_subset(cbits_t set, cbits_t other);
bool             cbits_is_superset(cbits_t set, cbits_t other);
char*            cbits_to_str(cbits_t set, char* str, size_t start, intptr_t stop);

bool             cbits_test(cbits_t set, size_t i);
bool             cbits_at(cbits_t set, size_t i);   // same as cbits_test()

void             cbits_set(cbits_t *self, size_t i);
void             cbits_reset(cbits_t *self, size_t i);
void             cbits_set_value(cbits_t *self, size_t i, bool value);
void             cbits_flip(cbits_t *self, size_t i);
void             cbits_set_all(cbits_t *self, bool value);
void             cbits_set_all64(cbits_t *self, uint64_t pattern);
void             cbits_flip_all(cbits_t *self);

void             cbits_intersect_with(cbits_t *self, cbits_t other);
void             cbits_union_with(cbits_t *self, cbits_t other);
void             cbits_xor_with(cbits_t *self, cbits_t other);

cbits_iter_t     cbits_begin(cbits_t* self);
cbits_iter_t     cbits_end(cbits_t* self);
void             cbits_next(cbits_iter_t* it);
bool             cbits_itval(cbits_iter_t it);
```

## Types

| cbits               | Type definition           | Used to represent...         |
|:--------------------|:--------------------------|:-----------------------------|
| `cbits`             | `struct { ... }`          | The cbits type               |
| `cbits_iter_t`      | `struct { ... }`          | The cbits iterator type      |

## Example
```c
#include <stdio.h>
#include "stc/cbits.h"

static inline cbits sieveOfEratosthenes(size_t n)
{
    cbits primes = cbits_with_size(n + 1, true);
    cbits_reset(&primes, 0);
    cbits_reset(&primes, 1);

    c_forrange (i, size_t, 2, n+1) {
        // If primes[i] is not changed, then it is a prime
        if (cbits_test(primes, i) && i*i <= n) {
            c_forrange (j, size_t, i*i, n+1, i) {
                cbits_reset(&primes, j);
            }
        }
    }
    return primes;
}

int main(void)
{
    int n = 100000000;
    printf("computing prime numbers up to %u\n", n);

    cbits primes = sieveOfEratosthenes(n);
    puts("done");

    size_t np = cbits_count(primes);
    printf("number of primes: %zu\n", np);

    printf("2 ");
    c_forrange (i, int, 3, 1001, 2) {
       if (cbits_test(primes, i)) printf("%d ", i);
    }
    puts("");
    cbits_del(&primes);
}
```
Output:
```
computing prime numbers up to 100000000
done
number of primes: 5761455
2 3 5 7 11 13 17 19 23 29 31 37 41 43 47 53 59 61 67 71 73 79 83 89 97 101 103 107 109 113 127 131 137 139 149 151 157 163 167 173 179 181 191 193 197 199 211 223 227 229 233 239 241 251 257 263 269 271 277 281 283 293 307 311 313 317 331 337 347 349 353 359 367 373 379 383 389 397 401 409 419 421 431 433 439 443 449 457 461 463 467 479 487 491 499 503 509 521 523 541 547 557 563 569 571 577 587 593 599 601 607 613 617 619 631 641 643 647 653 659 661 673 677 683 691 701 709 719 727 733 739 743 751 757 761 769 773 787 797 809 811 821 823 827 829 839 853 857 859 863 877 881 883 887 907 911 919 929 937 941 947 953 967 971 977 983 991 997
```
