/* MIT License
 *
 * Copyright (c) 2021 Tyge Løvset, NORCE, www.norceresearch.no
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
#ifndef CBITSET__H__
#define CBITSET__H__

/*
Similar to boost::dynamic_bitset / std::bitset

#include <stdio.h>
#include "cbits.h"

int main() {
    cbits_t bset = cbits_with_size(23, true);
    cbits_reset(&bset, 9);
    cbits_resize(&bset, 43, false);

    printf("%4zu: ", bset.size);
    c_forrange (i, bset.size)
        printf("%d", cbits_at(&bset, i));
    puts("");
    cbits_set(&bset, 28);
    cbits_resize(&bset, 77, true);
    cbits_resize(&bset, 93, false);
    cbits_resize(&bset, 102, true);
    cbits_set_value(&bset, 99, false);

    printf("%4zu: ", bset.size);
    c_forrange (i, bset.size)
        printf("%d", cbits_at(&bset, i));
    puts("");
    cbits_del(&bset);
}
*/
#include <stdlib.h>
#include <string.h>
#include "ccommon.h"

typedef struct cbits { uint64_t* _arr; size_t size; } cbits_t, cbits;

STC_API cbits_t      cbits_with_size(size_t size, bool value);
STC_API cbits_t      cbits_from_str(const char* str);
STC_API char*        cbits_to_str(cbits_t set, char* str, size_t start, intptr_t stop);
STC_API cbits_t      cbits_clone(cbits_t other);
STC_API void         cbits_resize(cbits_t* self, size_t size, bool value);
STC_API size_t       cbits_count(cbits_t set);
STC_API bool         cbits_is_disjoint(cbits_t set, cbits_t other);
STC_API bool         cbits_is_subset(cbits_t set, cbits_t other);
STC_API bool         cbits_is_superset(cbits_t set, cbits_t other);

STC_INLINE size_t    cbits_size(cbits_t set) {return set.size;}
#define              cbits_inits {NULL, 0}

STC_INLINE cbits_t cbits_init() {
    cbits_t bs = cbits_inits; return bs;
}
STC_INLINE void cbits_del(cbits_t* self) {
    c_free(self->_arr);
}

STC_DEF cbits_t* cbits_take(cbits_t* self, cbits_t other) {
    if (self->_arr != other._arr) cbits_del(self), *self = other;
    return self;
}
STC_DEF cbits_t* cbits_assign(cbits_t* self, cbits_t other) {
    if (self->_arr != other._arr) cbits_del(self), *self = cbits_clone(other);
    return self;
}
STC_INLINE cbits_t cbits_move(cbits_t* self) {
    cbits_t tmp = *self; self->_arr = NULL, self->size = 0;
    return tmp;
}

STC_INLINE bool cbits_test(cbits_t set, size_t i) {
    return (set._arr[i >> 6] & (1ull << (i & 63))) != 0;
}
STC_INLINE bool cbits_at(cbits_t set, size_t i) {
    return (set._arr[i >> 6] & (1ull << (i & 63))) != 0;
}

STC_INLINE void cbits_set(cbits_t *self, size_t i) {
    self->_arr[i >> 6] |= 1ull << (i & 63);
}
STC_INLINE void cbits_reset(cbits_t *self, size_t i) {
    self->_arr[i >> 6] &= ~(1ull << (i & 63));
}
STC_INLINE void cbits_set_value(cbits_t *self, size_t i, bool value) {
    self->_arr[i >> 6] ^= (-(uint64_t)value ^ self->_arr[i >> 6]) & 1ull << (i & 63);
}
STC_INLINE void cbits_flip(cbits_t *self, size_t i) {
    self->_arr[i >> 6] ^= 1ull << (i & 63);
}
STC_INLINE void cbits_set_all(cbits_t *self, bool value) {
    memset(self->_arr, -value, ((self->size + 63) >> 6) * 8);
}
STC_INLINE void cbits_set_all64(cbits_t *self, uint64_t pattern) {
    size_t n = (self->size + 63) >> 6;
    for (size_t i=0; i<n; ++i) self->_arr[i] = pattern;
}
STC_INLINE void cbits_flip_all(cbits_t *self) {
    size_t n = (self->size + 63) >> 6;
    for (size_t i=0; i<n; ++i) self->_arr[i] ^= ~0ull;
}

/* Intersection */
STC_INLINE void cbits_intersect_with(cbits_t *self, cbits_t other) {
    assert(self->size == other.size);
    size_t n = (self->size + 63) >> 6;
    for (size_t i=0; i<n; ++i) self->_arr[i] &= other._arr[i];
}
/* Union */
STC_INLINE void cbits_union_with(cbits_t *self, cbits_t other) {
    assert(self->size == other.size);
    size_t n = (self->size + 63) >> 6;
    for (size_t i=0; i<n; ++i) self->_arr[i] |= other._arr[i];
}
/* Exclusive disjunction */
STC_INLINE void cbits_xor_with(cbits_t *self, cbits_t other) {
    assert(self->size == other.size);
    size_t n = (self->size + 63) >> 6;
    for (size_t i=0; i<n; ++i) self->_arr[i] ^= other._arr[i];
}

STC_INLINE cbits_t cbits_intersect(cbits_t s1, cbits_t s2) {
    cbits_t set = cbits_clone(s1);
    cbits_intersect_with(&set, s2); return set;
}
STC_INLINE cbits_t cbits_union(cbits_t s1, cbits_t s2) {
    cbits_t set = cbits_clone(s1);
    cbits_union_with(&set, s2); return set;
}
STC_INLINE cbits_t cbits_xor(cbits_t s1, cbits_t s2) {
    cbits_t set = cbits_clone(s1);
    cbits_xor_with(&set, s2); return set;
}
STC_INLINE cbits_t cbits_not(cbits_t s1) {
    cbits_t set = cbits_clone(s1);
    cbits_flip_all(&set); return set;
}

typedef struct {
    cbits_t *_bs;
    size_t ref;
} cbits_iter_t;

STC_INLINE cbits_iter_t
cbits_begin(cbits_t* self) {
    cbits_iter_t it = {self, 0}; return it;
}
STC_INLINE cbits_iter_t
cbits_end(cbits_t* self) {
    cbits_iter_t it = {self, self->size}; return it;
}
STC_INLINE void
cbits_next(cbits_iter_t* it) {++it->ref;}

STC_INLINE bool cbits_itval(cbits_iter_t it) {
    return cbits_test(*it._bs, it.ref);
}

#if defined(__GNUC__) || defined(__clang__)
    STC_INLINE uint64_t cpopcount64(uint64_t x) {return __builtin_popcountll(x);}
#elif defined(_MSC_VER) && defined(_WIN64)
    #include <intrin.h>
    STC_INLINE uint64_t cpopcount64(uint64_t x) {return __popcnt64(x);}
#else
    STC_INLINE uint64_t cpopcount64(uint64_t x) { /* http://en.wikipedia.org/wiki/Hamming_weight */
        x -= (x >> 1) & 0x5555555555555555;
        x = (x & 0x3333333333333333) + ((x >> 2) & 0x3333333333333333);
        x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0f;
        return (x * 0x0101010101010101) >> 56;
    }
#endif

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)

STC_DEF void cbits_resize(cbits_t* self, size_t size, bool value) {
    size_t new_n = (size + 63) >> 6, osize = self->size, old_n = (osize + 63) >> 6;
    self->_arr = (uint64_t *) c_realloc(self->_arr, new_n * 8);
    self->size = size;
    if (new_n >= old_n) {
        memset(self->_arr + old_n, -value, (new_n - old_n) * 8);
        if (old_n > 0) {
            uint64_t m = (1ull << (osize & 63)) - 1; /* mask */
            value ? (self->_arr[old_n - 1] |= ~m) : (self->_arr[old_n - 1] &= m);
        }
    }
}

STC_DEF cbits_t cbits_with_size(size_t size, bool value) {
    cbits_t set = {(uint64_t *) c_malloc(((size + 63) >> 6) * 8), size};
    cbits_set_all(&set, value);
    return set;
}
STC_DEF cbits_t cbits_from_str(const char* str) {
    const char* p = str; while (*p) ++p;
    cbits_t set = cbits_with_size(p - str, false);
    for (size_t i=0; i<set.size; ++i) if (str[i] == '1') cbits_set(&set, i);
    return set;
}
STC_DEF char* cbits_to_str(cbits_t set, char* out, size_t start, intptr_t stop) {
    size_t end = stop < 0 ? set.size : stop;
    for (size_t i=start; i<end; ++i) out[i] = cbits_test(set, i) ? '1' : '0';
    out[end] = '\0'; return out;
}
STC_DEF cbits_t cbits_clone(cbits_t other) {
    size_t bytes = ((other.size + 63) >> 6) * 8;
    cbits_t set = {(uint64_t *) memcpy(c_malloc(bytes), other._arr, bytes), other.size};
    return set;
}
STC_DEF size_t cbits_count(cbits_t s) {
    size_t count = 0, n = ((s.size + 63) >> 6) - 1;
    if (s.size > 0) {
        for (size_t i=0; i<n; ++i) count += cpopcount64(s._arr[i]);
        count += cpopcount64(s._arr[n] & ((1ull << (s.size & 63)) - 1));
    }
    return count;
}

#define _cbits_SETOP(OPR) \
    assert(s.size == other.size); \
    if (s.size == 0) return false; /* ? */ \
    size_t n = ((s.size + 63) >> 6) - 1; \
    for (size_t i=0; i<n; ++i) \
        if ((s._arr[i] OPR other._arr[i]) != s._arr[i]) \
            return false; \
    uint64_t m = (1ull << (s.size & 63)) - 1, last = s._arr[n] & m; \
    return (last OPR (other._arr[n] & m)) == last

STC_DEF bool cbits_is_disjoint(cbits_t s, cbits_t other) { _cbits_SETOP(^); }
STC_DEF bool cbits_is_subset(cbits_t s, cbits_t other) { _cbits_SETOP(|); }
STC_DEF bool cbits_is_superset(cbits_t s, cbits_t other) { _cbits_SETOP(&); }

#endif
#endif
