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
#include <stc/priv/template.h>
#include <stc/ccommon.h>

/* Generic Quicksort in C, performs as fast as c++ std::sort().
template params:
#define i_val           - value type [required]
#define i_less          - less function. default: *x < *y
#define i_tag NAME      - define csort_NAME(). default {i_val}

// test:
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#define i_val int
#include <stc/algo/csort.h>
#include <stc/crandom.h>
#ifdef __cplusplus
#include <algorithm>
#endif

void testsort(csort_int_value *a, size_t size, const char *desc) {
    clock_t t = clock();
    csort_int(a, size);
    t = clock() - t;

    printf("%s: %zu elements sorted in %.3fms\n",
           desc, size, t*1000.0/CLOCKS_PER_SEC);
}

int main() {
    size_t i, size = 10000000;
    csort_int_value *a = (csort_int_value*)malloc(sizeof(*a) * size);
    if (a != NULL) {
        for (i = 0; i < size; i++)
            a[i] = crandom() & (1U << 28) - 1;
        testsort(a, size, "random");
        for (i = 0; i < 20; i++) printf(" %d", a[i]);
        puts("");
        free(a);
    }
}*/
typedef i_val c_PASTE(c_PASTE(csort_, i_tag), _value);

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

static inline void c_PASTE(csort_, i_tag)(i_val arr[], size_t n)
    { c_PASTE(cqsort_, i_tag)(arr, 0, (intptr_t)n - 1); }

#include <stc/priv/template.h>
