// Generic Quicksort in C, performs as fast as c++ std::sort().
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#ifdef __cplusplus
  #include <algorithm>
#endif
#define i_val int
#include <stc/algo/csort.h>

#define ROTL(d,bits) ((d<<(bits)) | (d>>(8*sizeof(d)-(bits))))
uint64_t random(uint64_t s[3]) {
    uint64_t xp = s[0], yp = s[1], zp = s[2];
    s[0] = 15241094284759029579u * zp;
    s[1] = yp - xp; s[1] = ROTL(s[1], 12);
    s[2] = zp - yp; s[2] = ROTL(s[2], 44);
    return xp;
}

void testsort(int *a, int size, const char *desc) {
    clock_t t = clock();
#ifdef __cplusplus
        { printf("std::sort: "); std::sort(a, a + size); }
#else
        { printf("stc_sort: "); csort_int(a, size); }
#endif
    t = clock() - t;

    printf("time: %.1fms, n: %d, %s\n",
           (double)t*1000.0/CLOCKS_PER_SEC, size, desc);
}


int main(int argc, char *argv[]) {
    size_t i, size = argc > 1 ? strtoull(argv[1], NULL, 0) : 10000000;
    uint64_t s[3] = {123456789, 3456789123, 789123456};

    int32_t *a = (int32_t*)malloc(sizeof(*a) * size);
    if (!a) return -1;

    for (i = 0; i < size; i++)
        a[i] = random(s) & (1U << 30) - 1;
    testsort(a, size, "random");
    for (i = 0; i < 20; i++)
        printf(" %d", (int)a[i]);
    puts("");
    for (i = 0; i < size; i++)
        a[i] = i;
    testsort(a, size, "sorted");
    for (i = 0; i < size; i++)
        a[i] = size - i;
    testsort(a, size, "reverse sorted");
    for (i = 0; i < size; i++)
        a[i] = 126735;
    testsort(a, size, "constant");
    for (i = 0; i < size; i++)
        a[i] = i + 1;
    a[size - 1] = 0;
    testsort(a, size, "rotated");
    free(a);
}
