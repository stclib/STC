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

int main(void) {
    cbits bset = cbits_with_size(23, true);
    cbits_reset(&bset, 9);
    cbits_resize(&bset, 43, false);

    printf("%4d: ", (int)cbits_size(&bset));
    c_forrange (i, cbits_size(&bset))
        printf("%d", cbits_at(&bset, i));
    puts("");
    cbits_set(&bset, 28);
    cbits_resize(&bset, 77, true);
    cbits_resize(&bset, 93, false);
    cbits_resize(&bset, 102, true);
    cbits_set_value(&bset, 99, false);

    printf("%4d: ", (int)cbits_size(&bset));
    c_forrange (i, cbits_size(&bset))
        printf("%d", cbits_at(&bset, i));
    puts("");

    cbits_drop(&bset);
}
*/
#include "priv/linkage.h"
#ifndef STC_CBITS_H_INCLUDED
#define STC_CBITS_H_INCLUDED
#include "common.h"
#include <stdlib.h>
#include <string.h>

#if INTPTR_MAX == INT64_MAX
#define _gnu_popc(x) __builtin_popcountll(x)
#define _msc_popc(x) (int)__popcnt64(x)
#else
#define _gnu_popc(x) __builtin_popcount(x)
#define _msc_popc(x) (int)__popcnt(x)
#endif
#define _cbits_BN (8*c_sizeof(uintptr_t))
#define _cbits_bit(i) ((uintptr_t)1 << ((i) & (_cbits_BN - 1)))
#define _cbits_words(n) (intptr_t)(((n) + (_cbits_BN - 1))/_cbits_BN)
#define _cbits_bytes(n) (_cbits_words(n)*c_sizeof(uintptr_t))

#if defined _MSC_VER
  #include <intrin.h>
  STC_INLINE int c_popcount(uintptr_t x) { return _msc_popc(x); }
#elif defined __GNUC__ || defined __clang__
  STC_INLINE int c_popcount(uintptr_t x) { return _gnu_popc(x); }
#else
  STC_INLINE int c_popcount(uintptr_t x) { /* http://en.wikipedia.org/wiki/Hamming_weight */
    x -= (x >> 1) & (uintptr_t)0x5555555555555555;
    x = (x & (uintptr_t)0x3333333333333333) + ((x >> 2) & (uintptr_t)0x3333333333333333);
    x = (x + (x >> 4)) & (uintptr_t)0x0f0f0f0f0f0f0f0f;
    return (int)((x*(uintptr_t)0x0101010101010101) >> (_cbits_BN - 8));
  }
#endif
#if defined __GNUC__ && !defined __clang__ && !defined __cplusplus
#pragma GCC diagnostic ignored "-Walloc-size-larger-than=" // gcc 11.4
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"     // gcc 11.4
#endif

STC_INLINE intptr_t _cbits_count(const uintptr_t* set, const intptr_t sz) {
    const intptr_t n = sz/_cbits_BN;
    intptr_t count = 0;
    for (intptr_t i = 0; i < n; ++i)
        count += c_popcount(set[i]);
    if (sz & (_cbits_BN - 1))
        count += c_popcount(set[n] & (_cbits_bit(sz) - 1));
    return count;
}

STC_INLINE char* _cbits_to_str(const uintptr_t* set, const intptr_t sz,
                               char* out, intptr_t start, intptr_t stop) {
    if (stop > sz) stop = sz;
    c_assert(start <= stop);

    c_memset(out, '0', stop - start);
    for (intptr_t i = start; i < stop; ++i)
        if ((set[i/_cbits_BN] & _cbits_bit(i)) != 0)
            out[i - start] = '1';
    out[stop - start] = '\0';
    return out;
}

#define _cbits_OPR(OPR, VAL) \
    const intptr_t n = sz/_cbits_BN; \
    for (intptr_t i = 0; i < n; ++i) \
        if ((set[i] OPR other[i]) != VAL) \
            return false; \
    if (!(sz & (_cbits_BN - 1))) \
        return true; \
    const uintptr_t i = (uintptr_t)n, m = _cbits_bit(sz) - 1; \
    return ((set[i] OPR other[i]) & m) == (VAL & m)

STC_INLINE bool _cbits_subset_of(const uintptr_t* set, const uintptr_t* other, const intptr_t sz)
    { _cbits_OPR(|, set[i]); }

STC_INLINE bool _cbits_disjoint(const uintptr_t* set, const uintptr_t* other, const intptr_t sz)
    { _cbits_OPR(&, 0); }

#endif // STC_CBITS_H_INCLUDED

#define _i_memb(name) c_JOIN(i_type, name)

#if !defined i_capacity // DYNAMIC SIZE BITARRAY

#define _i_assert(x) c_assert(x)
#define i_type cbits

typedef struct { uintptr_t *buffer; intptr_t _size; } i_type;

STC_INLINE cbits cbits_init(void) { return c_LITERAL(cbits){NULL}; }
STC_INLINE void cbits_drop(cbits* self) { i_free(self->buffer, _cbits_bytes(self->_size)); }
STC_INLINE intptr_t cbits_size(const cbits* self) { return self->_size; }

STC_INLINE cbits* cbits_take(cbits* self, cbits other) {
    if (self->buffer != other.buffer) {
        cbits_drop(self);
        *self = other;
    }
    return self;
}

STC_INLINE cbits cbits_clone(cbits other) {
    const intptr_t bytes = _cbits_bytes(other._size);
    cbits set = {(uintptr_t *)c_memcpy(i_malloc(bytes), other.buffer, bytes), other._size};
    return set;
}

STC_INLINE cbits* cbits_copy(cbits* self, const cbits* other) {
    if (self->buffer == other->buffer)
        return self;
    if (self->_size != other->_size)
        return cbits_take(self, cbits_clone(*other));
    c_memcpy(self->buffer, other->buffer, _cbits_bytes(other->_size));
    return self;
}

STC_INLINE void cbits_resize(cbits* self, const intptr_t size, const bool value) {
    const intptr_t new_n = _cbits_words(size), osize = self->_size, old_n = _cbits_words(osize);
    self->buffer = (uintptr_t *)i_realloc(self->buffer, old_n*8, new_n*8);
    self->_size = size;
    if (new_n >= old_n) {
        c_memset(self->buffer + old_n, -(int)value, (new_n - old_n)*8);
        if (old_n > 0 && osize < size) {
            uintptr_t mask = _cbits_bit(osize) - 1;
            if (value) self->buffer[old_n - 1] |= ~mask;
            else       self->buffer[old_n - 1] &= mask;
        }
    }
}

STC_INLINE void cbits_set_all(cbits *self, const bool value);
STC_INLINE void cbits_set_pattern(cbits *self, const uintptr_t pattern);

STC_INLINE cbits cbits_move(cbits* self) {
    cbits tmp = *self;
    self->buffer = NULL, self->_size = 0;
    return tmp;
}

STC_INLINE cbits cbits_with_size(const intptr_t size, const bool value) {
    cbits set = {(uintptr_t *)i_malloc(_cbits_bytes(size)), size};
    cbits_set_all(&set, value);
    return set;
}

STC_INLINE cbits cbits_with_pattern(const intptr_t size, const uintptr_t pattern) {
    cbits set = {(uintptr_t *)i_malloc(_cbits_bytes(size)), size};
    cbits_set_pattern(&set, pattern);
    return set;
}

#else // i_capacity: FIXED SIZE BITARRAY

#define _i_assert(x) (void)0
#ifndef i_type
#define i_type c_JOIN(cbits, i_capacity)
#endif

typedef struct { uintptr_t buffer[(i_capacity - 1)/64 + 1]; } i_type;

STC_INLINE void     _i_memb(_init)(i_type* self) { memset(self->buffer, 0, i_capacity*8); }
STC_INLINE void     _i_memb(_drop)(i_type* self) { (void)self; }
STC_INLINE intptr_t _i_memb(_size)(const i_type* self) { (void)self; return i_capacity; }
STC_INLINE i_type   _i_memb(_move)(i_type* self) { return *self; }

STC_INLINE i_type*  _i_memb(_take)(i_type* self, i_type other)
    { *self = other; return self; }

STC_INLINE i_type _i_memb(_clone)(i_type other)
    { return other; }

STC_INLINE i_type* _i_memb(_copy)(i_type* self, const i_type* other)
    { *self = *other; return self; }

STC_INLINE void _i_memb(_set_all)(i_type *self, const bool value);
STC_INLINE void _i_memb(_set_pattern)(i_type *self, const uintptr_t pattern);

STC_INLINE i_type _i_memb(_with_size)(const intptr_t size, const bool value) {
    c_assert(size <= i_capacity);
    i_type set; _i_memb(_set_all)(&set, value);
    return set;
}

STC_INLINE i_type _i_memb(_with_pattern)(const intptr_t size, const uintptr_t pattern) {
    c_assert(size <= i_capacity);
    i_type set; _i_memb(_set_pattern)(&set, pattern);
    return set;
}
#endif // i_capacity

// COMMON:

STC_INLINE void _i_memb(_set_all)(i_type *self, const bool value)
    { c_memset(self->buffer, value? ~0 : 0, _cbits_bytes(_i_memb(_size)(self))); }

STC_INLINE void _i_memb(_set_pattern)(i_type *self, const uintptr_t pattern) {
    intptr_t n = _cbits_words(_i_memb(_size)(self));
    while (n--) self->buffer[n] = pattern;
}

STC_INLINE bool _i_memb(_test)(const i_type* self, const intptr_t i)
    { return (self->buffer[i/_cbits_BN] & _cbits_bit(i)) != 0; }

STC_INLINE bool _i_memb(_at)(const i_type* self, const intptr_t i)
    { return (self->buffer[i/_cbits_BN] & _cbits_bit(i)) != 0; }

STC_INLINE void _i_memb(_set)(i_type *self, const intptr_t i)
    { self->buffer[i/_cbits_BN] |= _cbits_bit(i); }

STC_INLINE void _i_memb(_reset)(i_type *self, const intptr_t i)
    { self->buffer[i/_cbits_BN] &= ~_cbits_bit(i); }

STC_INLINE void _i_memb(_set_value)(i_type *self, const intptr_t i, const bool b) {
    self->buffer[i/_cbits_BN] ^= ((uintptr_t)-(int)b ^ self->buffer[i/_cbits_BN]) & _cbits_bit(i);
}

STC_INLINE void _i_memb(_flip)(i_type *self, const intptr_t i)
    { self->buffer[i/_cbits_BN] ^= _cbits_bit(i); }

STC_INLINE void _i_memb(_flip_all)(i_type *self) {
    intptr_t n = _cbits_words(_i_memb(_size)(self));
    while (n--) self->buffer[n] ^= ~(uintptr_t)0;
}

STC_INLINE i_type _i_memb(_from)(const char* str) {
    intptr_t n = c_strlen(str);
    i_type set = _i_memb(_with_size)(n, false);
    while (n--) if (str[n] == '1') _i_memb(_set)(&set, n);
    return set;
}

/* Intersection */
STC_INLINE void _i_memb(_intersect)(i_type *self, const i_type* other) {
    _i_assert(self->_size == other->_size);
    intptr_t n = _cbits_words(_i_memb(_size)(self));
    while (n--) self->buffer[n] &= other->buffer[n];
}
/* Union */
STC_INLINE void _i_memb(_union)(i_type *self, const i_type* other) {
    _i_assert(self->_size == other->_size);
    intptr_t n = _cbits_words(_i_memb(_size)(self));
    while (n--) self->buffer[n] |= other->buffer[n];
}
/* Exclusive disjunction */
STC_INLINE void _i_memb(_xor)(i_type *self, const i_type* other) {
    _i_assert(self->_size == other->_size);
    intptr_t n = _cbits_words(_i_memb(_size)(self));
    while (n--) self->buffer[n] ^= other->buffer[n];
}

STC_INLINE intptr_t _i_memb(_count)(const i_type* self)
    { return _cbits_count(self->buffer, _i_memb(_size)(self)); }

STC_INLINE char* _i_memb(_to_str)(const i_type* self, char* out, intptr_t start, intptr_t stop)
    { return _cbits_to_str(self->buffer, _i_memb(_size)(self), out, start, stop); }

STC_INLINE bool _i_memb(_subset_of)(const i_type* self, const i_type* other) {
    _i_assert(self->_size == other->_size);
    return _cbits_subset_of(self->buffer, other->buffer, _i_memb(_size)(self));
}

STC_INLINE bool _i_memb(_disjoint)(const i_type* self, const i_type* other) {
    _i_assert(self->_size == other->_size);
    return _cbits_disjoint(self->buffer, other->buffer, _i_memb(_size)(self));
}

#include "priv/linkage2.h"
#undef _i_memb
#undef _i_assert
#undef i_capacity
#undef i_type
