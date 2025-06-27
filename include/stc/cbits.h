/* MIT License
 *
 * Copyright (c) 2025 Tyge Løvset
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
    for (c_range(i, cbits_size(&bset)))
        printf("%d", cbits_at(&bset, i));
    puts("");
    cbits_set(&bset, 28);
    cbits_resize(&bset, 77, true);
    cbits_resize(&bset, 93, false);
    cbits_resize(&bset, 102, true);
    cbits_set_value(&bset, 99, false);

    printf("%4d: ", (int)cbits_size(&bset));
    for (c_range(i, cbits_size(&bset)))
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

#if INTPTR_MAX == INT64_MAX
#define _gnu_popc(x) __builtin_popcountll(x)
#define _msc_popc(x) (int)__popcnt64(x)
#else
#define _gnu_popc(x) __builtin_popcount(x)
#define _msc_popc(x) (int)__popcnt(x)
#endif
#define _cbits_WS c_sizeof(uintptr_t)
#define _cbits_WB (8*_cbits_WS)
#define _cbits_bit(i) ((uintptr_t)1 << ((i) & (_cbits_WB - 1)))
#define _cbits_words(n) (isize)(((n) + (_cbits_WB - 1))/_cbits_WB)
#define _cbits_bytes(n) (_cbits_words(n)*_cbits_WS)

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
    return (int)((x*(uintptr_t)0x0101010101010101) >> (_cbits_WB - 8));
  }
#endif
#if defined __GNUC__ && !defined __clang__ && !defined __cplusplus
#pragma GCC diagnostic ignored "-Walloc-size-larger-than=" // gcc 11.4
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"     // gcc 11.4
#endif

#define cbits_print(...) c_MACRO_OVERLOAD(cbits_print, __VA_ARGS__)
#define cbits_print_1(self) cbits_print_4(self, stdout, 0, -1)
#define cbits_print_2(self, stream) cbits_print_4(self, stream, 0, -1)
#define cbits_print_4(self, stream, start, end) cbits_print_5(cbits, self, stream, start, end)
#define cbits_print_3(SetType, self, stream) cbits_print_5(SetType, self, stream, 0, -1)
#define cbits_print_5(SetType, self, stream, start, end) do { \
    const SetType* _cb_set = self; \
    isize _cb_start = start, _cb_end = end; \
    if (_cb_end == -1) _cb_end = SetType##_size(_cb_set); \
    for (c_range_3(_cb_i, _cb_start, _cb_end)) \
        fputc(SetType##_test(_cb_set, _cb_i) ? '1' : '0', stream); \
} while (0)

STC_INLINE isize _cbits_count(const uintptr_t* set, const isize sz) {
    const isize n = sz/_cbits_WB;
    isize count = 0;
    for (isize i = 0; i < n; ++i)
        count += c_popcount(set[i]);
    if (sz & (_cbits_WB - 1))
        count += c_popcount(set[n] & (_cbits_bit(sz) - 1));
    return count;
}

STC_INLINE char* _cbits_to_str(const uintptr_t* set, const isize sz,
                               char* out, isize start, isize stop) {
    if (stop > sz) stop = sz;
    c_assert(start <= stop);

    c_memset(out, '0', stop - start);
    for (isize i = start; i < stop; ++i)
        if ((set[i/_cbits_WB] & _cbits_bit(i)) != 0)
            out[i - start] = '1';
    out[stop - start] = '\0';
    return out;
}

#define _cbits_OPR(OPR, VAL) \
    const isize n = sz/_cbits_WB; \
    for (isize i = 0; i < n; ++i) \
        if ((set[i] OPR other[i]) != VAL) \
            return false; \
    if ((sz & (_cbits_WB - 1)) == 0) \
        return true; \
    const uintptr_t i = (uintptr_t)n, m = _cbits_bit(sz) - 1; \
    return ((set[i] OPR other[i]) & m) == (VAL & m)

STC_INLINE bool _cbits_subset_of(const uintptr_t* set, const uintptr_t* other, const isize sz)
    { _cbits_OPR(|, set[i]); }

STC_INLINE bool _cbits_disjoint(const uintptr_t* set, const uintptr_t* other, const isize sz)
    { _cbits_OPR(&, 0); }

#endif // STC_CBITS_H_INCLUDED

#if defined T && !defined i_type
  #define i_type T
#endif
#if defined i_type
  #define Self c_GETARG(1, i_type)
  #define _i_length c_GETARG(2, i_type)
#else
  #define Self cbits
#endif
#ifndef i_allocator
  #define i_allocator c
#endif
#define _i_MEMB(name) c_JOIN(Self, name)


#if !defined _i_length // DYNAMIC SIZE BITARRAY

typedef struct { uintptr_t *buffer; isize _size; } Self;
#define _i_assert(x) c_assert(x)

STC_INLINE void cbits_drop(cbits* self) { i_free(self->buffer, _cbits_bytes(self->_size)); }
STC_INLINE isize cbits_size(const cbits* self) { return self->_size; }

STC_INLINE cbits* cbits_take(cbits* self, cbits other) {
    if (self->buffer != other.buffer) {
        cbits_drop(self);
        *self = other;
    }
    return self;
}

STC_INLINE cbits cbits_clone(cbits other) {
    cbits set = other;
    const isize bytes = _cbits_bytes(other._size);
    set.buffer = (uintptr_t *)c_safe_memcpy(i_malloc(bytes), other.buffer, bytes);
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

STC_INLINE bool cbits_resize(cbits* self, const isize size, const bool value) {
    const isize new_w = _cbits_words(size), osize = self->_size, old_w = _cbits_words(osize);
    uintptr_t* b = (uintptr_t *)i_realloc(self->buffer, old_w*_cbits_WS, new_w*_cbits_WS);
    if (b == NULL) return false;
    self->buffer = b; self->_size = size;
    if (size > osize) {
        c_memset(self->buffer + old_w, -(int)value, (new_w - old_w)*_cbits_WS);
        if (osize & (_cbits_WB - 1)) {
            uintptr_t mask = _cbits_bit(osize) - 1;
            if (value) self->buffer[old_w - 1] |= ~mask;
            else       self->buffer[old_w - 1] &= mask;
        }
    }
    return true;
}

STC_INLINE void cbits_set_all(cbits *self, const bool value);
STC_INLINE void cbits_set_pattern(cbits *self, const uintptr_t pattern);

STC_INLINE cbits cbits_move(cbits* self) {
    cbits tmp = *self;
    self->buffer = NULL, self->_size = 0;
    return tmp;
}

STC_INLINE cbits cbits_with_size(const isize size, const bool value) {
    cbits set = {(uintptr_t *)i_malloc(_cbits_bytes(size)), size};
    cbits_set_all(&set, value);
    return set;
}

STC_INLINE cbits cbits_with_pattern(const isize size, const uintptr_t pattern) {
    cbits set = {(uintptr_t *)i_malloc(_cbits_bytes(size)), size};
    cbits_set_pattern(&set, pattern);
    return set;
}

#else // _i_length: FIXED SIZE BITARRAY

#define _i_assert(x) (void)0

typedef struct { uintptr_t buffer[(_i_length - 1)/_cbits_WB + 1]; } Self;

STC_INLINE void     _i_MEMB(_drop)(Self* self) { (void)self; }
STC_INLINE isize    _i_MEMB(_size)(const Self* self) { (void)self; return _i_length; }
STC_INLINE Self     _i_MEMB(_move)(Self* self) { return *self; }
STC_INLINE Self*    _i_MEMB(_take)(Self* self, Self other) { *self = other; return self; }
STC_INLINE Self     _i_MEMB(_clone)(Self other) { return other; }
STC_INLINE void     _i_MEMB(_copy)(Self* self, const Self* other) { *self = *other; }
STC_INLINE void     _i_MEMB(_set_all)(Self *self, const bool value);
STC_INLINE void     _i_MEMB(_set_pattern)(Self *self, const uintptr_t pattern);

STC_INLINE Self _i_MEMB(_with_size)(const isize size, const bool value) {
    c_assert(size <= _i_length);
    Self set; _i_MEMB(_set_all)(&set, value);
    return set;
}

STC_INLINE Self _i_MEMB(_with_pattern)(const isize size, const uintptr_t pattern) {
    c_assert(size <= _i_length);
    Self set; _i_MEMB(_set_pattern)(&set, pattern);
    return set;
}
#endif // _i_length

// COMMON:

STC_INLINE void _i_MEMB(_set_all)(Self *self, const bool value)
    { c_memset(self->buffer, -(int)value, _cbits_bytes(_i_MEMB(_size)(self))); }

STC_INLINE void _i_MEMB(_set_pattern)(Self *self, const uintptr_t pattern) {
    isize n = _cbits_words(_i_MEMB(_size)(self));
    while (n--) self->buffer[n] = pattern;
}

STC_INLINE bool _i_MEMB(_test)(const Self* self, const isize i)
    { return (self->buffer[i/_cbits_WB] & _cbits_bit(i)) != 0; }

STC_INLINE bool _i_MEMB(_at)(const Self* self, const isize i)
    { c_assert(c_uless(i, _i_MEMB(_size)(self))); return _i_MEMB(_test)(self, i); }

STC_INLINE void _i_MEMB(_set)(Self *self, const isize i)
    { self->buffer[i/_cbits_WB] |= _cbits_bit(i); }

STC_INLINE void _i_MEMB(_reset)(Self *self, const isize i)
    { self->buffer[i/_cbits_WB] &= ~_cbits_bit(i); }

STC_INLINE void _i_MEMB(_set_value)(Self *self, const isize i, const bool b) {
    self->buffer[i/_cbits_WB] ^= ((uintptr_t)-(int)b ^ self->buffer[i/_cbits_WB]) & _cbits_bit(i);
}

STC_INLINE void _i_MEMB(_flip)(Self *self, const isize i)
    { self->buffer[i/_cbits_WB] ^= _cbits_bit(i); }

STC_INLINE void _i_MEMB(_flip_all)(Self *self) {
    isize n = _cbits_words(_i_MEMB(_size)(self));
    while (n--) self->buffer[n] ^= ~(uintptr_t)0;
}

STC_INLINE Self _i_MEMB(_from)(const char* str) {
    isize n = c_strlen(str);
    Self set = _i_MEMB(_with_size)(n, false);
    while (n--) if (str[n] == '1') _i_MEMB(_set)(&set, n);
    return set;
}

/* Intersection */
STC_INLINE void _i_MEMB(_intersect)(Self *self, const Self* other) {
    _i_assert(self->_size == other->_size);
    isize n = _cbits_words(_i_MEMB(_size)(self));
    while (n--) self->buffer[n] &= other->buffer[n];
}
/* Union */
STC_INLINE void _i_MEMB(_union)(Self *self, const Self* other) {
    _i_assert(self->_size == other->_size);
    isize n = _cbits_words(_i_MEMB(_size)(self));
    while (n--) self->buffer[n] |= other->buffer[n];
}
/* Exclusive disjunction */
STC_INLINE void _i_MEMB(_xor)(Self *self, const Self* other) {
    _i_assert(self->_size == other->_size);
    isize n = _cbits_words(_i_MEMB(_size)(self));
    while (n--) self->buffer[n] ^= other->buffer[n];
}

STC_INLINE isize _i_MEMB(_count)(const Self* self)
    { return _cbits_count(self->buffer, _i_MEMB(_size)(self)); }

STC_INLINE char* _i_MEMB(_to_str)(const Self* self, char* out, isize start, isize stop)
    { return _cbits_to_str(self->buffer, _i_MEMB(_size)(self), out, start, stop); }

STC_INLINE bool _i_MEMB(_subset_of)(const Self* self, const Self* other) {
    _i_assert(self->_size == other->_size);
    return _cbits_subset_of(self->buffer, other->buffer, _i_MEMB(_size)(self));
}

STC_INLINE bool _i_MEMB(_disjoint)(const Self* self, const Self* other) {
    _i_assert(self->_size == other->_size);
    return _cbits_disjoint(self->buffer, other->buffer, _i_MEMB(_size)(self));
}

#include "priv/linkage2.h"
#undef i_type
#undef _i_length
#undef _i_MEMB
#undef _i_assert
#undef Self
