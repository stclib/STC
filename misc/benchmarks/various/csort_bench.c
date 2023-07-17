// Generic Quicksort in C, performs as fast as c++ std::sort().
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#ifdef __cplusplus
  #include <algorithm>
#endif
#define NDEBUG
#define i_type Ints
#define i_key int
#define i_more
#include <stc/cvec.h>
#include <stc/algo/sort.h>

#define ROTL(d,bits) ((d<<(bits)) | (d>>(8*sizeof(d)-(bits))))
uint64_t romutrio(uint64_t s[3]) {
    uint64_t xp = s[0], yp = s[1], zp = s[2];
    s[0] = 15241094284759029579u * zp;
    s[1] = yp - xp; s[1] = ROTL(s[1], 12);
    s[2] = zp - yp; s[2] = ROTL(s[2], 44);
    return xp;
}

static int cmp_int(const void* a, const void* b) {
    return c_default_cmp((const int*)a, (const int*)b);
}

void testsort(Ints *a, int size, const char *desc) {
    clock_t t = clock();
#ifdef __cplusplus
    printf("std::sort: "); std::sort(a->data, a->data + size);
#elif defined QSORT
    printf("qsort: "); qsort(a->data, size, sizeof *a->data, cmp_int);
#else
    printf("STC sort_n: "); Ints_sort_n(a, size);
#endif
    t = clock() - t;

    printf("time: %.1fms, n: %d, %s\n",
           (double)t*1000.0/CLOCKS_PER_SEC, size, desc);
}


int main(int argc, char *argv[]) {
    size_t i, size = argc > 1 ? strtoull(argv[1], NULL, 0) : 10000000;
    uint64_t s[3] = {123456789, 3456789123, 789123456};

    Ints a = Ints_with_capacity(size);

    for (i = 0; i < size; i++)
        Ints_push(&a, romutrio(s) & (1U << 30) - 1);
    testsort(&a, size, "random");
    for (i = 0; i < 20; i++)
        printf(" %d", (int)*Ints_at(&a, i));
    puts("");
    for (i = 0; i < size; i++)
        *Ints_at_mut(&a, i) = i;
    testsort(&a, size, "sorted");
    for (i = 0; i < size; i++)
        *Ints_at_mut(&a, i) = size - i;
    testsort(&a, size, "reverse sorted");
    for (i = 0; i < size; i++)
        *Ints_at_mut(&a, i) = 126735;
    testsort(&a, size, "constant");
    for (i = 0; i < size; i++)
        *Ints_at_mut(&a, i) = i + 1;
    *Ints_at_mut(&a, size - 1) = 0;
    testsort(&a, size, "rotated");

    Ints_drop(&a);
}
