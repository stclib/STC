#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stc/crandom.h>

typedef long long Elem;
#define fmt_Elem "%lld"
#ifdef __cplusplus
  #include <algorithm>
#else
  #define i_val Elem
  #include <stc/algo/csort.h>
#endif


int testsort(Elem *a, size_t size, const char *desc) {
    clock_t t = clock();
#ifdef __cplusplus
    printf("std::sort: ");
    std::sort(a, a + size);
#else
    printf("csort: ");
    csort_Elem(a, size);
#endif
    t = clock() - t;

    printf("%s: %d elements sorted in %.2f ms\n",
           desc, (int)size, t*1000.0/CLOCKS_PER_SEC);
    return 0;
}

int main(int argc, char *argv[]) {
    size_t i, size = argc > 1 ? strtoull(argv[1], NULL, 0) : 10000000;
    Elem *a = (Elem*)malloc(sizeof(*a) * size);
    if (a == NULL) return -1;

    for (i = 0; i < size; i++)
        a[i] = crandom() & ((1U << 28) - 1);

    testsort(a, size, "random");
    for (i = 0; i < 20; i++)
        printf(" " fmt_Elem, a[i]);
    puts("");

    testsort(a, size, "sorted");

    for (i = 0; i < size; i++) a[i] = size - i;
    testsort(a, size, "reverse sorted");

    for (i = 0; i < size; i++) a[i] = 126735;
    testsort(a, size, "constant");

    for (i = 0; i < size; i++) a[i] = i + 1;
    a[size - 1] = 0;
    testsort(a, size, "rotated");

    for (i = 0; i < size; i++) a[i] = i % (size / 2);
    testsort(a, size, "repeated");

    free(a);
}
