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

#ifndef __cplusplus
#  define c_alloc(T)            c_malloc(sizeof(T))
#  define c_alloc_n(T, n)       c_malloc(sizeof(T)*(n))
#  define c_make(T)             (T)
#  define c_new(T, ...)         (T*)memcpy(c_alloc(T), (T[]){__VA_ARGS__}, sizeof(T))
#else
#  include <new>
#  define c_alloc(T)            static_cast<T*>(c_malloc(sizeof(T)))
#  define c_alloc_n(T, n)       static_cast<T*>(c_malloc(sizeof(T)*(n)))
#  define c_make(T)             T
#  define c_new(T, ...)         new (c_alloc(T)) T(__VA_ARGS__)
#endif
#ifndef c_malloc
#  define c_malloc(sz)          malloc(sz)
#  define c_calloc(n, sz)       calloc(n, sz)
#  define c_realloc(p, sz)      realloc(p, sz)
#  define c_free(p)             free(p)
#endif

typedef const char              c_strlit[];
#define c_delete(T, ptr)        do { T *_c_p = ptr; T##_drop(_c_p); c_free(_c_p); } while (0)
#define c_swap(T, x, y)         do { T _c_t = x; x = y; y = _c_t; } while (0)
#define c_arraylen(a)           (sizeof (a)/sizeof (a)[0])
#define c_less_cmp(less, x, y)  (less(y, x) - less(x, y))
#define c_default_less(x, y)    (*(x) < *(y))

#define c_default_cmp(x, y)     c_less_cmp(c_default_less, x, y)
#define c_default_eq(x, y)      (*(x) == *(y))
#define c_memcmp_eq(x, y)       (memcmp(x, y, sizeof *(x)) == 0)

#define c_default_clone(x)      (x)
#define c_default_fromraw(x)    (x) // [deprecated]
#define c_default_toraw(ptr)    (*(ptr))
#define c_default_drop(ptr)     ((void) (ptr))

#define c_option(flag)          ((i_opt) & (flag))
#define c_is_fwd                1
#define c_no_atomic             2
#define c_no_clone              4
#define c_no_cmp                8

/* Generic algorithms */

typedef char* c_mutstr;
typedef const char* c_rawstr;
#define c_rawstr_cmp(xp, yp) strcmp(*(xp), *(yp))
#define c_rawstr_eq(xp, yp) (!strcmp(*(xp), *(yp)))
#define c_rawstr_hash(p, dummy) c_strhash(*(p))
#define c_rawstr_clone(s) strcpy((char*)c_malloc(strlen(s) + 1), s)
#define c_rawstr_drop(p) c_free((char *) &**(p))

#define _c_ROTL(x, k) (x << (k) | x >> (8*sizeof(x) - (k)))

STC_INLINE uint64_t c_strhash(const char *s) {
    int c; uint64_t h = *s++;
    if (h) while ((c = *s++)) h = (h << 10) - h + c;
    return _c_ROTL(h, 26) ^ h;
}
// len >= 1
STC_INLINE uint64_t c_default_hash(const void* key, size_t len) {
    const uint8_t *x = (const uint8_t*) key; 
    uint64_t h = *x++;
    while (--len) h = (h << 10) - h + *x++;
    return _c_ROTL(h, 26) ^ h;
}
#define c_hash32(data, len_is_4) \
    ((*(const uint32_t*)data * 0xc6a4a7935bd1e99d) >> 15)
#define c_hash64(data, len_is_8) \
    (*(const uint64_t *)data * 0xc6a4a7935bd1e99d)

#define c_foreach(...) c_MACRO_OVERLOAD(c_foreach, __VA_ARGS__)
#define c_foreach_3(it, C, cnt) \
    for (C##_iter it = C##_begin(&cnt), it##_end_ = C##_end(&cnt) \
         ; it.ref != it##_end_.ref; C##_next(&it))
#define c_foreach_4(it, C, start, finish) \
    for (C##_iter it = start, it##_end_ = finish \
         ; it.ref != it##_end_.ref; C##_next(&it))

#define c_forpair(key, val, C, cnt) /* structured binding */ \
    for (struct {C##_iter _it, _end; C##_key key; C##_mapped val;} \
         _ = {C##_begin(&cnt), C##_end(&cnt)} \
         ; _._it.ref != _._end.ref && (_.key = _._it.ref->first, _.val = _._it.ref->second, true) \
         ; C##_next(&_._it))

#define c_forrange(...) c_MACRO_OVERLOAD(c_forrange, __VA_ARGS__)
#define c_forrange_1(stop) for (size_t _c_ii=0, _c_end=stop; _c_ii < _c_end; ++_c_ii)
#define c_forrange_2(i, stop) for (size_t i=0, _c_end=stop; i < _c_end; ++i)
#define c_forrange_3(i, type, stop) for (type i=0, _c_end=stop; i < _c_end; ++i)
#define c_forrange_4(i, type, start, stop) for (type i=start, _c_end=stop; i < _c_end; ++i)
#define c_forrange_5(i, type, start, stop, step) \
    for (type i=start, _c_inc=step, _c_end=(stop) - (0 < _c_inc) \
         ; (i <= _c_end) == (0 < _c_inc); i += _c_inc)

#define c_autovar(declvar, ...) for (declvar, *_c_ii = NULL; !_c_ii; ++_c_ii, __VA_ARGS__)
#define c_autoscope(init, ...) for (int _c_ii = (init, 0); !_c_ii; ++_c_ii, __VA_ARGS__)
#define c_autodefer(...) for (int _c_ii = 0; !_c_ii; ++_c_ii, __VA_ARGS__)
#define c_breakauto continue

#define c_auto(...) c_MACRO_OVERLOAD(c_auto, __VA_ARGS__)
#define c_auto_2(C, a) \
    c_autovar(C a = C##_init(), C##_drop(&a))
#define c_auto_3(C, a, b) \
    c_autovar(c_EXPAND(C a = C##_init(), b = C##_init()), \
              C##_drop(&b), C##_drop(&a))
#define c_auto_4(C, a, b, c) \
    c_autovar(c_EXPAND(C a = C##_init(), b = C##_init(), c = C##_init()), \
              C##_drop(&c), C##_drop(&b), C##_drop(&a))
#define c_auto_5(C, a, b, c, d) \
    c_autovar(c_EXPAND(C a = C##_init(), b = C##_init(), c = C##_init(), d = C##_init()), \
              C##_drop(&d), C##_drop(&c), C##_drop(&b), C##_drop(&a))

#define c_autobuf(b, type, n) c_autobuf_N(b, type, n, 256)
#define c_autobuf_N(b, type, n, BYTES) \
    for (type _c_b[((BYTES) - 1) / sizeof(type) + 1], \
         *b = (n)*sizeof *b > (BYTES) ? c_alloc_n(type, n) : _c_b \
         ; b; b != _c_b ? c_free(b) : (void)0, b = NULL)

#define c_apply(v, method, T, ...) do { \
    const T _c_arr[] = __VA_ARGS__; \
    for (size_t index = 0; index < c_arraylen(_c_arr); ++index) \
        { T v = _c_arr[index]; method; } \
} while (0)
#define c_apply_arr(v, method, T, arr, n) do { \
    const T* _c_arr = arr; size_t _n = n; \
    for (size_t index = 0; index < _n; ++index) \
        { T v = _c_arr[index]; method; } \
} while (0)
#define c_apply_cnt(v, method, C, ...) do { \
    size_t index = 0; \
    c_foreach (_it, C, __VA_ARGS__) \
        { const C##_value v = *_it.ref; method; ++index; } \
} while (0)
#define c_pair(v) (v).first, (v).second

// [deprecated]
#define c_apply_OLD(C, method, cx, ...) do { \
    const C##_raw _c_arr[] = __VA_ARGS__; \
    C* _c_cx = cx; \
    for (size_t _c_i = 0; _c_i < c_arraylen(_c_arr); ++_c_i) \
        C##_##method(_c_cx, _c_arr[_c_i]); \
} while (0)
// [deprecated]
#define c_apply_pair_OLD(C, method, cx, ...) do { \
    const C##_raw _c_arr[] = __VA_ARGS__; \
    C* _c_cx = cx; \
    for (size_t _c_i = 0; _c_i < c_arraylen(_c_arr); ++_c_i) \
        C##_##method(_c_cx, _c_arr[_c_i].first, _c_arr[_c_i].second); \
} while (0)

#define c_drop(C, ...) do { \
    C* _c_arr[] = {__VA_ARGS__}; \
    for (size_t _c_i = 0; _c_i < c_arraylen(_c_arr); ++_c_i) \
        C##_drop(_c_arr[_c_i]); \
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
