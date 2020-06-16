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
#ifndef CDEFS__H__
#define CDEFS__H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#if defined(_MSC_VER)
#  define STC_INLINE __forceinline
#elif defined(__GNUC__)
#  define STC_INLINE inline __attribute((always_inline))
#else
#  define STC_INLINE inline
#endif
#if defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
#define STC_API extern
#else
#define STC_API static STC_INLINE
#endif

/* Macro overloading feature support: https://rextester.com/ONP80107 */
#define c_CAT( A, B ) A ## B
#define c_EXPAND(...) __VA_ARGS__
#define c_VA_ARG_SIZE(...) c_EXPAND(c_APPLY_ARG_N((__VA_ARGS__, c_RSEQ_N)))
#define c_APPLY_ARG_N(ARGS) c_EXPAND(c_ARG_N ARGS)
#define c_ARG_N(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, N,...) N
#define c_RSEQ_N 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define c_OVERLOAD_SELECT(NAME, NUM) c_CAT( NAME ## _, NUM)

#define c_MACRO_OVERLOAD(NAME, ...) c_OVERLOAD_SELECT(NAME, c_VA_ARG_SIZE(__VA_ARGS__))(__VA_ARGS__)

#define c_new(...)      c_MACRO_OVERLOAD(c_new, __VA_ARGS__)
#define c_new_1(T)      ((T *) malloc(sizeof(T)))
#define c_new_2(T, n)   ((T *) malloc(sizeof(T) * (n)))

#define c_struct(S)     typedef struct S S; struct S
#define                 c_max_alloca   (1000)
#define                 c_swap(T, x, y) { T __t = x; x = y; y = __t; }

#define                 c_defaultInitRaw(x)    (x)
#define                 c_defaultGetRaw(ptr)   (*(ptr))
#define                 c_defaultCompare(x, y) (*(x) == *(y) ? 0 : *(x) < *(y) ? -1 : 1)
#define                 c_noCompare(x, y)      (0)
#define                 c_defaultEquals(x, y)  (*(x) == *(y))
#define                 c_memEquals(x, y)      (memcmp(x, y, sizeof(*(y))) == 0)
#define                 c_noDestroy(p)         ((void)0)

#define                 c_foreach(it, prefix, container) \
                            for (prefix##_iter_t it = prefix##_begin(&container); it.item; it = prefix##_next(it))

/* One-byte-at-a-time hash based on Murmur's mix */
static inline uint32_t  c_defaultHash(const void *data, size_t len) {
    const volatile uint8_t *key = (const uint8_t *) data;
    uint32_t x = UINT32_C(0xc613fc15);
    while (len--) {
        x ^= *key++;
        x *= UINT32_C(0x5bd1e995);
        x ^= x >> 15;
    }
    return x;
}

/* https://nullprogram.com/blog/2018/07/31/: assume len positive multiple of 4 */
/* https://probablydance.com/2018/06/16/fibonacci-hashing-the-optimization-that-the-world-forgot-or-a-better-alternative-to-integer-modulo/ */
static inline uint32_t c_lowbias32Hash(const void *data, size_t len) {
    const volatile uint32_t *key = (const uint32_t *) data;
    uint32_t x = *key;
    do {
        x ^= *key++ >> 16;
        x *= UINT32_C(0x7feb352d);
    } while (len -= 4);
    return x;
}

#endif
