/* MIT License
 *
 * Copyright (c) 2020 Tyge Løvset, NORCE, www.norceresearch.no
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/*
Similar to boost::dynamic_bitset / std::bitset

#include <stdio.h>
#include "cbitset.h"

int main() {
    cbitset_t set = cbitset_make(23, true);
    cbitset_reset(&set, 9);
    cbitset_resize(&set, 43, false);
    printf("%4zu: ", set.size);for (int i=0; i<set.size; ++i) printf("%d", cbitset_value(&set, i));puts("");
    cbitset_set(&set, 28);
    cbitset_resize(&set, 77, true);
    cbitset_resize(&set, 93, false);
    cbitset_resize(&set, 102, true);
    cbitset_setTo(&set, 99, false);
    printf("%4zu: ", set.size);for (int i=0; i<set.size; ++i) printf("%d", cbitset_value(&set, i));puts("");
    cbitset_destroy(&set);
}
*/
#ifndef CBITSET__H__
#define CBITSET__H__

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "cdefs.h"

typedef struct { uint64_t* _arr; size_t size; } cbitset_t;

#define cbitset_init {NULL, 0}

STC_API void cbitset_resize(cbitset_t* self, size_t size, bool value);
STC_API size_t cbitset_count(cbitset_t set);

STC_INLINE void cbitset_set_all(cbitset_t *self, bool value);

STC_INLINE cbitset_t cbitset_make(size_t size, bool value) {
    cbitset_t set = {(uint64_t *) malloc(((size + 63) >> 6) * 8), size};
    cbitset_set_all(&set, value);
    return set;
}
STC_INLINE cbitset_t cbitset_make_copy(cbitset_t other) {
    size_t n = (other.size + 63) >> 6;
    cbitset_t set = {(uint64_t *) memcpy(malloc(n * 8), other._arr, n * 8), other.size};
    return set;
}
STC_INLINE void cbitset_destroy(cbitset_t* self) {
    free(self->_arr);
}

STC_INLINE size_t cbitset_size(cbitset_t set) {return set.size;}

STC_INLINE void cbitset_set(cbitset_t *self, size_t i) {
    self->_arr[i >> 6] |= 1ull << (i & 63);
}
STC_INLINE void cbitset_reset(cbitset_t *self, size_t i) {
    self->_arr[i >> 6] &= ~(1ull << (i & 63));
}
STC_INLINE void cbitset_set_to(cbitset_t *self, size_t i, bool value) {
    value ? cbitset_set(self, i) : cbitset_reset(self, i);
}
STC_INLINE void cbitset_flip(cbitset_t *self, size_t i) {
    self->_arr[i >> 6] ^= 1ull << (i & 63);
}
STC_INLINE bool cbitset_test(cbitset_t set, size_t i) {
    return (set._arr[i >> 6] & (1ull << (i & 63))) != 0;
}

STC_INLINE void cbitset_set_all(cbitset_t *self, bool value) {
    memset(self->_arr, value ? 0xff : 0x0, ((self->size + 63) >> 6) * 8);
}
STC_INLINE void cbitset_set_all_64(cbitset_t *self, uint64_t pattern) {
    size_t n = (self->size + 63) >> 6;
    for (size_t i=0; i<n; ++i) self->_arr[i] = pattern;
}
STC_INLINE void cbitset_flip_all(cbitset_t *self) {
    size_t n = (self->size + 63) >> 6;
    for (size_t i=0; i<n; ++i) self->_arr[i] ^= ~0ull;
}
/* Intersection */
STC_INLINE void cbitset_set_and(cbitset_t *self, cbitset_t other) {
    assert(self->size == other.size);
    size_t n = (self->size + 63) >> 6;
    for (size_t i=0; i<n; ++i) self->_arr[i] &= other._arr[i];
}
/* Union */
STC_INLINE void cbitset_set_or(cbitset_t *self, cbitset_t other) {
    assert(self->size == other.size);
    size_t n = (self->size + 63) >> 6;
    for (size_t i=0; i<n; ++i) self->_arr[i] |= other._arr[i];
}
/* Exclusive disjunction */
STC_INLINE void cbitset_set_xor(cbitset_t *self, cbitset_t other) {
    assert(self->size == other.size);
    size_t n = (self->size + 63) >> 6;
    for (size_t i=0; i<n; ++i) self->_arr[i] ^= other._arr[i];
}

STC_INLINE cbitset_t cbitset_and(cbitset_t s1, cbitset_t s2) {
    cbitset_t set = cbitset_make_copy(s1);
    cbitset_set_and(&set, s2); return set;
}
STC_INLINE cbitset_t cbitset_or(cbitset_t s1, cbitset_t s2) {
    cbitset_t set = cbitset_make_copy(s1);
    cbitset_set_or(&set, s2); return set;
}
STC_INLINE cbitset_t cbitset_xor(cbitset_t s1, cbitset_t s2) {
    cbitset_t set = cbitset_make_copy(s1);
    cbitset_set_xor(&set, s2); return set;
}
STC_INLINE cbitset_t cbitset_not(cbitset_t s1) {
    cbitset_t set = cbitset_make_copy(s1);
    cbitset_flip_all(&set); return set;
}

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)

STC_API void cbitset_resize(cbitset_t* self, size_t size, bool value) {
    size_t new_n = (size + 63) >> 6, osize = self->size, old_n = (osize + 63) >> 6;
    self->_arr = (uint64_t *) realloc(self->_arr, new_n * 8);
    self->size = size;
    if (new_n >= old_n) {
        memset(self->_arr + old_n, value ? 0xff : 0x0, (new_n - old_n) * 8);
        if (old_n > 0) {
            uint64_t mask = (1ull << (osize & 63)) - 1;
            value ? (self->_arr[old_n - 1] |= ~mask) : (self->_arr[old_n - 1] &= mask);
        }
    }
}

#if defined(__GNUC__)
    #define c_popcount64(x) __builtin_popcountll(x)
#elif defined(_MSC_VER)
    #define c_popcount64(x) _mm_popcnt_u64(x)
#else
/* http://en.wikipedia.org/wiki/Hamming_weight#Efficient_implementation */
static inline uint64_t c_popcount64(uint64_t x) {
    uint64_t m1 = 0x5555555555555555ll;
    uint64_t m2 = 0x3333333333333333ll;
    uint64_t m4 = 0x0F0F0F0F0F0F0F0Fll;
    uint64_t h01 = 0x0101010101010101ll;
    x -= (x >> 1) & m1;
    x = (x & m2) + ((x >> 2) & m2);
    x = (x + (x >> 4)) & m4;
    return (x * h01) >> 56;
}
#endif

STC_API size_t cbitset_count(cbitset_t set) {
    size_t count = 0, n = (set.size + 63) >> 6;
    if (set.size > 0) {
        --n; for (size_t i=0; i<n; ++i) count += c_popcount64(set._arr[i]);
        count += c_popcount64(set._arr[n] & ((1ull << (set.size & 63)) - 1));
    }
    return count;
}

#endif

#endif
