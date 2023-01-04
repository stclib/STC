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
#ifndef STC_CSORT_H_INCLUDED
#define STC_CSORT_H_INCLUDED
#include <stdint.h>
#define c_CONCAT(a, b) a ## b
#define c_PASTE(a, b) c_CONCAT(a, b)
#endif

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

int testsort(csortval_int *a, size_t size, const char *desc) {
    clock_t t = clock();
#ifdef __cplusplus
    printf("std::sort: ");
    std::sort(a, a + size);
#else
    printf("csort: ");
    csort_int(a, size);
#endif
    t = clock() - t;

    printf("%s: %zu elements sorted in %.3fms\n",
           desc, size, t * 1000.0 / CLOCKS_PER_SEC);
    return 0;
}

int main(int argc, char *argv[]) {
    size_t i, size = argc > 1 ? strtoull(argv[1], NULL, 0) : 10000000;
    csortval_int *a = (csortval_int*)malloc(sizeof(*a) * size);
    if (a != NULL) {
        for (i = 0; i < size; i++)
            a[i] = crandom() & (1U << 28) - 1;
        testsort(a, size, "random");
        for (i = 0; i < 20; i++) printf(" %d", a[i]);
        puts("");
        testsort(a, size, "sorted");
        for (i = 0; i < size; i++) a[i] = size - i;
        testsort(a, size, "reverse sorted");
        for (i = 0; i < size; i++) a[i] = 126735;
        testsort(a, size, "constant");
        free(a);
    }
    return 0;
}*/
#ifndef i_tag
#define i_tag i_val
#endif
#ifndef i_less
#define i_less(x, y) *x < *y
#endif

typedef i_val c_PASTE(csortval_, i_tag);

static inline void c_PASTE(cisort_, i_tag)(i_val arr[], intptr_t low, intptr_t high) {
    for (intptr_t j = low, i = low+1; i <= high; j = i, ++i) {
        i_val key = arr[i];
        while (j >= 0 && (i_less((&key), (&arr[j])))) {
            arr[j + 1] = arr[j];
            --j;
        }
        arr[j + 1] = key;
    }
}

static inline void c_PASTE(cqsort_, i_tag)(i_val arr[], intptr_t low, intptr_t high)
{
    intptr_t i = low, j = high;
    i_val pivot = arr[(i + j)/2];

    while (i <= j) {
        while (i_less((&arr[i]), (&pivot))) ++i;
        while (i_less((&pivot), (&arr[j]))) --j;
        if (i <= j) {
            i_val t = arr[i]; arr[i] = arr[j]; arr[j] = t;
            ++i; --j;
        }
    }
    if (j > low) j - low < 65 ? c_PASTE(cisort_, i_tag)(arr, low, j)
                              : c_PASTE(cqsort_, i_tag)(arr, low, j);
    if (i < high) high - i < 65 ? c_PASTE(cisort_, i_tag)(arr, i, high)
                                : c_PASTE(cqsort_, i_tag)(arr, i, high);
}

static inline void c_PASTE(csort_, i_tag)(i_val arr[], size_t elements)
{
    elements < 65 ? c_PASTE(cisort_, i_tag)(arr, 0, (intptr_t)elements - 1)
                  : c_PASTE(cqsort_, i_tag)(arr, 0, (intptr_t)elements - 1);
}
#undef i_tag
#undef i_val
#undef i_less
