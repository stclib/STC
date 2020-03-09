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

#ifndef CDEF__H__
#define CDEF__H__

// Macro overloading feature support: https://rextester.com/ONP80107
#define cdef_CAT( A, B ) A ## B
#define cdef_EXPAND(...) __VA_ARGS__
#define cdef_VA_ARG_SIZE(...) cdef_EXPAND(cdef_APPLY_ARG_N((__VA_ARGS__, cdef_RSEQ_N)))
#define cdef_APPLY_ARG_N(ARGS) cdef_EXPAND(cdef_ARG_N ARGS)
#define cdef_ARG_N(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, N,...) N
#define cdef_RSEQ_N 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define cdef_OVERLOAD_SELECT(NAME, NUM) cdef_CAT( NAME ## _, NUM)

#define cdef_MACRO_OVERLOAD(NAME, ...) cdef_OVERLOAD_SELECT(NAME, cdef_VA_ARG_SIZE(__VA_ARGS__))(__VA_ARGS__)
// #define foo(...)     cdef_MACRO_OVERLOAD(foo, __VA_ARGS__)
// #define foo_1(X)     foo_2(X, 100)
// #define foo_2(X, Y)  X + Y

#define             _cdef_max_alloca   (1000)
#define             _cdef_swap(T, x, y) { T __t = x; x = y; y = __t; }

#define             cdef_initRaw(x) (x)
#define             cdef_getRaw(x) (x)
static inline void  cdef_destroy(void* value) {}

#define             cforeach(it, ctag, con) \
                        for (ctag##_iter_t it = ctag##_begin(con); it.item != ctag##_end(con).item; it = ctag##_next(it))

static inline uint32_t 
cdef_murmurHash(const void *data, size_t len) // One-at-a-time 32bit
{
    const unsigned char *key = (const unsigned char *) data;
    uint32_t h = 3323198485ul;
    while (len--) {
        h ^= *key++;
        h *= 0x5bd1e995;
        h ^= h >> 15;
    }
    return h;
}


#endif