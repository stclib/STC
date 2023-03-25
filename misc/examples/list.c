#include <stdio.h>
#include <time.h>
#include <stc/algo/filter.h>
#include <stc/crandom.h>

#define i_type DList
#define i_val double
#include <stc/clist.h>

int main() {
    const int n = 3000000;
    DList list = {0};

    stc64_t rng = stc64_new(1234567);
    stc64_uniformf_t dist = stc64_uniformf_new(100.0f, n);
    int m = 0;
    c_forrange (n)
        DList_push_back(&list, stc64_uniformf(&rng, &dist)), ++m;

    double sum = 0.0;
    printf("sumarize %d:\n", m);
    c_foreach (i, DList, list)
        sum += *i.ref;
    printf("sum %f\n\n", sum);

    c_forfilter (i, DList, list, c_flt_take(i, 10))
        printf("%8d: %10f\n", c_flt_last(i), *i.ref);

    puts("sort");
    DList_sort(&list); // qsort O(n*log n)
    puts("sorted");

    c_forfilter (i, DList, list, c_flt_take(i, 10))
        printf("%8d: %10f\n", c_flt_last(i), *i.ref);
    puts("");

    DList_drop(&list);
    list = c_make(DList, {10, 20, 30, 40, 30, 50});

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
