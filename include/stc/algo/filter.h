/* MIT License
 *
 * Copyright (c) 2023 Tyge Løvset
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
#include <stdio.h>
#define i_TYPE Vec, int
#include "stc/cstack.h"
#include "stc/algorithm.h"

int main(void)
{
    Vec vec = c_init(Vec, {1, 2, 3, 4, 5, 6, 7, 8, 1, 2, 3, 9, 10, 11, 12, 5});

    c_filter(Vec, vec
         , c_flt_skipwhile(*value < 3)  // skip leading values < 3
        && (*value & 1) == 1            // then use odd values only
        && c_flt_map(*value * 2)        // multiply by 2
        && c_flt_takewhile(*value < 20) // stop if mapped *value >= 20
        && printf(" %d", *value)        // print value
    );
    //  6 10 14 2 6 18
    puts("");
    Vec_drop(&vec);
}
*/
#ifndef STC_TRANSFORM_H_INCLUDED
#define STC_TRANSFORM_H_INCLUDED

#include "../common.h"

// c_filter:

#define c_flt_skip(n) (c_flt_counter() > (n))
#define c_flt_take(n) _flt_take(&_fl, n)
#define c_flt_skipwhile(pred) (_fl.sb[_fl.sb_top++] |= !(pred))
#define c_flt_takewhile(pred) _flt_takewhile(&_fl, pred)
#define c_flt_counter() (++_fl.sn[++_fl.sn_top])
#define c_flt_getcount() (_fl.sn[_fl.sn_top])
#define c_flt_map(expr) (_mapped = (expr), value = &_mapped)
#define c_flt_src _it.ref

#define c_filter(C, cnt, ...) \
    c_filter_from(C, C##_begin(&cnt), __VA_ARGS__)

#define c_filter_from(C, start, ...) do { \
    struct _flt_base _fl = {0}; \
    C##_iter _it = start; \
    C##_value *value = _it.ref, _mapped; \
    for ((void)_mapped ; !_fl.done & (_it.ref != NULL) ; \
         C##_next(&_it), value = _it.ref, _fl.sn_top=0, _fl.sb_top=0) \
      (void)(__VA_ARGS__); \
} while (0)

// ------------------------ private -------------------------
#ifndef c_NFILTERS
#define c_NFILTERS 32
#endif

struct _flt_base {
    uint8_t sn_top, sb_top;
    bool done, sb[c_NFILTERS];
    uint32_t sn[c_NFILTERS];
};

static inline bool _flt_take(struct _flt_base* fl, uint32_t n) {
    uint32_t k = ++fl->sn[++fl->sn_top];
    fl->done |= (k >= n);
    return n > 0;
}

static inline bool _flt_takewhile(struct _flt_base* fl, bool pred) {
    bool skip = (fl->sb[fl->sb_top++] |= !pred);
    fl->done |= skip;
    return !skip;
}

#endif
