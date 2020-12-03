# Container type cbitset_t: Indexed Set of Boolean Values (Bits)

This describes the API of type **cbitset**.

## Types

| cbitset               | Type definition           | Used to represent...                 |
|:----------------------|:--------------------------|:-------------------------------------|
| `cbitset_t`           | `struct { ... }`          | The cbitset type                     |
| `cbitset_iter_t`      | `struct { ... }`          | The cbitset iterator type            |

## Header file

All cstr definitions and prototypes may be included in your C source file by including a single header file.

```c
#include "stc/cbitset.h"
```
## Methods

```c
cbitset_t        cbitset_init(void);
cbitset_t        cbitset_with_size(size_t size, bool value);
cbitset_t        cbitset_from_str(const char* str);
void             cbitset_resize(cbitset_t* self, size_t size, bool value);

void             cbitset_del(cbitset_t* self);

cbitset_t        cbitset_intersect(cbitset_t s1, cbitset_t s2);
cbitset_t        cbitset_union(cbitset_t s1, cbitset_t s2);
cbitset_t        cbitset_xor(cbitset_t s1, cbitset_t s2);
cbitset_t        cbitset_not(cbitset_t s1);

cbitset_t*       cbitset_take(cbitset_t* self, cbitset_t other);
cbitset_t*       cbitset_assign(cbitset_t* self, cbitset_t other);
cbitset_t        cbitset_clone(cbitset_t other);
cbitset_t        cbitset_move(cbitset_t* self);

size_t           cbitset_size(cbitset_t set);
size_t           cbitset_count(cbitset_t set);
bool             cbitset_is_disjoint(cbitset_t set, cbitset_t other);
bool             cbitset_is_subset(cbitset_t set, cbitset_t other);
bool             cbitset_is_superset(cbitset_t set, cbitset_t other);
char*            cbitset_to_str(cbitset_t set, char* str, size_t start, intptr_t stop);

void             cbitset_set(cbitset_t *self, size_t i);
void             cbitset_reset(cbitset_t *self, size_t i);
void             cbitset_set_value(cbitset_t *self, size_t i, bool value);
void             cbitset_flip(cbitset_t *self, size_t i);
bool             cbitset_test(cbitset_t set, size_t i);
void             cbitset_set_all(cbitset_t *self, bool value);
void             cbitset_set_all_64(cbitset_t *self, uint64_t pattern);
void             cbitset_flip_all(cbitset_t *self);

void             cbitset_intersect_with(cbitset_t *self, cbitset_t other);
void             cbitset_union_with(cbitset_t *self, cbitset_t other);
void             cbitset_xor_with(cbitset_t *self, cbitset_t other);

cbitset_iter_t   cbitset_begin(cbitset_t* self);
cbitset_iter_t   cbitset_end(cbitset_t* self);
void             cbitset_next(cbitset_iter_t* it);
bool             cbitset_itval(cbitset_iter_t it);
```

## Example
```c
#include <stdio.h>
#include "stc/cbitset.h"

static inline cbitset_t sieveOfEratosthenes(size_t n)
{
    cbitset_t pbits = cbitset_with_size(n + 1, true);
    cbitset_reset(&pbits, 0);
    cbitset_reset(&pbits, 1);

    c_forrange (i, size_t, 2, n+1) {
        // If pbits[i] is not changed, then it is a prime
        if (cbitset_test(pbits, i) && i*i <= n) {
            c_forrange (j, size_t, i*i, n+1, i) {
                cbitset_reset(&pbits, j);
            }
        }
    }
    return pbits;
}

int main(void)
{
    int n = 100000000;
    printf("computing prime numbers up to %u\n", n);

    cbitset_t primes = sieveOfEratosthenes(n);
    puts("done");

    size_t np = cbitset_count(primes);
    printf("number of primes: %zu\n", np);

    printf("2 ");
    c_forrange (i, int, 3, 1001, 2) {
       if (cbitset_test(primes, i)) printf("%d ", i);
    }
    puts("");
    cbitset_del(&primes);
}
```
Output:
```
computing prime numbers up to 100000000
done
number of primes: 5761455
2 3 5 7 11 13 17 19 23 29 31 37 41 43 47 53 59 61 67 71 73 79 83 89 97 101 103 107 109 113 127 131 137 139 149 151 157 163 167 173 179 181 191 193 197 199 211 223 227 229 233 239 241 251 257 263 269 271 277 281 283 293 307 311 313 317 331 337 347 349 353 359 367 373 379 383 389 397 401 409 419 421 431 433 439 443 449 457 461 463 467 479 487 491 499 503 509 521 523 541 547 557 563 569 571 577 587 593 599 601 607 613 617 619 631 641 643 647 653 659 661 673 677 683 691 701 709 719 727 733 739 743 751 757 761 769 773 787 797 809 811 821 823 827 829 839 853 857 859 863 877 881 883 887 907 911 919 929 937 941 947 953 967 971 977 983 991 997
```
