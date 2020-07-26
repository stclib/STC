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

#define STC_INLINE static inline
#if defined(_MSC_VER)
#define STC_FORCE_INLINE static __forceinline
#elif defined(__GNUC__)
#define STC_FORCE_INLINE static inline __attribute((always_inline))
#else
#define STC_FORCE_INLINE static inline
#endif

#if defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
#define STC_API extern
#else
#define STC_API STC_INLINE
#endif

/* Macro overloading feature support: https://rextester.com/ONP80107 */
#define c_CAT(A, B) A ## B
#define c_EXPAND(...) __VA_ARGS__
#define _c_RSEQ_N 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define _c_ARG_N(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, N,...) N
#define _c_ZERO_ARGS_PREFIX__ZERO_ARGS_SUFFIX ,,,,,,,,,,,,,0 /* 13 commas */
#define _c_ZERO_ARGS(...) c_EXPAND(_c_ZERO_ARGS_PREFIX_ ## __VA_ARGS__ ## _ZERO_ARGS_SUFFIX)
#define _c_VA_ARG_SIZE(...) c_EXPAND(_c_APPLY_ARG_N((_c_ZERO_ARGS(__VA_ARGS__), _c_RSEQ_N)))
#define _c_APPLY_ARG_N(ARGS) c_EXPAND(_c_ARG_N ARGS)
#define _c_OVERLOAD_SELECT(NAME, NUM) c_CAT(NAME ## _, NUM)

#define c_MACRO_OVERLOAD(NAME, ...) _c_OVERLOAD_SELECT(NAME, _c_VA_ARG_SIZE(__VA_ARGS__))(__VA_ARGS__)
/*#define FOO(...) c_MACRO_OVERLOAD(FOO, __VA_ARGS__)   #define FOO_0() "0"   #define FOO_1(x) "1"   #define FOO_2(x,y) "2"*/

#define c_new(T)        ((T *) malloc(sizeof(T)))
#define c_new_N(T, n)   ((T *) malloc(sizeof(T) * (n)))

#define c_max_alloca    (1000)
#define c_swap(T, x, y) { T __t = x; x = y; y = __t; }

#define c_defaultInitRaw(x)    (x)
#define c_defaultGetRaw(ptr)   (*(ptr))
#define c_noCompare(x, y)      (0)
#define c_memCompare(x, y)     memcmp(x, y, sizeof(*(y)))
#define c_memEquals(x, y)      (memCompare(x, y) == 0)
#define c_defaultEquals(x, y)  (*(x) == *(y))
#define c_defaultLess(x, y)    (*(x) < *(y))
#define c_compare(less, x, y)  (less(x, y) ? -1 : less(y, x))
#define c_defaultCompare(x, y) c_compare(c_defaultLess, x, y)
#define c_defaultDestroy(p)    ((void)0)

#define c_foreach(it, prefix, container) \
            for (prefix##_iter_t it = prefix##_begin(&container); it.item; it = prefix##_next(it))
#define c_items(...) {__VA_ARGS__}
#define c_push(container, prefix, items) do { \
    const prefix##_input_t __arr[] = items; \
    prefix##_pushN(container, __arr, sizeof(__arr)/sizeof(prefix##_input_t)); \
} while (0)

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

/* https://programmingpraxis.com/2018/06/19/fibonacci-hash */
/* https://probablydance.com/2018/06/16/fibonacci-hashing-the-optimization-that-the-world-forgot-or-a-better-alternative-to-integer-modulo/ */
static inline uint32_t c_fibonacciHash32(const void* data, size_t len) {
    const volatile uint32_t *key = (const uint32_t *) data;
    uint32_t x = *key++ * 2654435769u;
    while (len -= 4) x ^= *key++ * 2654435769u;
    return x;
}
static inline uint32_t c_fibonacciHash64(const void* data, size_t len) {
    const volatile uint64_t *key = (const uint64_t *) data;
    uint64_t x = *key++ * 11400714819323198485ull;
    while (len -= 8) x ^= *key++ * 11400714819323198485ull;
    return (uint32_t) (x >> 13);
}

#endif
