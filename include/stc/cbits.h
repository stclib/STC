/* MIT License
 *
 * Copyright (c) 2022 Tyge Løvset, NORCE, www.norceresearch.no
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
#ifndef CBITS_H_INCLUDED
#define CBITS_H_INCLUDED
/*
Similar to boost::dynamic_bitset / std::bitset

#include <stdio.h>
#include "cbits.h"

int main() {
    c_autovar (cbits bset = cbits_with_size(23, true), cbits_drop(&bset))
    {
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
    }
}
*/
#include "ccommon.h"
#include <stdlib.h>
#include <string.h>

struct cbits {
    uint64_t *data64;
    size_t size;
} typedef cbits;

STC_API cbits       cbits_from_n(const char* str, size_t n);
STC_API cbits       cbits_with_size(size_t size, bool value);
STC_API cbits       cbits_with_values(size_t size, uint64_t pattern);
STC_API char*       cbits_to_str(cbits set, char* str, size_t start, intptr_t stop);
STC_API cbits       cbits_clone(cbits other);
STC_API void        cbits_resize(cbits* self, size_t size, bool value);
STC_API cbits*      cbits_copy(cbits* self, cbits other);
STC_API size_t      cbits_count(cbits set);
STC_API bool        cbits_subset_of(cbits set, cbits other);
STC_API bool        cbits_disjoint(cbits set, cbits other);

STC_INLINE cbits    cbits_init() { return c_make(cbits){NULL, 0}; }
STC_INLINE cbits    cbits_from(const char* s) { return cbits_from_n(s, strlen(s)); }
STC_INLINE void     cbits_clear(cbits* self) { self->size = 0; }
STC_INLINE void     cbits_drop(cbits* self) { c_free(self->data64); }
STC_INLINE size_t   cbits_size(cbits set) { return set.size; }

#define _cbits_bit(i) ((uint64_t)1 << ((i) & 63))
#define _cbits_words(n) (((n) + 63) >> 6)
#define _cbits_bytes(n) (_cbits_words(n) * sizeof(uint64_t))

#define cbits_new(literal) \
    cbits_from_n(literal, c_strlen_lit(literal))

STC_INLINE cbits* cbits_take(cbits* self, cbits other) {
    if (self->data64 != other.data64) {cbits_drop(self); *self = other;}
    return self;
}

STC_INLINE cbits cbits_move(cbits* self) {
    cbits tmp = *self; self->data64 = NULL, self->size = 0;
    return tmp;
}

STC_INLINE bool cbits_test(cbits set, const size_t i) {
    return (set.data64[i >> 6] & _cbits_bit(i)) != 0;
}

STC_INLINE bool cbits_at(cbits set, const size_t i) {
    return (set.data64[i >> 6] & _cbits_bit(i)) != 0;
}

STC_INLINE void cbits_set(cbits *self, const size_t i) {
    self->data64[i >> 6] |= _cbits_bit(i);
}

STC_INLINE void cbits_reset(cbits *self, const size_t i) {
    self->data64[i >> 6] &= ~_cbits_bit(i);
}

STC_INLINE void cbits_set_value(cbits *self, const size_t i, const bool value) {
    self->data64[i >> 6] ^= ((uint64_t)-(int)value ^ self->data64[i >> 6]) & _cbits_bit(i);
}

STC_INLINE void cbits_flip(cbits *self, const size_t i) {
    self->data64[i >> 6] ^= _cbits_bit(i);
}

STC_INLINE void cbits_set_all(cbits *self, const bool value) {
    memset(self->data64, -(int)value, _cbits_bytes(self->size));
}

STC_INLINE void cbits_set_values(cbits *self, const uint64_t pattern) {
    size_t n = _cbits_words(self->size);
    for (size_t i=0; i<n; ++i) self->data64[i] = pattern;
}

STC_INLINE void cbits_flip_all(cbits *self) {
    size_t n = _cbits_words(self->size);
    for (size_t i=0; i<n; ++i) self->data64[i] ^= ~(uint64_t)0;
}

/* Intersection */
STC_INLINE void cbits_intersect(cbits *self, cbits other) {
    assert(self->size == other.size);
    size_t n = _cbits_words(self->size);
    for (size_t i=0; i<n; ++i) self->data64[i] &= other.data64[i];
}
/* Union */
STC_INLINE void cbits_union(cbits *self, cbits other) {
    assert(self->size == other.size);
    size_t n = _cbits_words(self->size);
    for (size_t i=0; i<n; ++i) self->data64[i] |= other.data64[i];
}
/* Exclusive disjunction */
STC_INLINE void cbits_xor(cbits *self, cbits other) {
    assert(self->size == other.size);
    size_t n = _cbits_words(self->size);
    for (size_t i=0; i<n; ++i) self->data64[i] ^= other.data64[i];
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

#if defined(_i_implement)

STC_DEF cbits* cbits_copy(cbits* self, cbits other) {
    if (self->data64 == other.data64) return self;
    if (self->size != other.size) return cbits_take(self, cbits_clone(other));
    memcpy(self->data64, other.data64, _cbits_bytes(other.size));
    return self;
}

STC_DEF void cbits_resize(cbits* self, const size_t size, const bool value) {
    const size_t new_n = _cbits_words(size), osize = self->size, old_n = _cbits_words(osize);
    self->data64 = (uint64_t *) c_realloc(self->data64, new_n * 8);
    self->size = size;
    if (new_n >= old_n) {
        memset(self->data64 + old_n, -(int)value, (new_n - old_n) * 8);
        if (old_n > 0) {
            uint64_t m = _cbits_bit(osize) - 1; /* mask */
            value ? (self->data64[old_n - 1] |= ~m) : (self->data64[old_n - 1] &= m);
        }
    }
}

STC_DEF cbits cbits_with_size(const size_t size, const bool value) {
    cbits set = {(uint64_t *) c_malloc(_cbits_bytes(size)), size};
    cbits_set_all(&set, value);
    return set;
}
STC_DEF cbits cbits_with_values(const size_t size, const uint64_t pattern) {
    cbits set = {(uint64_t *) c_malloc(_cbits_bytes(size)), size};
    cbits_set_values(&set, pattern);
    return set;
}
STC_DEF cbits cbits_from_n(const char* str, const size_t n) {
    cbits set = cbits_with_size(n, false);
    for (size_t i=0; i<set.size; ++i)
        if (str[i] == '1') cbits_set(&set, i);
    return set;
}
STC_DEF char* cbits_to_str(cbits set, char* out, size_t start, intptr_t stop) {
    if (stop < 0) stop = set.size;
    memset(out, '0', stop - start);
    for (intptr_t i=start; i<stop; ++i) 
        if (cbits_test(set, i)) out[i - start] = '1';
    out[stop - start] = '\0';
    return out;
}
STC_DEF cbits cbits_clone(cbits other) {
    const size_t bytes = _cbits_bytes(other.size);
    cbits set = {(uint64_t *) memcpy(c_malloc(bytes), other.data64, bytes), other.size};
    return set;
}
STC_DEF size_t cbits_count(cbits s) {
    size_t count = 0, n = s.size >> 6;
    for (size_t i = 0; i < n; ++i) count += cpopcount64(s.data64[i]);
    if (s.size & 63) count += cpopcount64(s.data64[n] & (_cbits_bit(s.size) - 1));
    return count;
}

#define _cbits_OPR(OPR, VAL) \
    assert(s.size == other.size); \
    const size_t n = s.size >> 6; \
    for (size_t i = 0; i < n; ++i) \
        if ((s.data64[i] OPR other.data64[i]) != VAL) \
            return false; \
    if (!(s.size & 63)) return true; \
    const uint64_t i = n, m = _cbits_bit(s.size) - 1; \
    return ((s.data64[i] OPR other.data64[i]) & m) == (VAL & m)

STC_DEF bool cbits_subset_of(cbits s, cbits other) { _cbits_OPR(|, s.data64[i]); }
STC_DEF bool cbits_disjoint(cbits s, cbits other) { _cbits_OPR(&, 0); }

#endif
#endif
#undef i_opt
