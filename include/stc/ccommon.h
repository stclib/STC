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
#ifndef CCOMMON_H_INCLUDED
#define CCOMMON_H_INCLUDED

#define _CRT_SECURE_NO_WARNINGS
#include <inttypes.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#if defined(_MSC_VER)
#  pragma warning(disable: 4116 4996) // unnamed type definition in parentheses
#  define STC_FORCE_INLINE static __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#  define STC_FORCE_INLINE static inline __attribute((always_inline))
#else
#  define STC_FORCE_INLINE static inline
#endif
#define STC_INLINE static inline

/* Macro overloading feature support based on: https://rextester.com/ONP80107 */
#define c_MACRO_OVERLOAD(name, ...) \
    c_paste(name, c_numargs(__VA_ARGS__))(__VA_ARGS__)
#define c_concat(a, b) a ## b
#define c_paste(a, b) c_concat(a, b)
#define c_expand(...) __VA_ARGS__
#define c_numargs(...) _c_APPLY_ARG_N((__VA_ARGS__, _c_RSEQ_N))

#define _c_APPLY_ARG_N(args) c_expand(_c_ARG_N args)
#define _c_RSEQ_N 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define _c_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, \
                 _14, _15, _16, N, ...) N

#define c_static_assert(cond) \
    typedef char c_paste(_static_assert_line_, __LINE__)[(cond) ? 1 : -1]
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

#define c_strlen_lit(literal)   (sizeof "" literal - 1U)
#define c_delete(T, ptr)        do { T *_c_p = ptr; T##_drop(_c_p); c_free(_c_p); } while (0)
#define c_swap(T, x, y)         do { T _c_t = x; x = y; y = _c_t; } while (0)
#define c_arraylen(a)           (sizeof (a)/sizeof (a)[0])
#define c_less_cmp(less, x, y)  (less(y, x) - less(x, y))
#define c_default_less(x, y)    (*(x) < *(y))

#define c_default_cmp(x, y)     c_less_cmp(c_default_less, x, y)
#define c_default_eq(x, y)      (*(x) == *(y))
#define c_memcmp_eq(x, y)       (memcmp(x, y, sizeof *(x)) == 0)

#define c_default_from(x)       (x)
#define c_default_toraw(ptr)    (*(ptr))
#define c_default_drop(ptr)     ((void) (ptr))

#define c_option(flag)          ((i_opt) & (flag))
#define c_is_fwd                (1<<0)
#define c_no_atomic             (1<<1)
#define c_no_clone              (1<<2)
#define c_no_cmp                (1<<3)
#define c_hash                  (1<<4)
#define c_eq                    (1<<5)
#define c_static                (1<<6)
#define c_header                (1<<7)
#define c_implement             (1<<8)

/* Generic algorithms */

typedef const char* crawstr;
#define crawstr_cmp(xp, yp) strcmp(*(xp), *(yp))
#define crawstr_eq(xp, yp) (!strcmp(*(xp), *(yp)))
#define crawstr_hash(p, dummy) c_strhash(*(p))

#define _c_ROTL(x, k) (x << (k) | x >> (8*sizeof(x) - (k)))

STC_INLINE uint64_t c_strhash(const char *s) {
    int c; uint64_t h = *s++;
    if (h) while ((c = *s++)) h = (h << 10) - h + c;
    return _c_ROTL(h, 26) ^ h;
}
STC_INLINE uint64_t c_default_hash(const void* key, size_t len) {
    if (!len) return 1;
    const uint8_t *x = (const uint8_t*) key; 
    uint64_t h = *x++;
    while (--len) h = (h << 10) - h + *x++;
    return _c_ROTL(h, 26) ^ h;
}
STC_INLINE uint64_t c_hash32(const void* key, size_t n) {
    return *(uint32_t *)key*0xc6a4a7935bd1e99d;
}
STC_INLINE uint64_t c_hash64(const void* key, size_t n) {
    return *(uint64_t *)key*0xc6a4a7935bd1e99d;
}

STC_INLINE char* c_strnstrn(const char *s, const char *needle, size_t slen, const size_t nlen) {
    if (!nlen) return (char *)s;
    if (nlen > slen) return NULL;
    slen -= nlen;
    do {
        if (*s == *needle && !memcmp(s, needle, nlen)) 
            return (char *)s;
        ++s;
    } while (slen--);
    return NULL;
}

#define c_foreach(...) c_MACRO_OVERLOAD(c_foreach, __VA_ARGS__)

#define c_foreach3(it, C, cnt) \
    for (C##_iter it = C##_begin(&cnt), it##_end_ = C##_end(&cnt) \
         ; it.ref != it##_end_.ref; C##_next(&it))

#define c_foreach4(it, C, start, finish) \
    for (C##_iter it = start, it##_end_ = finish \
         ; it.ref != it##_end_.ref; C##_next(&it))

#define c_forpair(key, val, C, cnt) /* structured binding */ \
    for (struct {C##_iter _it; C##_value* _endref; C##_key key; C##_mapped val;} \
         _ = {C##_begin(&cnt), C##_end(&cnt).ref} \
         ; _._it.ref != _._endref && (_.key = _._it.ref->first, _.val = _._it.ref->second, true) \
         ; C##_next(&_._it))

#define c_forrange(...) c_MACRO_OVERLOAD(c_forrange, __VA_ARGS__)
#define c_forrange1(stop) for (size_t _c_ii=0, _c_end=stop; _c_ii < _c_end; ++_c_ii)
#define c_forrange2(i, stop) for (size_t i=0, _c_end=stop; i < _c_end; ++i)
#define c_forrange3(i, type, stop) for (type i=0, _c_end=stop; i < _c_end; ++i)
#define c_forrange4(i, type, start, stop) for (type i=start, _c_end=stop; i < _c_end; ++i)
#define c_forrange5(i, type, start, stop, step) \
    for (type i=start, _c_inc=step, _c_end=(stop) - (0 < _c_inc) \
         ; (i <= _c_end) == (0 < _c_inc); i += _c_inc)

#define c_autovar(...) c_MACRO_OVERLOAD(c_autovar, __VA_ARGS__)
#define c_autovar2(declvar, drop) for (declvar, **_c_ii = NULL; !_c_ii; ++_c_ii, drop)
#define c_autovar3(declvar, cond, drop) for (declvar, **_c_ii = NULL; !_c_ii && (cond); ++_c_ii, drop)
#define c_autoscope(init, drop) for (int _c_ii = (init, 0); !_c_ii; ++_c_ii, drop)
#define c_autodefer(...) for (int _c_ii = 0; !_c_ii; ++_c_ii, __VA_ARGS__)
#define c_breakauto continue

#define c_auto(...) c_MACRO_OVERLOAD(c_auto, __VA_ARGS__)
#define c_auto2(C, a) \
    c_autovar2(C a = C##_init(), C##_drop(&a))
#define c_auto3(C, a, b) \
    c_autovar2(c_expand(C a = C##_init(), b = C##_init()), \
               (C##_drop(&b), C##_drop(&a)))
#define c_auto4(C, a, b, c) \
    c_autovar2(c_expand(C a = C##_init(), b = C##_init(), c = C##_init()), \
               (C##_drop(&c), C##_drop(&b), C##_drop(&a)))
#define c_auto5(C, a, b, c, d) \
    c_autovar2(c_expand(C a = C##_init(), b = C##_init(), c = C##_init(), d = C##_init()), \
               (C##_drop(&d), C##_drop(&c), C##_drop(&b), C##_drop(&a)))

#define c_autobuf(b, type, n) c_autobuf_N(b, type, n, 256)
#define c_autobuf_N(b, type, n, BYTES) \
    for (type _c_b[((BYTES) - 1) / sizeof(type) + 1], \
         *b = (n)*sizeof *b > (BYTES) ? c_alloc_n(type, n) : _c_b \
         ; b; b != _c_b ? c_free(b) : (void)0, b = NULL)

#define c_apply(v, action, T, ...) do { \
    typedef T _c_T; \
    const _c_T _c_arr[] = __VA_ARGS__; \
    for (size_t index = 0; index < c_arraylen(_c_arr); ++index) \
        { const _c_T v = _c_arr[index]; action; } \
} while (0)
#define c_apply_arr(v, action, T, arr, n) do { \
    typedef T _c_T; \
    _c_T *_c_arr = arr; \
    for (size_t index = 0, _c_n = n; index < _c_n; ++index) \
        { _c_T *v = _c_arr + index; action; } \
} while (0)
#define c_apply_cnt(v, action, C, ...) do { \
    size_t index = 0; \
    c_foreach (_it, C, __VA_ARGS__) \
        { C##_value* v = _it.ref; action; ++index; } \
} while (0)
#define c_pair(v) (v).first, (v).second

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
#endif // CCOMMON_H_INCLUDED

#undef STC_API
#undef STC_DEF
#undef _i_static
#undef _i_implement

#if !c_option(c_static) && (c_option(c_header) || c_option(c_implement) || \
                            defined(STC_HEADER) || defined(STC_IMPLEMENTATION))
#  define STC_API extern
#  define STC_DEF
#else
#  define _i_static
#  define STC_API static inline
#  define STC_DEF static inline
#endif
#if c_option(c_implement) || defined(STC_IMPLEMENTATION) || defined(_i_static)
#  define _i_implement
#endif
