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

        printf("%4zu: ", cbits_size(bset));
        c_forrange (i, cbits_size(bset))
            printf("%d", cbits_at(&bset, i));
        puts("");
        cbits_set(&bset, 28);
        cbits_resize(&bset, 77, true);
        cbits_resize(&bset, 93, false);
        cbits_resize(&bset, 102, true);
        cbits_set_value(&bset, 99, false);

        printf("%4zu: ", cbits_size(bset));
        c_forrange (i, cbits_size(bset))
            printf("%d", cbits_at(&bset, i));
        puts("");
    }
}
*/
#include "ccommon.h"
#include <stdlib.h>
#include <string.h>

#if !defined i_type && defined i_len
  #define i_type c_paste(cbits, i_len)
#elif !defined i_type
  #define i_type cbits
#endif
#define _i_memb(name) c_paste(i_type, name)
#ifdef i_len
  struct { uint64_t data64[(i_len - 1)/64 + 1]; } typedef i_type;
#else
  struct { uint64_t *data64; size_t _size; } typedef i_type;
#endif

#define _cbits_bit(i) ((uint64_t)1 << ((i) & 63))
#define _cbits_words(n) (((n) + 63)>>6)
#define _cbits_bytes(n) (_cbits_words(n) * sizeof(uint64_t))

#ifndef i_len
STC_API void        _i_memb(_resize)(i_type* self, size_t size, bool value);
#endif
STC_API i_type      _i_memb(_with_size)(size_t size, bool value);
STC_API i_type      _i_memb(_with_pattern)(size_t size, uint64_t pattern);
STC_API i_type      _i_memb(_from_n)(const char* str, size_t n);
STC_API i_type*     _i_memb(_copy)(i_type* self, i_type other);
STC_API i_type      _i_memb(_clone)(i_type other);
STC_API size_t      _i_memb(_count)(i_type set);
STC_API bool        _i_memb(_subset_of)(i_type set, i_type other);
STC_API bool        _i_memb(_disjoint)(i_type set, i_type other);
STC_API char*       _i_memb(_to_str)(i_type set, char* str, size_t start, intptr_t stop);

#ifdef i_len
#define _i_assert(x) (void)0
STC_INLINE i_type   _i_memb(_init)(void) { return c_make(i_type){0}; }
STC_INLINE void     _i_memb(_drop)(i_type* self) {}
STC_INLINE size_t   _i_memb(_size)(i_type set) { return i_len; }
STC_INLINE i_type   _i_memb(_move)(i_type* self) { return *self; }
STC_INLINE i_type*  _i_memb(_take)(i_type* self, i_type other)
    { *self = other; return self; }
#else
#define _i_assert(x) assert(x)
STC_INLINE i_type   _i_memb(_init)(void) { return c_make(i_type){NULL}; }
STC_INLINE void     _i_memb(_drop)(i_type* self) { c_free(self->data64); }
STC_INLINE size_t   _i_memb(_size)(i_type set) { return set._size; }

STC_INLINE i_type _i_memb(_move)(i_type* self) {
    i_type tmp = *self;
    self->data64 = NULL, self->_size = 0;
    return tmp;
}

STC_INLINE i_type* _i_memb(_take)(i_type* self, i_type other) {
    if (self->data64 != other.data64) {
        _i_memb(_drop)(self);
        *self = other;
    }
    return self;
}
#endif

STC_INLINE i_type _i_memb(_from)(const char* s)
    { return _i_memb(_from_n)(s, strlen(s)); }

STC_INLINE bool _i_memb(_test)(i_type set, const size_t i) 
    { return (set.data64[i>>6] & _cbits_bit(i)) != 0; }

STC_INLINE bool _i_memb(_at)(i_type set, const size_t i)
    { return (set.data64[i>>6] & _cbits_bit(i)) != 0; }

STC_INLINE void _i_memb(_set)(i_type *self, const size_t i)
    { self->data64[i>>6] |= _cbits_bit(i); }

STC_INLINE void _i_memb(_reset)(i_type *self, const size_t i)
    { self->data64[i>>6] &= ~_cbits_bit(i); }

STC_INLINE void _i_memb(_set_value)(i_type *self, const size_t i, const bool value) {
    self->data64[i>>6] ^= ((uint64_t)-(int)value ^ self->data64[i>>6]) & _cbits_bit(i);
}

STC_INLINE void _i_memb(_flip)(i_type *self, const size_t i)
    { self->data64[i>>6] ^= _cbits_bit(i); }

STC_INLINE void _i_memb(_set_all)(i_type *self, const bool value)
    { memset(self->data64, value? ~0 : 0, _cbits_bytes(_i_memb(_size)(*self))); }

STC_INLINE void _i_memb(_set_pattern)(i_type *self, const uint64_t pattern) {
    size_t n = _cbits_words(_i_memb(_size)(*self));
    while (n--) self->data64[n] = pattern;
}

STC_INLINE void _i_memb(_flip_all)(i_type *self) {
    size_t n = _cbits_words(_i_memb(_size)(*self));
    while (n--) self->data64[n] ^= ~(uint64_t)0;
}

/* Intersection */
STC_INLINE void _i_memb(_intersect)(i_type *self, i_type other) {
    _i_assert(self->_size == other._size);
    size_t n = _cbits_words(_i_memb(_size)(*self));
    while (n--) self->data64[n] &= other.data64[n];
}
/* Union */
STC_INLINE void _i_memb(_union)(i_type *self, i_type other) {
    _i_assert(self->_size == other._size);
    size_t n = _cbits_words(_i_memb(_size)(*self));
    while (n--) self->data64[n] |= other.data64[n];
}
/* Exclusive disjunction */
STC_INLINE void _i_memb(_xor)(i_type *self, i_type other) {
    _i_assert(self->_size == other._size);
    size_t n = _cbits_words(_i_memb(_size)(*self));
    while (n--) self->data64[n] ^= other.data64[n];
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

#if defined(i_implement)

#ifdef i_len
STC_DEF i_type _i_memb(_clone)(i_type other)
    { return other; }

STC_DEF i_type* _i_memb(_copy)(i_type* self, i_type other) 
    { *self = other; return self; }

STC_DEF i_type _i_memb(_with_size)(const size_t size, const bool value) {
    assert(size <= i_len);
    i_type set; _i_memb(_set_all)(&set, value);
    return set;
}

STC_DEF i_type _i_memb(_with_pattern)(const size_t size, const uint64_t pattern) {
    assert(size <= i_len);
    i_type set; _i_memb(_set_pattern)(&set, pattern);
    return set;
}
#else

STC_DEF i_type _i_memb(_clone)(i_type other) {
    const size_t bytes = _cbits_bytes(other._size);
    i_type set = {(uint64_t *)memcpy(c_malloc(bytes), other.data64, bytes), other._size};
    return set;
}

STC_DEF i_type* _i_memb(_copy)(i_type* self, i_type other) {
    if (self->data64 == other.data64)
        return self;
    if (self->_size != other._size)
        return _i_memb(_take)(self, _i_memb(_clone)(other));
    memcpy(self->data64, other.data64, _cbits_bytes(other._size));
    return self;
}

STC_DEF void _i_memb(_resize)(i_type* self, const size_t size, const bool value) {
    const size_t new_n = _cbits_words(size), osize = self->_size, old_n = _cbits_words(osize);
    self->data64 = (uint64_t *)c_realloc(self->data64, new_n*8);
    self->_size = size;
    if (new_n >= old_n) {
        memset(self->data64 + old_n, -(int)value, (new_n - old_n)*8);
        if (old_n > 0) {
            uint64_t m = _cbits_bit(osize) - 1; /* mask */
            value ? (self->data64[old_n - 1] |= ~m) 
                  : (self->data64[old_n - 1] &= m);
        }
    }
}

STC_DEF i_type _i_memb(_with_size)(const size_t size, const bool value) {
    i_type set = {(uint64_t *)c_malloc(_cbits_bytes(size)), size};
    _i_memb(_set_all)(&set, value);
    return set;
}

STC_DEF i_type _i_memb(_with_pattern)(const size_t size, const uint64_t pattern) {
    i_type set = {(uint64_t *)c_malloc(_cbits_bytes(size)), size};
    _i_memb(_set_pattern)(&set, pattern);
    return set;
}

#endif

STC_DEF i_type _i_memb(_from_n)(const char* str, const size_t n) {
    i_type set = _i_memb(_with_size)(n, false);
    for (size_t i = 0; i < n; ++i)
        if (str[i] == '1')
            _i_memb(_set)(&set, i);
    return set;
}

STC_DEF size_t _i_memb(_count)(i_type set) {
    const size_t sz = _i_memb(_size)(set), n = sz>>6;
    size_t count = 0;
    for (size_t i = 0; i < n; ++i)
        count += cpopcount64(set.data64[i]);
    if (sz & 63)
        count += cpopcount64(set.data64[n] & (_cbits_bit(sz) - 1));
    return count;
}

STC_DEF char* _i_memb(_to_str)(i_type set, char* out, size_t start, intptr_t stop) {
    if (stop < 0)
        stop = _i_memb(_size)(set);
    memset(out, '0', stop - start);
    for (intptr_t i = start; i < stop; ++i) 
        if (_i_memb(_test)(set, i))
            out[i - start] = '1';
    out[stop - start] = '\0';
    return out;
}

#define _cbits_OPR(OPR, VAL) \
    _i_assert(set._size == other._size); \
    const size_t sz = _i_memb(_size)(set), n = sz>>6; \
    for (size_t i = 0; i < n; ++i) \
        if ((set.data64[i] OPR other.data64[i]) != VAL) \
            return false; \
    if (!(sz & 63)) \
        return true; \
    const uint64_t i = n, m = _cbits_bit(sz) - 1; \
    return ((set.data64[i] OPR other.data64[i]) & m) == (VAL & m)

STC_DEF bool _i_memb(_subset_of)(i_type set, i_type other) { _cbits_OPR(|, set.data64[i]); }
STC_DEF bool _i_memb(_disjoint)(i_type set, i_type other) { _cbits_OPR(&, 0); }

#endif
#endif
#undef _i_memb
#undef _i_assert
#undef i_len
#undef i_type
#undef i_opt
