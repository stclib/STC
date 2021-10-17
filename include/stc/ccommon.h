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
#ifndef CCOMMON_H_INCLUDED
#define CCOMMON_H_INCLUDED

#define __USE_MINGW_ANSI_STDIO 1

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <assert.h>

#if defined(_MSC_VER)
#  define STC_FORCE_INLINE static __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#  define STC_FORCE_INLINE static inline __attribute((always_inline))
#else
#  define STC_FORCE_INLINE static inline
#endif
#define STC_INLINE static inline

#if defined(STC_HEADER) || defined(STC_IMPLEMENTATION) || defined(i_imp)
#  define STC_API extern
#  define STC_DEF
#  define STC_LIBRARY_ONLY(...) __VA_ARGS__
#  define STC_STATIC_ONLY(...)
#else
#  define STC_API static inline
#  define STC_DEF static inline
#  define STC_LIBRARY_ONLY(...)
#  define STC_STATIC_ONLY(...) __VA_ARGS__
#endif

/* Macro overloading feature support based on: https://rextester.com/ONP80107 */
#define c_MACRO_OVERLOAD(name, ...) \
    c_PASTE3(name, _, c_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)
#define c_CONCAT(a, b) a ## b
#define c_PASTE(a, b) c_CONCAT(a, b)
#define c_CONCAT3(a, b, c) a ## b ## c
#define c_PASTE3(a, b, c) c_CONCAT3(a, b, c)
#define c_EXPAND(...) __VA_ARGS__
#define c_NUM_ARGS(...) _c_APPLY_ARG_N((__VA_ARGS__, _c_RSEQ_N))

#define _c_APPLY_ARG_N(args) c_EXPAND(_c_ARG_N args)
#define _c_RSEQ_N 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, \
                  16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define _c_ARG_N(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, \
                 _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, \
                 _23, _24, _25, _26, _27, _28, _29, _30, N, ...) N

#define c_static_assert(cond) \
    typedef char c_PASTE(_static_assert_line_, __LINE__)[(cond) ? 1 : -1]
#define c_container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

#if __cplusplus
#define c_new(T)                static_cast<T*>(c_malloc(sizeof(T)))
#define c_new_n(T, n)           static_cast<T*>(c_malloc(sizeof(T)*(n)))
#define c_make(T)               T
#else
#define c_new(T)                c_malloc(sizeof(T))
#define c_new_n(T, n)           c_malloc(sizeof(T)*(n))
#define c_make(T)               (T)
#endif
#ifndef c_malloc
#define c_malloc(sz)            malloc(sz)
#define c_calloc(n, sz)         calloc(n, sz)
#define c_realloc(p, sz)        realloc(p, sz)
#define c_free(p)               free(p)
#endif

#define c_swap(T, x, y)         do { T _c_t = x; x = y; y = _c_t; } while (0)
#define c_arraylen(a)           (sizeof (a)/sizeof (a)[0])

#define c_default_compare(x, y) c_less_compare(c_default_less, x, y)
#define c_default_less(x, y)    (*(x) < *(y))
#define c_no_compare(x, y)      (assert(!"c_no_compare() called"), (x)==(y))
#define c_less_compare(less, x, y) (less(y, x) - less(x, y))

#define c_default_equals(x, y)  (*(x) == *(y))
#define c_memcmp_equals(x, y)   (memcmp(x, y, sizeof *(x)) == 0)

#define c_rawstr_compare(x, y)  strcmp(*(x), *(y))
#define c_rawstr_equals(x, y)   (strcmp(*(x), *(y)) == 0)
#define c_rawstr_hash(p, dummy) c_strhash(*(p))

#define c_no_clone(x)           (assert(!"c_no_clone() called"), x)
#define c_default_fromraw(x)    (x)
#define c_default_toraw(ptr)    (*(ptr))

#define c_default_del(ptr)      ((void) (ptr))

#define _c_rotl(x, k) (x << (k) | x >> (8*sizeof(x) - (k)))
STC_INLINE uint64_t c_strhash(const char *str) {
    int c; uint64_t h = 0xb5ad4eceda1ce2a9;
    while ((c = *str++)) h = (_c_rotl(h, 4) ^ (h << 13)) + c;
    return h ^ (h >> 15);
}
STC_INLINE uint64_t c_default_hash(const void *key, size_t len);
#define c_default_hash32(data, len_is_4) \
    ((*(const uint32_t*)data * 0xc6a4a7935bd1e99d) >> 15)
#define c_default_hash64(data, len_is_8) \
    (*(const uint64_t *)data * 0xc6a4a7935bd1e99d)

/* Generic algorithms */

#define c_foreach(...) c_MACRO_OVERLOAD(c_foreach, __VA_ARGS__)
#define c_foreach_3(it, CX, cnt) \
    for (CX##_iter_t it = CX##_begin(&cnt), it##_end_ = CX##_end(&cnt) \
         ; it.ref != it##_end_.ref; CX##_next(&it))
#define c_foreach_4(it, CX, start, finish) \
    for (CX##_iter_t it = start, it##_end_ = finish \
         ; it.ref != it##_end_.ref; CX##_next(&it))

#define c_forrange(...) c_MACRO_OVERLOAD(c_forrange, __VA_ARGS__)
#define c_forrange_1(stop) for (size_t _c_ii=0, _c_end=stop; _c_ii < _c_end; ++_c_ii)
#define c_forrange_2(i, stop) for (size_t i=0, _c_end=stop; i < _c_end; ++i)
#define c_forrange_3(i, type, stop) for (type i=0, _c_end=stop; i < _c_end; ++i)
#define c_forrange_4(i, type, start, stop) for (type i=start, _c_end=stop; i < _c_end; ++i)
#define c_forrange_5(i, type, start, stop, step) \
    for (type i=start, _c_inc=step, _c_end=(stop) - (0 < _c_inc) \
         ; (i <= _c_end) == (0 < _c_inc); i += _c_inc)

#define c_autoscope(init, ...) for (int _c_ii = (init, 0); !_c_ii; ++_c_ii, __VA_ARGS__)
#define c_autovar(declvar, ...) for (declvar, *_c_ii = NULL; !_c_ii; ++_c_ii, __VA_ARGS__)
#define c_autodefer(...) for (int _c_ii = 0; !_c_ii; ++_c_ii, __VA_ARGS__)
#define c_exitauto continue

#define c_auto(...) c_MACRO_OVERLOAD(c_auto, __VA_ARGS__)
#define c_auto_2(CX, a) \
    c_autovar(CX a = CX##_init(), CX##_del(&a))
#define c_auto_3(CX, a, b) \
    c_autovar(c_EXPAND(CX a = CX##_init(), b = CX##_init()), \
              CX##_del(&b), CX##_del(&a))
#define c_auto_4(CX, a, b, c) \
    c_autovar(c_EXPAND(CX a = CX##_init(), b = CX##_init(), c = CX##_init()), \
              CX##_del(&c), CX##_del(&b), CX##_del(&a))

#define c_autobuf(b, type, n) c_autobuf_N(b, type, n, 256)
#define c_autobuf_N(b, type, n, BYTES) \
    for (type _c_b[((BYTES) - 1) / sizeof(type) + 1], \
         *b = (n)*sizeof *b > (BYTES) ? c_new_n(type, n) : _c_b \
         ; b; b != _c_b ? c_free(b) : (void)0, b = NULL)

#define c_apply(CX, method, cx, ...) do { \
    const CX##_rawvalue_t _c_arr[] = __VA_ARGS__; \
    CX* _c_cx = cx; \
    for (size_t _c_i = 0; _c_i < c_arraylen(_c_arr); ++_c_i) \
        CX##_##method(_c_cx, _c_arr[_c_i]); \
} while (0)
#define c_apply_pair(CX, method, cx, ...) do { \
    const CX##_rawvalue_t _c_arr[] = __VA_ARGS__; \
    CX* _c_cx = cx; \
    for (size_t _c_i = 0; _c_i < c_arraylen(_c_arr); ++_c_i) \
        CX##_##method(_c_cx, _c_arr[_c_i].first, _c_arr[_c_i].second); \
} while (0)
#define c_apply_n(CX, method, cx, arr, n) do { \
    CX* _c_cx = cx; \
    for (const CX##_rawvalue_t *_c_i = arr, *_c_end = _c_i+(n); _c_i != _c_end; ++_c_i) \
        CX##_##method(_c_cx, *_c_i); \
} while (0)

#define c_del(CX, ...) do { \
    CX* _c_arr[] = {__VA_ARGS__}; \
    for (size_t _c_i = 0; _c_i < c_arraylen(_c_arr); ++_c_i) \
        CX##_del(_c_arr[_c_i]); \
} while (0)

#if defined(__SIZEOF_INT128__)
    #define c_umul128(a, b, lo, hi) \
        do { __uint128_t _z = (__uint128_t)(a)*(b); \
             *(lo) = (uint64_t)_z, *(hi) = _z >> 64; } while(0)
#elif defined(_MSC_VER) && defined(_WIN64)
    #include <intrin.h>
    #define c_umul128(a, b, lo, hi) ((void)(*(lo) = _umul128(a, b, hi)))
#elif defined(__x86_64__)
    #define c_umul128(a, b, lo, hi) \
        asm("mulq %3" : "=a"(*(lo)), "=d"(*(hi)) : "a"(a), "rm"(b))
#endif
#endif
