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
#define i_val int
#include <stc/cstack.h>
#include <stc/calgo.h>

int main()
{
    cstack_int stk = c_make(cstack_int, {1, 2, 3, 4, 5, 6, 7, 8, 9});

    c_foreach (i, cstack_int, stk)
        printf(" %d", *i.ref);
    puts("");
   
    c_forfilter (i, cstack_int, stk,
                    c_flt_skipwhile(i, *i.ref < 3) &&
                    (*i.ref & 1) == 0              && // even only
                    c_flt_take(i, 2))                 // break after 2
        printf(" %d", *i.ref);
    puts("");

    cstack_int_drop(&stk);
}
*/
#ifndef STC_FILTER_H_INCLUDED
#define STC_FILTER_H_INCLUDED

#include <stc/ccommon.h>

// c_forfilter:

#define c_flt_skip(i, n) (c_flt_counter(i) > (n))
#define c_flt_skipwhile(i, pred) ((i).b.s2[(i).b.s2top++] |= !(pred))
#define c_flt_take(i, n) _flt_take(&(i).b, n)
#define c_flt_takewhile(i, pred) _flt_takewhile(&(i).b, pred)
#define c_flt_counter(i) ++(i).b.s1[(i).b.s1top++]
#define c_flt_getcount(i) (i).b.s1[(i).b.s1top - 1]

#define c_forfilter(i, C, cnt, filter) \
    c_forfilter_it(i, C, C##_begin(&cnt), filter)

#define c_forfilter_it(i, C, start, filter) \
    for (struct {struct _flt_base b; C##_iter it; C##_value *ref;} \
         i = {.it=start, .ref=i.it.ref} ; !i.b.done & (i.it.ref != NULL) ; \
         C##_next(&i.it), i.ref = i.it.ref, i.b.s1top=0, i.b.s2top=0) \
      if (!(filter)) ; else


// c_find_if, c_erase_if, c_eraseremove_if:

#define c_find_if(...) c_MACRO_OVERLOAD(c_find_if, __VA_ARGS__)
#define c_find_if_4(it, C, cnt, pred) do { \
    intptr_t _index = 0; \
    for (it = C##_begin(&cnt); it.ref && !(pred); C##_next(&it)) \
        ++_index; \
} while (0)

#define c_find_if_5(it, C, start, end, pred) do { \
    intptr_t _index = 0; \
    const C##_value* _endref = (end).ref; \
    for (it = start; it.ref != _endref && !(pred); C##_next(&it)) \
        ++_index; \
    if (it.ref == _endref) it.ref = NULL; \
} while (0)


// Use with: clist, cmap, cset, csmap, csset:
#define c_erase_if(it, C, cnt, pred) do { \
    C* _cnt = &cnt; \
    intptr_t _index = 0; \
    for (C##_iter it = C##_begin(_cnt); it.ref; ++_index) { \
        if (pred) it = C##_erase_at(_cnt, it); \
        else C##_next(&it); \
    } \
} while (0)


// Use with: cstack, cvec, cdeq, cqueue:
#define c_eraseremove_if(it, C, cnt, pred) do { \
    C* _cnt = &cnt; \
    intptr_t _n = 0, _index = 0; \
    C##_iter it = C##_begin(_cnt), _i; \
    while (it.ref && !(pred)) \
        C##_next(&it), ++_index; \
    for (_i = it; it.ref; C##_next(&it), ++_index) \
        if (pred) C##_value_drop(it.ref), ++_n; \
        else *_i.ref = *it.ref, C##_next(&_i); \
    _cnt->_len -= _n; \
} while (0)

// ------------------------ private -------------------------
#ifndef c_NFILTERS
#define c_NFILTERS 32
#endif

struct _flt_base {
    uint32_t s1[c_NFILTERS];
    bool s2[c_NFILTERS], done;
    uint8_t s1top, s2top;
};

static inline bool _flt_take(struct _flt_base* b, uint32_t n) {
    uint32_t k = ++b->s1[b->s1top++];
    b->done |= (k >= n);
    return k <= n;
}

static inline bool _flt_takewhile(struct _flt_base* b, bool pred) {
    bool skip = (b->s2[b->s2top++] |= !pred);
    b->done |= skip;
    return !skip;
}

#endif
