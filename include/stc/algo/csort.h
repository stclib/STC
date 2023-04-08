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
#include "../ccommon.h"
#include "../priv/template.h"

/* Generic Quicksort in C, performs as fast as c++ std::sort().
template params:
#define i_val           - value type [required]
#define i_less          - less function. default: *x < *y
#define i_tag NAME      - define csort_NAME(). default {i_val}

// test:
#include <stdio.h>
#define i_val int
#include <stc/algo/csort.h>

int main() {
    int arr[] = {23, 321, 5434, 25, 245, 1, 654, 33, 543, 21};
    
    csort_int(arr, c_arraylen(arr));

    for (int i = 0; i < c_arraylen(arr); i++)
        printf(" %d", arr[i]);
    puts("");
}
*/

typedef i_val c_PASTE(c_CONCAT(csort_, i_tag), _value);

static inline void c_PASTE(cisort_, i_tag)(i_val arr[], intptr_t lo, intptr_t hi) {
    for (intptr_t j = lo, i = lo + 1; i <= hi; j = i, ++i) {
        i_val key = arr[i];
        while (j >= 0 && (i_less((&key), (&arr[j])))) {
            arr[j + 1] = arr[j];
            --j;
        }
        arr[j + 1] = key;
    }
}

static inline void c_PASTE(cqsort_, i_tag)(i_val arr[], intptr_t lo, intptr_t hi) {
    intptr_t i = lo, j;
    while (lo < hi) {
        i_val pivot = arr[lo + (hi - lo)*7/16];
        j = hi;

        while (i <= j) {
            while (i_less((&arr[i]), (&pivot))) ++i;
            while (i_less((&pivot), (&arr[j]))) --j;
            if (i <= j) {
                c_swap(i_val, arr+i, arr+j);
                ++i; --j;
            }
        }
        if (j - lo > hi - i) {
            c_swap(intptr_t, &lo, &i);
            c_swap(intptr_t, &hi, &j);
        }

        if (j - lo > 64) c_PASTE(cqsort_, i_tag)(arr, lo, j);
        else if (j > lo) c_PASTE(cisort_, i_tag)(arr, lo, j);
        lo = i;
    }
}

static inline void c_PASTE(csort_, i_tag)(i_val arr[], intptr_t n)
    { c_PASTE(cqsort_, i_tag)(arr, 0, n - 1); }

#include "../priv/template2.h"
