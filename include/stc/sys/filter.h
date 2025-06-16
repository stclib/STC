/* MIT License
 *
 * Copyright (c) 2025 Tyge Løvset
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
#define T Vec, int
#include <stc/cstack.h>
#include <stc/algorithm.h>

int main(void)
{
    Vec vec = c_make(Vec, {1, 2, 3, 4, 5, 6, 7, 8, 1, 2, 3, 9, 10, 11, 12, 5});

    c_filter(Vec, vec, true
        && c_flt_skipwhile(*value < 3)  // skip leading values < 3
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
// IWYU pragma: private, include "stc/algorithm.h"
#ifndef STC_FILTER_H_INCLUDED
#define STC_FILTER_H_INCLUDED

#include "../common.h"

// ------- c_filter --------
#define c_flt_take(n) _flt_take(&fltbase, n)
#define c_flt_skip(n) (c_flt_counter() > (n))
#define c_flt_takewhile(pred) _flt_takewhile(&fltbase, pred)
#define c_flt_skipwhile(pred) (fltbase.sb[fltbase.sb_top++] |= !(pred))
#define c_flt_counter() (++fltbase.sn[++fltbase.sn_top])
#define c_flt_getcount() (fltbase.sn[fltbase.sn_top])
#define c_flt_map(expr) (_mapped = (expr), value = &_mapped)
#define c_flt_src _it.ref

#define c_filter(C, cnt, pred) \
    _c_filter(C, C##_begin(&cnt), _, pred)

#define c_filter_from(C, start, pred) \
    _c_filter(C, start, _, pred)

#define c_filter_reverse(C, cnt, pred) \
    _c_filter(C, C##_rbegin(&cnt), _r, pred)

#define c_filter_reverse_from(C, start, pred) \
    _c_filter(C, start, _r, pred)

#define _c_filter(C, start, rev, pred) do { \
    struct _flt_base fltbase = {0}; \
    C##_iter _it = start; \
    C##_value *value = _it.ref, _mapped = {0}; \
    for ((void)_mapped ; !fltbase.done & (_it.ref != NULL) ; \
         C##rev##next(&_it), value = _it.ref, fltbase.sn_top=0, fltbase.sb_top=0) \
      (void)(pred); \
} while (0)

// ------- c_filter_zip --------
#define c_filter_zip(...) c_MACRO_OVERLOAD(c_filter_zip, __VA_ARGS__)
#define c_filter_zip_4(C, cnt1, cnt2, pred) \
    c_filter_zip_5(C, cnt1, C, cnt2, pred)
#define c_filter_zip_5(C1, cnt1, C2, cnt2, pred) \
    _c_filter_zip(C1, C1##_begin(&cnt1), C2, C2##_begin(&cnt2), _, pred)

#define c_filter_reverse_zip(...) c_MACRO_OVERLOAD(c_filter_reverse_zip, __VA_ARGS__)
#define c_filter_reverse_zip_4(C, cnt1, cnt2, pred) \
    c_filter_reverse_zip_5(C, cnt1, C, cnt2, pred)
#define c_filter_reverse_zip_5(C1, cnt1, C2, cnt2, pred) \
    _c_filter_zip(C1, C1##_rbegin(&cnt1), C2, C2##_rbegin(&cnt2), _r, pred)

#define c_filter_pairwise(C, cnt, pred) \
    _c_filter_zip(C, C##_begin(&cnt), C, C##_advance(_it1, 1), _, pred)

#define c_flt_map1(expr) (_mapped1 = (expr), value1 = &_mapped1)
#define c_flt_map2(expr) (_mapped2 = (expr), value2 = &_mapped2)
#define c_flt_src1 _it1.ref
#define c_flt_src2 _it2.ref

#define _c_filter_zip(C1, start1, C2, start2, rev, pred) do { \
    struct _flt_base fltbase = {0}; \
    C1##_iter _it1 = start1; \
    C2##_iter _it2 = start2; \
    C1##_value* value1 = _it1.ref, _mapped1; (void)_mapped1; \
    C2##_value* value2 = _it2.ref, _mapped2; (void)_mapped2; \
    for (; !fltbase.done & (_it1.ref != NULL) & (_it2.ref != NULL); \
         C1##rev##next(&_it1), value1 = _it1.ref, C2##rev##next(&_it2), value2 = _it2.ref, \
         fltbase.sn_top=0, fltbase.sb_top=0) \
      (void)(pred); \
} while (0)

// ------- c_ffilter --------
// c_ffilter allows to execute imperative statements for each element
// in a for-loop, e.g., calling nested generic statements instead
// of defining a function/expression for it:
/*
    Vec vec = ..., vec2 = ...;
    for (c_ffilter(i, Vec, vec, true
        && c_fflt_skipwhile(i, *i.ref < 3)  // skip leading values < 3
        && (*i.ref & 1) == 1                // then use odd values only
        && c_fflt_map(i, *i.ref * 2)        // multiply by 2
        && c_fflt_takewhile(i, *i.ref < 20) // stop if mapped *i.ref >= 20
    )){
        c_eraseremove_if(Vec, &vec2, *value == *i.ref);
    }
*/
#define c_fflt_take(i, n) _flt_take(&i.base, n)
#define c_fflt_skip(i, n) (c_fflt_counter(i) > (n))
#define c_fflt_takewhile(i, pred) _flt_takewhile(&i.base, pred)
#define c_fflt_skipwhile(i, pred) (i.base.sb[i.base.sb_top++] |= !(pred))
#define c_fflt_counter(i) (++i.base.sn[++i.base.sn_top])
#define c_fflt_getcount(i) (i.base.sn[i.base.sn_top])
#define c_fflt_map(i, expr) (i.mapped = (expr), i.ref = &i.mapped)
#define c_fflt_src(i) i.iter.ref

#define c_forfilter(...) for (c_ffilter(__VA_ARGS__))
#define c_forfilter_from(...) for (c_ffilter_from(__VA_ARGS__))
#define c_forfilter_reverse(...) for (c_ffilter_reverse(__VA_ARGS__))
#define c_forfilter_reverse_from(...) for (c_ffilter_reverse_from(__VA_ARGS__))

#define c_ffilter(i, C, cnt, pred) \
    _c_ffilter(i, C, C##_begin(&cnt), _, pred)

#define c_ffilter_from(i, C, start, pred) \
    _c_ffilter(i, C, start, _, pred)

#define c_ffilter_reverse(i, C, cnt,pred) \
    _c_ffilter(i, C, C##_rbegin(&cnt), _r, pred)

#define c_ffilter_reverse_from(i, C, start, pred) \
    _c_ffilter(i, C, start, _r, pred)

#define _c_ffilter(i, C, start, rev, pred) \
    struct {C##_iter iter; C##_value *ref, mapped; struct _flt_base base;} \
    i = {.iter=start, .ref=i.iter.ref} ; !i.base.done & (i.iter.ref != NULL) ; \
    C##rev##next(&i.iter), i.ref = i.iter.ref, i.base.sn_top=0, i.base.sb_top=0) \
      if (!(pred)) ; else if (1

// ------------------------ private -------------------------
#ifndef c_NFILTERS
#define c_NFILTERS 20
#endif

struct _flt_base {
    uint8_t sn_top, sb_top;
    bool done, sb[c_NFILTERS];
    uint32_t sn[c_NFILTERS];
};

static inline bool _flt_take(struct _flt_base* base, uint32_t n) {
    uint32_t k = ++base->sn[++base->sn_top];
    base->done |= (k >= n);
    return n > 0;
}

static inline bool _flt_takewhile(struct _flt_base* base, bool pred) {
    bool skip = (base->sb[base->sb_top++] |= !pred);
    base->done |= skip;
    return !skip;
}

#endif // STC_FILTER_H_INCLUDED
