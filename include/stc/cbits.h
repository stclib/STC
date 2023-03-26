/* MIT License
 *
 * Copyright (c) 2023 Tyge Løvset
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
#include "cbits.h"

int main() {
    cbits bset = cbits_with_size(23, true);
    cbits_reset(&bset, 9);
    cbits_resize(&bset, 43, false);

    printf("%4zu: ", cbits_size(&bset));
    c_forrange (i, cbits_size(&bset))
        printf("%d", cbits_at(&bset, i));
    puts("");
    cbits_set(&bset, 28);
    cbits_resize(&bset, 77, true);
    cbits_resize(&bset, 93, false);
    cbits_resize(&bset, 102, true);
    cbits_set_value(&bset, 99, false);

    printf("%4zu: ", cbits_size(&bset));
    c_forrange (i, cbits_size(&bset))
        printf("%d", cbits_at(&bset, i));
    puts("");

    cbits_drop(&bset);
}
*/
#ifndef CBITS_H_INCLUDED
#include "ccommon.h"
#include <stdlib.h>
#include <string.h>

#ifndef i_ssize
#define i_ssize intptr_t
#endif
#define _cbits_bit(i) ((uint64_t)1 << ((i) & 63))
#define _cbits_words(n) (i_ssize)(((n) + 63)>>6)
#define _cbits_bytes(n) (_cbits_words(n) * c_sizeof(uint64_t))

#if defined(__GNUC__)
    STC_INLINE int cpopcount64(uint64_t x) {return __builtin_popcountll(x);}
    #ifndef __clang__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wstringop-overflow="
    #pragma GCC diagnostic ignored "-Walloc-size-larger-than="
    #endif
#elif defined(_MSC_VER) && defined(_WIN64)
    #include <intrin.h>
    STC_INLINE int cpopcount64(uint64_t x) {return (int)__popcnt64(x);}
#else
    STC_INLINE int cpopcount64(uint64_t x) { /* http://en.wikipedia.org/wiki/Hamming_weight */
        x -= (x >> 1) & 0x5555555555555555;
        x = (x & 0x3333333333333333) + ((x >> 2) & 0x3333333333333333);
        x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0f;
        return (int)((x * 0x0101010101010101) >> 56);
    }
#endif

STC_INLINE i_ssize _cbits_count(const uint64_t* set, const i_ssize sz) {
    const i_ssize n = sz>>6;
    i_ssize count = 0;
    for (i_ssize i = 0; i < n; ++i)
        count += cpopcount64(set[i]);
    if (sz & 63)
        count += cpopcount64(set[n] & (_cbits_bit(sz) - 1));
    return count;
}

STC_INLINE char* _cbits_to_str(const uint64_t* set, const i_ssize sz, 
                               char* out, i_ssize start, i_ssize stop) {
    if (stop > sz) stop = sz;
    assert(start <= stop);

    c_memset(out, '0', stop - start);
    for (i_ssize i = start; i < stop; ++i) 
        if ((set[i>>6] & _cbits_bit(i)) != 0)
            out[i - start] = '1';
    out[stop - start] = '\0';
    return out;
}

#define _cbits_OPR(OPR, VAL) \
    const i_ssize n = sz>>6; \
    for (i_ssize i = 0; i < n; ++i) \
        if ((set[i] OPR other[i]) != VAL) \
            return false; \
    if (!(sz & 63)) \
        return true; \
    const uint64_t i = (uint64_t)n, m = _cbits_bit(sz) - 1; \
    return ((set[i] OPR other[i]) & m) == (VAL & m)

STC_INLINE bool _cbits_subset_of(const uint64_t* set, const uint64_t* other, const i_ssize sz)
    { _cbits_OPR(|, set[i]); }

STC_INLINE bool _cbits_disjoint(const uint64_t* set, const uint64_t* other, const i_ssize sz)
    { _cbits_OPR(&, 0); }

#endif // CBITS_H_INCLUDED

#define _i_memb(name) c_PASTE(i_type, name)

#if !defined i_capacity // DYNAMIC SIZE BITARRAY

#define _i_assert(x) assert(x)
#define i_type cbits

struct { uint64_t *data64; i_ssize _size; } typedef i_type;

STC_INLINE cbits   cbits_init(void) { return c_LITERAL(cbits){NULL}; }
STC_INLINE void    cbits_create(cbits* self) { self->data64 = NULL; self->_size = 0; }
STC_INLINE void    cbits_drop(cbits* self) { c_free(self->data64); }
STC_INLINE i_ssize cbits_size(const cbits* self) { return self->_size; }

STC_INLINE cbits* cbits_take(cbits* self, cbits other) {
    if (self->data64 != other.data64) {
        cbits_drop(self);
        *self = other;
    }
    return self;
}

STC_INLINE cbits cbits_clone(cbits other) {
    const i_ssize bytes = _cbits_bytes(other._size);
    cbits set = {(uint64_t *)c_memcpy(c_malloc(bytes), other.data64, bytes), other._size};
    return set;
}

STC_INLINE cbits* cbits_copy(cbits* self, const cbits* other) {
    if (self->data64 == other->data64)
        return self;
    if (self->_size != other->_size)
        return cbits_take(self, cbits_clone(*other));
    c_memcpy(self->data64, other->data64, _cbits_bytes(other->_size));
    return self;
}

STC_INLINE void cbits_resize(cbits* self, const i_ssize size, const bool value) {
    const i_ssize new_n = _cbits_words(size), osize = self->_size, old_n = _cbits_words(osize);
    self->data64 = (uint64_t *)c_realloc(self->data64, new_n*8);
    self->_size = size;
    if (new_n >= old_n) {
        c_memset(self->data64 + old_n, -(int)value, (new_n - old_n)*8);
        if (old_n > 0) {
            uint64_t m = _cbits_bit(osize) - 1; /* mask */
            value ? (self->data64[old_n - 1] |= ~m) 
                  : (self->data64[old_n - 1] &= m);
        }
    }
}

STC_INLINE void cbits_set_all(cbits *self, const bool value);
STC_INLINE void cbits_set_pattern(cbits *self, const uint64_t pattern);

STC_INLINE cbits cbits_move(cbits* self) {
    cbits tmp = *self;
    self->data64 = NULL, self->_size = 0;
    return tmp;
}

STC_INLINE cbits cbits_with_size(const i_ssize size, const bool value) {
    cbits set = {(uint64_t *)c_malloc(_cbits_bytes(size)), size};
    cbits_set_all(&set, value);
    return set;
}

STC_INLINE cbits cbits_with_pattern(const i_ssize size, const uint64_t pattern) {
    cbits set = {(uint64_t *)c_malloc(_cbits_bytes(size)), size};
    cbits_set_pattern(&set, pattern);
    return set;
}

#else // i_capacity: FIXED SIZE BITARRAY

#define _i_assert(x) (void)0
#ifndef i_type
#define i_type c_PASTE(cbits, i_capacity)
#endif

struct { uint64_t data64[(i_capacity - 1)/64 + 1]; } typedef i_type;

STC_INLINE i_type   _i_memb(_init)(void) { return c_LITERAL(i_type){0}; }
STC_INLINE void     _i_memb(_create)(i_type* self) {}
STC_INLINE void     _i_memb(_drop)(i_type* self) {}
STC_INLINE i_ssize  _i_memb(_size)(const i_type* self) { return i_capacity; }
STC_INLINE i_type   _i_memb(_move)(i_type* self) { return *self; }

STC_INLINE i_type*  _i_memb(_take)(i_type* self, i_type other)
    { *self = other; return self; }

STC_INLINE i_type _i_memb(_clone)(i_type other)
    { return other; }

STC_INLINE i_type* _i_memb(_copy)(i_type* self, const i_type* other) 
    { *self = *other; return self; }
    
STC_INLINE void _i_memb(_set_all)(i_type *self, const bool value);
STC_INLINE void _i_memb(_set_pattern)(i_type *self, const uint64_t pattern);

STC_INLINE i_type _i_memb(_with_size)(const i_ssize size, const bool value) {
    assert(size <= i_capacity);
    i_type set; _i_memb(_set_all)(&set, value);
    return set;
}

STC_INLINE i_type _i_memb(_with_pattern)(const i_ssize size, const uint64_t pattern) {
    assert(size <= i_capacity);
    i_type set; _i_memb(_set_pattern)(&set, pattern);
    return set;
}
#endif // i_capacity

// COMMON:

STC_INLINE void _i_memb(_set_all)(i_type *self, const bool value)
    { c_memset(self->data64, value? ~0 : 0, _cbits_bytes(_i_memb(_size)(self))); }

STC_INLINE void _i_memb(_set_pattern)(i_type *self, const uint64_t pattern) {
    i_ssize n = _cbits_words(_i_memb(_size)(self));
    while (n--) self->data64[n] = pattern;
}

STC_INLINE bool _i_memb(_test)(const i_type* self, const i_ssize i) 
    { return (self->data64[i>>6] & _cbits_bit(i)) != 0; }

STC_INLINE bool _i_memb(_at)(const i_type* self, const i_ssize i)
    { return (self->data64[i>>6] & _cbits_bit(i)) != 0; }

STC_INLINE void _i_memb(_set)(i_type *self, const i_ssize i)
    { self->data64[i>>6] |= _cbits_bit(i); }

STC_INLINE void _i_memb(_reset)(i_type *self, const i_ssize i)
    { self->data64[i>>6] &= ~_cbits_bit(i); }

STC_INLINE void _i_memb(_set_value)(i_type *self, const i_ssize i, const bool b) {
    self->data64[i>>6] ^= ((uint64_t)-(int)b ^ self->data64[i>>6]) & _cbits_bit(i);
}

STC_INLINE void _i_memb(_flip)(i_type *self, const i_ssize i)
    { self->data64[i>>6] ^= _cbits_bit(i); }

STC_INLINE void _i_memb(_flip_all)(i_type *self) {
    i_ssize n = _cbits_words(_i_memb(_size)(self));
    while (n--) self->data64[n] ^= ~(uint64_t)0;
}

STC_INLINE i_type _i_memb(_from)(const char* str) {
    int64_t n = c_strlen(str);
    i_type set = _i_memb(_with_size)(n, false);
    while (n--) if (str[n] == '1') _i_memb(_set)(&set, n);
    return set;
}

/* Intersection */
STC_INLINE void _i_memb(_intersect)(i_type *self, const i_type* other) {
    _i_assert(self->_size == other->_size);
    i_ssize n = _cbits_words(_i_memb(_size)(self));
    while (n--) self->data64[n] &= other->data64[n];
}
/* Union */
STC_INLINE void _i_memb(_union)(i_type *self, const i_type* other) {
    _i_assert(self->_size == other->_size);
    i_ssize n = _cbits_words(_i_memb(_size)(self));
    while (n--) self->data64[n] |= other->data64[n];
}
/* Exclusive disjunction */
STC_INLINE void _i_memb(_xor)(i_type *self, const i_type* other) {
    _i_assert(self->_size == other->_size);
    i_ssize n = _cbits_words(_i_memb(_size)(self));
    while (n--) self->data64[n] ^= other->data64[n];
}

STC_INLINE int64_t _i_memb(_count)(const i_type* self)
    { return _cbits_count(self->data64, _i_memb(_size)(self)); }

STC_INLINE char* _i_memb(_to_str)(const i_type* self, char* out, int64_t start, int64_t stop)
    { return _cbits_to_str(self->data64, _i_memb(_size)(self), out, start, stop); }

STC_INLINE bool _i_memb(_subset_of)(const i_type* self, const i_type* other) { 
    _i_assert(self->_size == other->_size);
    return _cbits_subset_of(self->data64, other->data64, _i_memb(_size)(self));
}

STC_INLINE bool _i_memb(_disjoint)(const i_type* self, const i_type* other) {
    _i_assert(self->_size == other->_size);
    return _cbits_disjoint(self->data64, other->data64, _i_memb(_size)(self));
}
#if defined __GNUC__ && !defined __clang__
#pragma GCC diagnostic pop
#endif
#define CBITS_H_INCLUDED
#undef _i_size
#undef _i_memb
#undef _i_assert
#undef i_capacity
#undef i_type
#undef i_opt
#undef i_header
#undef i_implement
#undef i_static
#undef i_exterm
