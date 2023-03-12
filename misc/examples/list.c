#include <stdio.h>
#include <time.h>
#include <stc/calgo.h>
#include <stc/crandom.h>

#define i_val double
#define i_tag fx
#define i_extern // include sort function
#include <stc/clist.h>

int main() {
    const int n = 1000000;

    c_auto (clist_fx, list)
    {
        stc64_t rng = stc64_new(1234);
        stc64_uniformf_t dist = stc64_uniformf_new(100.0f, n);
        int m = 0;
        c_forrange (n)
            clist_fx_push_back(&list, stc64_uniformf(&rng, &dist)), ++m;

        double sum = 0.0;
        printf("sumarize %d:\n", m);
        c_foreach (i, clist_fx, list)
            sum += *i.ref;
        printf("sum %f\n\n", sum);

        c_forfilter (i, clist_fx, list, c_flt_take(i, 10))
            printf("%8d: %10f\n", c_flt_last(i), *i.ref);

        puts("sort");
        clist_fx_sort(&list); // mergesort O(n*log n)
        puts("sorted");

        double last = 0;
        c_foreach (i, clist_fx, list) {
            if (*i.ref < last) {printf("ERROR"); exit(-1);}
            last = *i.ref;
        }

        c_forfilter (i, clist_fx, list, c_flt_take(i, 10))
            printf("%8d: %10f\n", c_flt_last(i), *i.ref);
        puts("");

        clist_fx_clear(&list);
        c_forlist (i, int, {10, 20, 30, 40, 30, 50})
            clist_fx_push_back(&list, *i.ref);

        const double* v = clist_fx_get(&list, 30);
        printf("found: %f\n", *v);
        c_foreach (i, clist_fx, list)
            printf(" %g", *i.ref);
        puts("");

        clist_fx_remove(&list, 30);
        clist_fx_insert_at(&list, clist_fx_begin(&list), 5); // same as push_front()
        clist_fx_push_back(&list, 500);
        clist_fx_push_front(&list, 1964);

        printf("Full: ");
        c_foreach (i, clist_fx, list)
            printf(" %g", *i.ref);

        printf("\nSubs: ");
        clist_fx_iter it = clist_fx_begin(&list);
        c_foreach (i, clist_fx, clist_fx_advance(it, 4), clist_fx_end(&list))
            printf(" %g", *i.ref);
        puts("");
    }
}
