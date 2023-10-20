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
/* Generic Quicksort in C, performs as fast as c++ std::sort().
template params:
#define i_key           - value type [required]
#define i_less          - less function. default: *x < *y
#define i_type name     - define {{name}}_sort_n(), else {{i_key}}array_sort_n().

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

    const int* found = ints_binary_search(nums, 25, c_arraylen(nums));
    if (found) printf("found: %d\n", *found);

    found = ints_lower_bound(nums, 200, c_arraylen(nums));
    if (found) printf("found lower 200: %d\n", *found);
}

// ex2: Test on a cdeque !!
#include <stdio.h>
#define i_type IDeq
#define i_key int
#define i_opt c_use_cmp | c_more // retain input template params to be reused by sort.h
#include "stc/cdeq.h"
#include "stc/algo/quicksort.h"

int main(void) {
    IDeq nums = c_init(IDeq, {5434, 25, 245, 1, 654, 33, 543, 21});
    IDeq_push_front(&nums, 23);
    IDeq_push_front(&nums, 321);
    
    IDeq_quicksort(&nums);

    c_foreach (i, IDeq, nums)
        printf(" %d", *i.ref);
    puts("");

    const int* found = IDeq_binary_search(&nums, 25);
    if (found) printf("found: %d\n", *found);

    found = IDeq_lower_bound(&nums, 200);
    if (found) printf("found lower 200: %d\n", *found);

    IDeq_drop(&nums);
}
*/
#include "../ccommon.h"

#if !defined i_key && defined i_val
  #define i_key i_val
#endif
#ifndef i_type
  #define _i_is_arr
  #define i_at(arr, idx) (&arr[idx])
  #define i_at_mut i_at
  #ifndef i_tag
    #define i_tag i_key
  #endif
  #define i_type c_JOIN(i_tag, s)
  typedef i_key i_type, c_JOIN(i_type, _value), c_JOIN(i_type, _raw);
#else
  #define i_at(arr, idx) _cx_MEMB(_at)(arr, idx)
  #define i_at_mut(arr, idx) _cx_MEMB(_at_mut)(arr, idx)
#endif
#include "../priv/template.h"

// quick sort

static inline void _cx_MEMB(_insertsort_ij)(i_type* arr, intptr_t lo, intptr_t hi) {
    for (intptr_t j = lo, i = lo + 1; i <= hi; j = i, ++i) {
        _cx_value x = *i_at(arr, i);
        _cx_raw rx = i_keyto(&x);
        while (j >= 0) {
            _cx_raw ry = i_keyto(i_at(arr, j));
            if (!(i_less((&rx), (&ry)))) break;
            *i_at_mut(arr, j + 1) = *i_at(arr, j);
            --j;
        }
        *i_at_mut(arr, j + 1) = x;
    }
}

static inline void _cx_MEMB(_quicksort_ij)(i_type* arr, intptr_t lo, intptr_t hi) {
    intptr_t i = lo, j;
    while (lo < hi) {
        _cx_raw pivot = i_keyto(i_at(arr, lo + (hi - lo)*7/16)), rx;
        j = hi;

        while (i <= j) {
            do { rx = i_keyto(i_at(arr, i)); } while (i_less((&rx), (&pivot)) && ++i);
            do { rx = i_keyto(i_at(arr, j)); } while (i_less((&pivot), (&rx)) && --j);
            if (i <= j) {
                c_swap(i_key, i_at_mut(arr, i), i_at_mut(arr, j));
                ++i; --j;
            }
        }
        if (j - lo > hi - i) {
            c_swap(intptr_t, &lo, &i);
            c_swap(intptr_t, &hi, &j);
        }

        if (j - lo > 64) _cx_MEMB(_quicksort_ij)(arr, lo, j);
        else if (j > lo) _cx_MEMB(_insertsort_ij)(arr, lo, j);
        lo = i;
    }
}

// lower bound

static inline intptr_t // -1 = not found
_cx_MEMB(_lower_bound_range)(const i_type* arr, const _cx_raw raw, intptr_t first, intptr_t last) {
    intptr_t step, count = last - first;

    while (count > 0) {
        intptr_t it = first;
        step = count / 2;
        it += step;

        const _cx_raw rx = i_keyto(i_at(arr, it));
        if (i_less((&rx), (&raw))) {
            first = ++it;
            count -= step + 1;
        } else
            count = step;
    }
    return first == last ? -1 : first;
}

// binary search

static inline intptr_t // -1 = not found
_cx_MEMB(_binary_search_range)(const i_type* arr, const _cx_raw raw, intptr_t first, intptr_t last) {
    intptr_t res = _cx_MEMB(_lower_bound_range)(arr, raw, first, last);
    if (res != -1) {
        const _cx_raw rx = i_keyto(i_at(arr, res));
        if (i_less((&raw), (&rx))) res = -1;
    }
    return res;
}

#ifdef _i_is_arr

static inline void _cx_MEMB(_quicksort)(i_type* arr, intptr_t n)
    { _cx_MEMB(_quicksort_ij)(arr, 0, n - 1); }

static inline intptr_t // -1 = not found
_cx_MEMB(_lower_bound)(const i_type* arr, const _cx_raw raw, intptr_t n)
    { return _cx_MEMB(_lower_bound_range)(arr, raw, 0, n); }

static inline intptr_t // -1 = not found
_cx_MEMB(_binary_search)(const i_type* arr, const _cx_raw raw, intptr_t n)
    { return _cx_MEMB(_binary_search_range)(arr, raw, 0, n); }

#else

static inline void _cx_MEMB(_quicksort)(i_type* arr)
    { _cx_MEMB(_quicksort_ij)(arr, 0, _cx_MEMB(_size)(arr) - 1); }

static inline intptr_t // -1 = not found
_cx_MEMB(_lower_bound)(const i_type* arr, const _cx_raw raw)
    { return _cx_MEMB(_lower_bound_range)(arr, raw, 0, _cx_MEMB(_size)(arr)); }

static inline intptr_t // -1 = not found
_cx_MEMB(_binary_search)(const i_type* arr, const _cx_raw raw)
    { return _cx_MEMB(_binary_search_range)(arr, raw, 0, _cx_MEMB(_size)(arr)); }

#endif

#include "../priv/template2.h"
#undef _i_is_arr
#undef i_at
#undef i_at_mut
