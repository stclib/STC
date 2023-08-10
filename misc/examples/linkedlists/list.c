#include <stdio.h>
#include <time.h>
#include <stc/algorithm.h>
#include <stc/crand.h>

#define i_type DList
#define i_key double
#define i_use_cmp
#include <stc/clist.h>

int main(void) {
    const int n = 3000000;
    DList list = {0};

    csrand(1234567);
    int m = 0;
    c_forrange (n)
        DList_push_back(&list, crandf()*n + 100), ++m;

    double sum = 0.0;
    printf("sumarize %d:\n", m);
    c_foreach (i, DList, list)
        sum += *i.ref;
    printf("sum %f\n\n", sum);

    c_forfilter (i, DList, list, c_flt_take(i, 10))
        printf("%8d: %10f\n", c_flt_getcount(i), *i.ref);

    puts("sort");
    DList_sort(&list); // qsort O(n*log n)
    puts("sorted");

    c_forfilter (i, DList, list, c_flt_take(i, 10))
        printf("%8d: %10f\n", c_flt_getcount(i), *i.ref);
    puts("");

    DList_drop(&list);
    list = c_init(DList, {10, 20, 30, 40, 30, 50});

    const double* v = DList_get(&list, 30);
    printf("found: %f\n", *v);

    c_foreach (i, DList, list)
        printf(" %g", *i.ref);
    puts("");

    DList_remove(&list, 30);
    DList_insert_at(&list, DList_begin(&list), 5); // same as push_front()
    DList_push_back(&list, 500);
    DList_push_front(&list, 1964);

    printf("Full: ");
    c_foreach (i, DList, list)
        printf(" %g", *i.ref);

    printf("\nSubs: ");
    DList_iter it = DList_begin(&list);

    c_foreach (i, DList, DList_advance(it, 4), DList_end(&list))
        printf(" %g", *i.ref);
    puts("");

    DList_drop(&list);
}
