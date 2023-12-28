#include <stdio.h>
#include <time.h>
#include "stc/algorithm.h"
#include "stc/crand.h"

#define i_TYPE DList,double
#define i_use_cmp
#include "stc/list.h"

int main(void) {
    const int n = 3000000;
    DList list = {0};

    csrand(1234567);
    int m = 0;
    c_forrange (n)
        DList_push_back(&list, crandf()*n + 100), ++m;

    printf("sum of %d: ", m);
    double sum = 0.0;
    c_filter(DList, list, sum += *value);
    printf("%f\n", sum);

    // Print 10 first items (using c_filter):
    c_filter(DList, list,
        (printf("%4d: %10f\n", c_flt_counter(), *value), c_flt_take(10))
    );

    puts("sort:");
    DList_sort(&list); // qsort O(n*log n)

    // Alternative way to print 10 first items:
    c_foreach_n (i, DList, list, 10)
        printf("%4d: %10f\n", (int)i.index + 1, *i.ref);

    DList_drop(&list);
    list = c_init(DList, {10, 20, 30, 40, 30, 50});

    printf("List: ");
    c_foreach (i, DList, list)
        printf(" %g", *i.ref);
    puts("");

    const double* v = DList_get(&list, 30);
    printf("Found: %g\n", *v);

    DList_remove(&list, 30);
    DList_insert_at(&list, DList_begin(&list), 5); // same as push_front()
    DList_push_front(&list, 2023);
    DList_push_back(&list, 2024);

    printf("Full: ");
    c_foreach (i, DList, list)
        printf(" %g", *i.ref);

    printf("\nTail: ");
    DList_iter it = DList_begin(&list);

    c_foreach (i, DList, DList_advance(it, 4), DList_end(&list))
        printf(" %g", *i.ref);
    puts("");

    DList_drop(&list);
}
