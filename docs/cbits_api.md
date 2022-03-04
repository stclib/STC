# STC [cbits](../include/stc/cbits.h): Bitset
![Bitset](pics/bitset.jpg)

A **cbits** represents a set of bits. It provides accesses to the value of individual bits via *cbits_test()* and provides the bitwise operators that one can apply to builtin integers. The number of bits in the set is specified at runtime via a parameter to the constructor *cbits_with_size()* or by *cbits_resize()*. A **cbits** bitset can be manipulated by standard logic operators and converted to and from strings.

See the c++ class [std::bitset](https://en.cppreference.com/w/cpp/utility/bitset) or
[boost::dynamic_bitset](https://www.boost.org/doc/libs/release/libs/dynamic_bitset/dynamic_bitset.html)
for a functional description.

## Header file

All cbits definitions and prototypes are available by including a single header file.

```c
#include <stc/cbits.h>
```
## Methods

```c
cbits            cbits_init(void);
cbits            cbits_new(const char literal[]);
cbits            cbits_from(const char* str);
cbits            cbits_with_size(size_t size, bool value);
cbits            cbits_with_values(size_t size, uint64_t pattern);
cbits            cbits_clone(cbits other);

void             cbits_clear(cbits* self);
cbits*           cbits_copy(cbits* self, cbits other);
void             cbits_resize(cbits* self, size_t size, bool value);
void             cbits_drop(cbits* self);

cbits*           cbits_take(cbits* self, cbits other);       // give other to self
cbits            cbits_move(cbits* self);                    // transfer self to caller

size_t           cbits_size(cbits set);
size_t           cbits_count(cbits set);                     // count number of bits set

bool             cbits_test(cbits set, size_t i);
bool             cbits_at(cbits set, size_t i);              // same as cbits_test()
bool             cbits_subset_of(cbits set, cbits other);    // is set a subset of other?
bool             cbits_disjoint(cbits set, cbits other);     // no common bits
char*            cbits_to_str(cbits set, char* str, size_t start, intptr_t stop);

void             cbits_set(cbits *self, size_t i);
void             cbits_reset(cbits *self, size_t i);
void             cbits_set_all(cbits *self, bool value);
void             cbits_set_value(cbits *self, size_t i, bool value);
void             cbits_set_values(cbits *self, uint64_t pattern);
void             cbits_flip_all(cbits *self);
void             cbits_flip(cbits *self, size_t i);

void             cbits_intersect(cbits *self, cbits other);
void             cbits_union(cbits *self, cbits other);
void             cbits_xor(cbits *self, cbits other);        // set of disjoint bits
```

## Types

| cbits               | Type definition           | Used to represent...         |
|:--------------------|:--------------------------|:-----------------------------|
| `cbits`             | `struct { ... }`          | The cbits type               |
| `cbits_iter`        | `struct { ... }`          | The cbits iterator type      |

## Example
```c
#include <stc/cbits.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

cbits sieveOfEratosthenes(size_t n)
{
    cbits bits = cbits_with_size(n>>1, true);
    size_t q = (size_t) sqrt(n);

    for (size_t i = 3; i <= q; i += 2) {
        for (size_t j = i; j < n; j += 2) {
            if (cbits_test(bits, j>>1)) {
                i = j;
                break;
            }
        }
        for (size_t j = i*i; j < n; j += i*2)
            cbits_reset(&bits, j>>1);
    }
    return bits;
}

int main(void)
{
    size_t n = 100000000;
    printf("computing prime numbers up to %" PRIuMAX "\n", n);

    clock_t t1 = clock();
    cbits primes = sieveOfEratosthenes(n + 1);
    size_t nprimes = cbits_count(primes);
    clock_t t2 = clock();

    printf("number of primes: %" PRIuMAX ", time: %f\n", nprimes, (float)(t2 - t1)/CLOCKS_PER_SEC);

    printf(" 2");
    for (size_t i = 3; i < 1000; i += 2)
       if (cbits_test(primes, i>>1)) printf(" %" PRIuMAX "", i);
    puts("");

    cbits_drop(&primes);
}
```
Output:
```
computing prime numbers up to 100000000
done
number of primes: 5761455
2 3 5 7 11 13 17 19 23 29 31 37 41 43 47 53 59 61 67 71 73 79 83 89 97 101 103 107 109 113 127 131 137 139 149 151 157 163 167 173 179 181 191 193 197 199 211 223 227 229 233 239 241 251 257 263 269 271 277 281 283 293 307 311 313 317 331 337 347 349 353 359 367 373 379 383 389 397 401 409 419 421 431 433 439 443 449 457 461 463 467 479 487 491 499 503 509 521 523 541 547 557 563 569 571 577 587 593 599 601 607 613 617 619 631 641 643 647 653 659 661 673 677 683 691 701 709 719 727 733 739 743 751 757 761 769 773 787 797 809 811 821 823 827 829 839 853 857 859 863 877 881 883 887 907 911 919 929 937 941 947 953 967 971 977 983 991 997
```
