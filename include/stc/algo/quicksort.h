/* MIT License
 *
 * Copyright (c) 2024 Tyge Løvset
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
/* Generic Quicksort in C, performs as fast as c++ std::sort().
template params:
#define i_key           - value type [required]
#define i_less          - optional less function. default: *x < *y
#define i_type name     - optional, define {{name}}_quicksort(), else {{i_key}}s_quicksort().
#define i_type name,key - alternative way to define both i_type and i_key.

// ex1:
#include <stdio.h>
#define i_key int
#include "stc/algo/quicksort.h"

int main(void) {
    int nums[] = {23, 321, 5434, 25, 245, 1, 654, 33, 543, 21};

    ints_quicksort(nums, c_arraylen(nums));

    for (int i = 0; i < c_arraylen(nums); i++)
        printf(" %d", nums[i]);
    puts("");

    isize idx = ints_binary_search(nums, 25, c_arraylen(nums));
    if (idx != -1) printf("found: %d\n", nums[idx]);

    idx = ints_lower_bound(nums, 200, c_arraylen(nums));
    if (idx != -1) printf("found lower 200: %d\n", nums[idx]);
}

// ex2: Test on a deque !!
#include <stdio.h>
#define i_type IDeq
#define i_key int
#define i_opt c_use_cmp | c_more // retain input template params to be reused by quicksort.h
#include "stc/deq.h"
#include "stc/algo/quicksort.h"

int main(void) {
    IDeq nums = c_init(IDeq, {5434, 25, 245, 1, 654, 33, 543, 21});
    IDeq_push_front(&nums, 23);
    IDeq_push_front(&nums, 321);

    IDeq_quicksort(&nums);

    c_foreach (i, IDeq, nums)
        printf(" %d", *i.ref);
    puts("");

    isize idx = IDeq_binary_search(&nums, 25);
    if (idx != -1) printf("found: %d\n", *IDeq_at(&nums, idx));

    idx = IDeq_lower_bound(&nums, 200);
    if (idx != -1) printf("found lower 200: %d\n", *IDeq_at(&nums, idx));

    IDeq_drop(&nums);
}
*/
#include "../common.h"

#ifndef _i_template
  #define _i_is_arr
  #if !defined i_key
    #error "i_key not defined"
  #elif !defined i_type
    #define i_type c_JOIN(i_key, s)
  #endif
  typedef i_key i_type, c_JOIN(i_type, _value), c_JOIN(i_type, _raw);
  #define i_at(arr, idx) (&(arr)[idx])
  #define i_at_mut i_at
#else
  #define i_at(self, idx) _c_MEMB(_at)(self, idx)
  #define i_at_mut(self, idx) _c_MEMB(_at_mut)(self, idx)
#endif
#include "../priv/template.h"

// quick sort

static inline void _c_MEMB(_insertsort_ij)(Self* self, isize lo, isize hi) {
    for (isize j = lo, i = lo + 1; i <= hi; j = i, ++i) {
        _m_value x = *i_at(self, i);
        _m_raw rx = i_keytoraw((&x));
        while (j >= 0) {
            _m_raw ry = i_keytoraw(i_at(self, j));
            if (!(i_less((&rx), (&ry)))) break;
            *i_at_mut(self, j + 1) = *i_at(self, j);
            --j;
        }
        *i_at_mut(self, j + 1) = x;
    }
}

static inline void _c_MEMB(_quicksort_ij)(Self* self, isize lo, isize hi) {
    isize i = lo, j;
    while (lo < hi) {
        _m_raw pivot = i_keytoraw(i_at(self, (isize)(lo + (hi - lo)*7LL/16))), rx;
        j = hi;
        do {
            do { rx = i_keytoraw(i_at(self, i)); } while (i_less((&rx), (&pivot)) && ++i);
            do { rx = i_keytoraw(i_at(self, j)); } while (i_less((&pivot), (&rx)) && --j);
            if (i > j) break;
            c_swap(i_at_mut(self, i), i_at_mut(self, j));
            ++i; --j;
        } while (i <= j);

        if (j - lo > hi - i) {
            c_swap(&lo, &i);
            c_swap(&hi, &j);
        }
        if (j - lo > 64) _c_MEMB(_quicksort_ij)(self, lo, j);
        else if (j > lo) _c_MEMB(_insertsort_ij)(self, lo, j);
        lo = i;
    }
}

// lower bound

static inline isize // -1 = not found
_c_MEMB(_lower_bound_range)(const Self* self, const _m_raw raw, isize first, isize last) {
    isize count = last - first, step = count/2;

    while (count > 0) {
        isize it = first;
        it += step;

        const _m_raw rx = i_keytoraw(i_at(self, it));
        if (i_less((&rx), (&raw))) {
            first = ++it;
            count -= step + 1;
            step = count*3/4;
        } else {
            count = step;
            step = count/4;
        }
    }
    return first == last ? -1 : first;
}

// binary search

static inline isize // -1 = not found
_c_MEMB(_binary_search_range)(const Self* self, const _m_raw raw, isize first, isize last) {
    isize res = _c_MEMB(_lower_bound_range)(self, raw, first, last);
    if (res != -1) {
        const _m_raw rx = i_keytoraw(i_at(self, res));
        if (i_less((&raw), (&rx))) res = -1;
    }
    return res;
}

#ifdef _i_is_arr

static inline void _c_MEMB(_quicksort)(Self* arr, isize n)
    { _c_MEMB(_quicksort_ij)(arr, 0, n - 1); }

static inline isize // -1 = not found
_c_MEMB(_lower_bound)(const Self* arr, const _m_raw raw, isize n)
    { return _c_MEMB(_lower_bound_range)(arr, raw, 0, n); }

static inline isize // -1 = not found
_c_MEMB(_binary_search)(const Self* arr, const _m_raw raw, isize n)
    { return _c_MEMB(_binary_search_range)(arr, raw, 0, n); }

#else

static inline void _c_MEMB(_quicksort)(Self* self)
    { _c_MEMB(_quicksort_ij)(self, 0, _c_MEMB(_size)(self) - 1); }

static inline isize // -1 = not found
_c_MEMB(_lower_bound)(const Self* self, const _m_raw raw)
    { return _c_MEMB(_lower_bound_range)(self, raw, 0, _c_MEMB(_size)(self)); }

static inline isize // -1 = not found
_c_MEMB(_binary_search)(const Self* self, const _m_raw raw)
    { return _c_MEMB(_binary_search_range)(self, raw, 0, _c_MEMB(_size)(self)); }

#endif

#undef _i_is_arr
#undef i_at
#undef i_at_mut
#include "../priv/template2.h"
