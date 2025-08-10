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
/* Generic Quicksort in C, performs as fast as c++ std::sort(), and more robust.
template params:
#define i_key keytype  - [required] (or use i_type, see below)
#define i_less(xp, yp) - optional less function. default: *xp < *yp
#define i_cmp(xp, yp)  - alternative 3-way comparison. c_default_cmp(xp, yp)
#define T name         - optional, defines {name}_sort(), else {i_key}s_sort().
#define T name, key    - alternative one-liner to define both i_type and i_key.

// ex1:
#include <stdio.h>
#define i_key int
#include <stc/sort.h>

int main(void) {
    int nums[] = {23, 321, 5434, 25, 245, 1, 654, 33, 543, 21};

    ints_sort(nums, c_arraylen(nums));

    for (int i = 0; i < c_arraylen(nums); i++)
        printf(" %d", nums[i]);
    puts("");

    isize idx = ints_binary_search(nums, 25, c_arraylen(nums));
    if (idx != c_NPOS) printf("found: %d\n", nums[idx]);

    idx = ints_lower_bound(nums, 200, c_arraylen(nums));
    if (idx != c_NPOS) printf("found lower 200: %d\n", nums[idx]);
}

// ex2: Test on a deque !!
#include <stdio.h>
#define T IDeq, int, (c_use_cmp) // enable comparison functions
#include <stc/deque.h>

int main(void) {
    IDeq nums = c_make(IDeq, {5434, 25, 245, 1, 654, 33, 543, 21});
    IDeq_push_front(&nums, 23);
    IDeq_push_front(&nums, 321);

    IDeq_sort(&nums);

    for (c_each (i, IDeq, nums))
        printf(" %d", *i.ref);
    puts("");

    isize idx = IDeq_binary_search(&nums, 25);
    if (idx != c_NPOS) printf("found: %d\n", *IDeq_at(&nums, idx));

    idx = IDeq_lower_bound(&nums, 200);
    if (idx != c_NPOS) printf("found lower 200: %d\n", *IDeq_at(&nums, idx));

    IDeq_drop(&nums);
}
*/
#ifndef _i_template
  #include "priv/linkage.h"
  #include "common.h"

  #define _i_is_array
  #if defined T && !defined i_type
    #define i_type T
  #endif
  #if defined i_type && !defined i_key
    #define Self c_GETARG(1, i_type)
    #define i_key c_GETARG(2, i_type)
  #elif defined i_type
    #define Self i_type
  #else
    #define Self c_JOIN(i_key, s)
  #endif

  typedef i_key Self;
  typedef Self c_JOIN(Self, _value), c_JOIN(Self, _raw);
  #define i_at(arr, idx) (&(arr)[idx])
  #define i_at_mut i_at
  #include "priv/template.h" // IWYU pragma: keep
#endif

#include "priv/sort_prv.h"

#ifdef _i_is_array
  #undef _i_is_array
  #include "priv/linkage2.h"
  #include "priv/template2.h"
#endif
#undef i_at
#undef i_at_mut
