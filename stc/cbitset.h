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
    CBitset set = cbitset_make(23, true);
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

typedef struct { uint64_t* _arr; size_t size; } CBitset;

STC_INLINE void cbitset_setAll(CBitset *self, bool value);

STC_INLINE CBitset cbitset_make(size_t size, bool value) {
    CBitset set = {(uint64_t *) malloc(((size + 63) >> 6) * 8), size};
    cbitset_setAll(&set, value);
    return set;
}
STC_INLINE CBitset cbitset_from(CBitset other) {
    size_t n = (other.size + 63) >> 6;
    CBitset set = {(uint64_t *) memcpy(malloc(n * 8), other._arr, n * 8), other.size};
    return set;
}
STC_INLINE void cbitset_destroy(CBitset* self) {
    free(self->_arr);
}

STC_INLINE void cbitset_resize(CBitset* self, size_t size, bool value) {
    size_t newsz = (size + 63) >> 6, oldsz = (self->size + 63) >> 6;
    self->_arr = (uint64_t *) realloc(self->_arr, newsz * 8);
    memset(self->_arr + oldsz, value ? 0xff : 0x0, (newsz - oldsz) * 8);
    if (self->size & 63) {
        size_t idx = (self->size - 1) >> 6; uint64_t bits = (1ull << (self->size & 63)) - 1;
        value ? (self->_arr[idx] |= ~bits) : (self->_arr[idx] &= bits);
    }
    self->size = size;
}
STC_INLINE size_t cbitset_size(CBitset set) {return set.size;}

STC_INLINE void cbitset_set(CBitset *self, size_t i) {self->_arr[i >> 6] |= 1ull << (i & 63);}
STC_INLINE void cbitset_reset(CBitset *self, size_t i) {self->_arr[i >> 6] &= ~(1ull << (i & 63));}
STC_INLINE void cbitset_setTo(CBitset *self, size_t i, bool value) {value ? cbitset_set(self, i) : cbitset_reset(self, i);}
STC_INLINE void cbitset_flip(CBitset *self, size_t i) {self->_arr[i >> 6] ^= 1ull << (i & 63);}
STC_INLINE bool cbitset_test(CBitset set, size_t i) {return (set._arr[i >> 6] & (1ull << (i & 63))) != 0;}

STC_INLINE void cbitset_setAll(CBitset *self, bool value) {
    memset(self->_arr, value ? 0xff : 0x0, ((self->size + 63) >> 6) * 8);
}
STC_INLINE void cbitset_setAll64(CBitset *self, uint64_t pattern) {
    size_t n = (self->size + 63) >> 6;
    for (size_t i=0; i<n; ++i) self->_arr[i] = pattern;
}
STC_INLINE void cbitset_flipAll(CBitset *self) {
    size_t n = (self->size + 63) >> 6;
    for (size_t i=0; i<n; ++i) self->_arr[i] ^= ~0ull;
}
/* Intersection */
STC_INLINE void cbitset_setAnd(CBitset *self, CBitset other) {
    assert(self->size == other.size);
    size_t n = (self->size + 63) >> 6;
    for (size_t i=0; i<n; ++i) self->_arr[i] &= other._arr[i];
}
/* Union */
STC_INLINE void cbitset_setOr(CBitset *self, CBitset other) {
    assert(self->size == other.size);
    size_t n = (self->size + 63) >> 6;
    for (size_t i=0; i<n; ++i) self->_arr[i] |= other._arr[i];
}
/* Exclusive disjunction */
STC_INLINE void cbitset_setXor(CBitset *self, CBitset other) {
    assert(self->size == other.size);
    size_t n = (self->size + 63) >> 6;
    for (size_t i=0; i<n; ++i) self->_arr[i] ^= other._arr[i];
}

STC_INLINE CBitset cbitset_and(CBitset s1, CBitset s2) {
    CBitset set = cbitset_from(s1);
    cbitset_setAnd(&set, s2); return set;
}
STC_INLINE CBitset cbitset_or(CBitset s1, CBitset s2) {
    CBitset set = cbitset_from(s1);
    cbitset_setOr(&set, s2); return set;
}
STC_INLINE CBitset cbitset_xor(CBitset s1, CBitset s2) {
    CBitset set = cbitset_from(s1);
    cbitset_setXor(&set, s2); return set;
}
STC_INLINE CBitset cbitset_not(CBitset s1) {
    CBitset set = cbitset_from(s1);
    cbitset_flipAll(&set); return set;
}

#endif
