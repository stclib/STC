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
#ifndef CCOMMON__H__
#define CCOMMON__H__

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

#if defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
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

/* Macro overloading feature support: https://rextester.com/ONP80107 */
#define _c_CAT( A, B ) A ## B
#define _c_EXPAND(...) __VA_ARGS__
#define _c_ARG_N(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, N,...) N
#define _c_RSEQ_N 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define _c_APPLY_ARG_N(ARGS) _c_EXPAND(_c_ARG_N ARGS)
#define _c_VA_ARG_SIZE(...) _c_EXPAND(_c_APPLY_ARG_N((__VA_ARGS__, _c_RSEQ_N)))
#define _c_OVERLOAD_SELECT(NAME, NUM) _c_CAT( NAME ## _, NUM)

#define c_MACRO_OVERLOAD(NAME, ...) _c_OVERLOAD_SELECT(NAME, _c_VA_ARG_SIZE(__VA_ARGS__))(__VA_ARGS__)
#define c_static_assert(cond, ...) typedef char _static_assert_[(cond) ? 1 : -1]
#define c_container_of(ptr, type, member) \
        ((type *)((char *)(ptr) - offsetof(type, member)))

#define c_new(...)              c_MACRO_OVERLOAD(c_new, __VA_ARGS__)
#define c_new_1(T)              ((T *) c_malloc(sizeof(T)))
#define c_new_2(T, n)           ((T *) c_malloc(sizeof(T) * (n)))
#ifndef c_malloc
#define c_malloc(sz)            malloc(sz)
#define c_calloc(n, sz)         calloc(n, sz)
#define c_realloc(p, sz)        realloc(p, sz)
#define c_free(p)               free(p)
#endif

#define c_swap(T, x, y)         do { T __t = x; x = y; y = __t; } while (0)
#define c_arraylen(a)           (sizeof (a)/sizeof (a)[0])

#define c_default_compare(x, y) (c_default_less(y, x) - c_default_less(x, y))
#define c_default_less(x, y)    (*(x) < *(y))
#define c_no_compare(x, y)      (assert(!"c_no_compare() called"), 0)

#define c_default_equals(x, y)  (*(x) == *(y))
#define c_plain_equals(x, y)    (memcmp(x, y, sizeof *(x)) == 0)

#define c_plain_fromraw(x)      (x)
#define c_no_clone(x)           (assert(!"emplace*() with c_no_clone"), x)
#define c_plain_toraw(ptr)    (*(ptr))

#define c_plain_del(ptr)      ((void) (ptr))

/* Generic algorithms */

#define c_foreach(...) c_MACRO_OVERLOAD(c_foreach, __VA_ARGS__)
#define c_foreach_3(it, ctype, cnt) \
    for (ctype##_iter_t it = ctype##_begin(&cnt), it##_end_ = ctype##_end(&cnt); it.ref != it##_end_.ref; ctype##_next(&it))
#define c_foreach_4(it, ctype, start, finish) \
    for (ctype##_iter_t it = start, it##_end_ = finish; it.ref != it##_end_.ref; ctype##_next(&it))

#define c_forrange(...) c_MACRO_OVERLOAD(c_forrange, __VA_ARGS__)
#define c_forrange_1(stop) for (size_t _c_i=0, _c_end_=stop; _c_i < _c_end_; ++_c_i)
#define c_forrange_2(i, stop) for (size_t i=0, i##_end_=stop; i < i##_end_; ++i)
#define c_forrange_3(i, type, stop) for (type i=0, i##_end_=stop; i < i##_end_; ++i)
#define c_forrange_4(i, type, start, stop) for (type i=start, i##_end_=stop; i < i##_end_; ++i)
#define c_forrange_5(i, type, start, stop, step) \
    for (type i=start, i##_inc_=step, i##_end_=(stop) - (0 < i##_inc_); (i <= i##_end_) == (0 < i##_inc_); i += i##_inc_)

#define c_withfile(f, open) for (FILE *f = open; f; fclose(f), f = NULL)
#define c_withbuffer(b, type, n) c_withbuffer_x(b, type, n, 256)
#define c_withbuffer_x(b, type, n, BYTES) \
    for (type __b[((BYTES) - 1) / sizeof(type) + 1], \
               *b = (n) * sizeof *b > (BYTES) ? c_new_2(type, n) : __b; \
         b; b != __b ? c_free(b) : (void)0, b = NULL)
#define c_breakwith continue

#define c_init(ctype, c, ...) \
    ctype c = ctype##_init(); c_emplace_items(&c, ctype, __VA_ARGS__)

#define c_emplace_items(self, ctype, ...) do { \
    const ctype##_rawvalue_t __arr[] = __VA_ARGS__; \
    ctype##_emplace_n(self, __arr, c_arraylen(__arr)); \
} while (0)

#define c_del(ctype, ...) do { \
    ctype* __arr[] = {__VA_ARGS__}; \
    for (size_t __i = 0; __i < c_arraylen(__arr); ++__i) \
        ctype##_del(__arr[__i]); \
} while (0)

#endif
