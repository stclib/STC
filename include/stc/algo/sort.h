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
#include <stc/algo/sort.h>

int main(void) {
    int nums[] = {23, 321, 5434, 25, 245, 1, 654, 33, 543, 21};
    
    intarray_sort_n(nums, c_arraylen(nums));

    for (int i = 0; i < c_arraylen(nums); i++)
        printf(" %d", nums[i]);
    puts("");
}

// ex2:
#define i_key int
#define i_type IDeq
#define i_more // retain input template params to be reused by sort.h
#include <stc/cdeq.h>
#include <stc/algo/sort.h>

int main(void) {
    IDeq nums = c_init(IDeq, {5434, 25, 245, 1, 654, 33, 543, 21});
    IDeq_push_front(&nums, 23);
    IDeq_push_front(&nums, 321);
    
    IDeq_sort_n(&nums, IDeq_size(&nums));

    c_foreach (i, IDeq, nums)
        printf(" %d", *i.ref);
    IDeq_drop(&nums);
}
*/
#include "../ccommon.h"

#if !defined i_key && defined i_val
  #define i_key i_val
#endif
#ifndef i_type
  #define i_at(arr, idx) (&arr[idx])
  #ifndef i_tag
    #define i_tag i_key
  #endif
  #define i_type c_PASTE(i_tag, s)
  typedef i_key i_type;
#endif
#ifndef i_at
  #define i_at(arr, idx) _cx_MEMB(_at_mut)(arr, idx)
#endif
#include "../priv/template.h"


static inline void _cx_MEMB(_insertsort_ij)(_cx_Self* arr, intptr_t lo, intptr_t hi) {
    for (intptr_t j = lo, i = lo + 1; i <= hi; j = i, ++i) {
        i_key key = *i_at(arr, i);
        while (j >= 0 && (i_less((&key), i_at(arr, j)))) {
            *i_at(arr, j + 1) = *i_at(arr, j);
            --j;
        }
        *i_at(arr, j + 1) = key;
    }
}

static inline void _cx_MEMB(_sort_ij)(_cx_Self* arr, intptr_t lo, intptr_t hi) {
    intptr_t i = lo, j;
    while (lo < hi) {
        i_key pivot = *i_at(arr, lo + (hi - lo)*7/16);
        j = hi;

        while (i <= j) {
            while (i_less(i_at(arr, i), (&pivot))) ++i;
            while (i_less((&pivot), i_at(arr, j))) --j;
            if (i <= j) {
                c_swap(i_key, i_at(arr, i), i_at(arr, j));
                ++i; --j;
            }
        }
        if (j - lo > hi - i) {
            c_swap(intptr_t, &lo, &i);
            c_swap(intptr_t, &hi, &j);
        }

        if (j - lo > 64) _cx_MEMB(_sort_ij)(arr, lo, j);
        else if (j > lo) _cx_MEMB(_insertsort_ij)(arr, lo, j);
        lo = i;
    }
}

static inline void _cx_MEMB(_sort_n)(_cx_Self* arr, intptr_t len) {
    _cx_MEMB(_sort_ij)(arr, 0, len - 1);
}

#include "../priv/template2.h"
#undef i_at
