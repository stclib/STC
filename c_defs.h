// MIT License
//
// Copyright (c) 2020 Tyge Løvset, NORCE, www.norceresearch.no
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef C_DEFS__H__
#define C_DEFS__H__

#include <stdint.h>
#include <stdbool.h>

// Macro overloading feature support: https://rextester.com/ONP80107
#define c_defs_CAT( A, B ) A ## B
#define c_defs_EXPAND(...) __VA_ARGS__
#define c_defs_VA_ARG_SIZE(...) c_defs_EXPAND(c_defs_APPLY_ARG_N((__VA_ARGS__, c_defs_RSEQ_N)))
#define c_defs_APPLY_ARG_N(ARGS) c_defs_EXPAND(c_defs_ARG_N ARGS)
#define c_defs_ARG_N(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, N,...) N
#define c_defs_RSEQ_N 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define c_defs_OVERLOAD_SELECT(NAME, NUM) c_defs_CAT( NAME ## _, NUM)

#define c_defs_MACRO_OVERLOAD(NAME, ...) c_defs_OVERLOAD_SELECT(NAME, c_defs_VA_ARG_SIZE(__VA_ARGS__))(__VA_ARGS__)
// #define foo(...)     c_defs_MACRO_OVERLOAD(foo, __VA_ARGS__)
// #define foo_1(X)     foo_2(X, 100)
// #define foo_2(X, Y)  X + Y

#define                 c_defs_max_alloca   (1000)
#define                 c_defs_swap(T, x, y) { T __t = x; x = y; y = __t; }

#define                 c_defs_initRaw(x) (x)
#define                 c_defs_getRaw(x) (x)
static inline void      c_defs_destroy(void* value) {}

#define                 c_foreach(it, ctag, con) \
                            for (ctag##_iter_t it = ctag##_begin(con); it.item != ctag##_end(con).item; it = ctag##_next(it))

static inline uint32_t  c_defs_murmurHash(const void *data, size_t len) { // One-at-a-time 32bit
    const unsigned char *key = (const unsigned char *) data;
    uint32_t h = 0xC613FC15; // ‭0x749E3E6989DF617‬;  64bit
    while (len--) {
        h ^= *key++;
        h *= 0x5bd1e995;     // 0x5bd1e9955bd1e995; 64bit
        h ^= h >> 15;        // 47;                 64bit
    }
    return h;
}

#endif