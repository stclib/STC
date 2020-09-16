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
#include <assert.h>

#if defined(_MSC_VER)
#define STC_FORCE_INLINE static __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#define STC_FORCE_INLINE static inline __attribute((always_inline))
#else
#define STC_FORCE_INLINE static inline
#endif
#define STC_INLINE static inline

#if defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
#define STC_API extern
#else
#define STC_API STC_INLINE
#endif

#if defined(_MSC_VER) || defined(__GNUC__) || defined(__clang__) || defined(__TINYC__)
#define c_UNION(...) union {__VA_ARGS__;}
#else
#define c_UNION(x, ...) x
#endif

/* Macro overloading feature support: https://rextester.com/ONP80107 */
#define _c_CAT( A, B ) A ## B
#define _c_EXPAND(...) __VA_ARGS__
#define _c_ARG_N(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, N,...) N
#define _c_RSEQ_N 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define _c_APPLY_ARG_N(ARGS) _c_EXPAND(_c_ARG_N ARGS)
#define _c_VA_ARG_SIZE(...) _c_EXPAND(_c_APPLY_ARG_N((__VA_ARGS__, _c_RSEQ_N)))
#define _c_OVERLOAD_SELECT(NAME, NUM) _c_CAT( NAME ## _, NUM)

#define c_MACRO_OVERLOAD(NAME, ...) _c_OVERLOAD_SELECT(NAME, _c_VA_ARG_SIZE(__VA_ARGS__))(__VA_ARGS__)
/*#define FOO(...) c_MACRO_OVERLOAD(FOO, __VA_ARGS__)   #define FOO_1(x) "1"   #define FOO_2(x,y) "2"*/

#define c_new(T)        ((T *) malloc(sizeof(T)))
#define c_new_n(T, n)   ((T *) malloc(sizeof(T) * (n)))
#define c_static_assert(cond, msg) typedef char static_assert_##msg[(cond) ? 1 : -1]
#define c_container_of(ptr, type, member) \
                        ((type *)((char *)(ptr) - offsetof(type, member)))

#define c_max_alloca    (512)
#define c_swap(T, x, y) do { T __t = x; x = y; y = __t; } while (0)

#define c_default_from_raw(x)   (x)
#define c_default_to_raw(ptr)   (*(ptr))
#define c_no_compare(x, y)      (0)
#define c_mem_equals(x, y)      (memcmp(x, y, sizeof(*(y))) == 0)
#define c_default_equals(x, y)  (*(x) == *(y))
#define c_default_less(x, y)    (*(x) < *(y))
#define c_less_compare(less, x, y) (less(x, y) ? -1 : less(y, x))
#define c_default_compare(x, y) c_less_compare(c_default_less, x, y)
#define c_default_destroy(ptr)  ((void) (ptr))

#define c_foreach(...) c_MACRO_OVERLOAD(c_foreach, __VA_ARGS__)

#define c_foreach_3(it, ctype, cnt) \
    for (ctype##_iter_t it = ctype##_begin(&cnt), it##_end_ = ctype##_end(&cnt); it.get != it##_end_.get; ctype##_next(&it))
#define c_foreach_4(it, ctype, start, finish) \
    for (ctype##_iter_t it = start, it##_end_ = finish; it.get != it##_end_.get; ctype##_next(&it))

#define c_push_items(self, ctype, ...) do { \
    const ctype##_input_t __arr[] = __VA_ARGS__; \
    ctype##_push_n(self, __arr, sizeof(__arr)/sizeof(__arr[0])); \
} while (0)

#define c_apply(array, type, value, n) do { \
    type __val = value, *__i = array, *__last = __i + (n); \
    while (__i != __last) = *__i++ = __val; \
} while (0)

#define c_destroy(ctype, ...) do { \
    struct ctype* __arr[] = {__VA_ARGS__}; \
    for (size_t i=0; i<sizeof(__arr)/sizeof(__arr[0]); ++i) \
        ctype##_destroy(__arr[i]); \
} while (0)

#endif
