#include <stdio.h>
#include <time.h>
#include <stc/crandom.h>

#define i_val double
#define i_tag fx
#include <stc/clist.h>

int main() {
    int k;
    const int n = 2000000;

    c_auto (clist_fx, list)
    {
        stc64_t rng = stc64_new(1234);
        stc64_uniformf_t dist = stc64_uniformf_new(100.0f, n);
        int m = 0;
        c_forrange (i, int, n)
            clist_fx_push_back(&list, stc64_uniformf(&rng, &dist)), ++m;
        double sum = 0.0;
        printf("sumarize %d:\n", m);
        c_foreach (i, clist_fx, list)
            sum += *i.ref;
        printf("sum %f\n\n", sum);

        k = 0;
        c_foreach (i, clist_fx, list)
            if (++k <= 10) printf("%8d: %10f\n", k, *i.ref); else break;
        puts("sort");
        clist_fx_sort(&list); // mergesort O(n*log n)
        puts("sorted");

        k = 0;
        c_foreach (i, clist_fx, list)
            if (++k <= 10) printf("%8d: %10f\n", k, *i.ref); else break;
        puts("");

        clist_fx_clear(&list);
        c_apply(v, clist_fx_push_back(&list, v), int, {10, 20, 30, 40, 30, 50});
        const double* v = clist_fx_get(&list, 30);
        printf("found: %f\n", *v);
        c_foreach (i, clist_fx, list) printf(" %g", *i.ref);
        puts("");

        clist_fx_remove(&list, 30);
        clist_fx_insert_at(&list, clist_fx_begin(&list), 5); // same as push_front()
        clist_fx_push_back(&list, 500);
        clist_fx_push_front(&list, 1964);
        clist_fx_iter it = clist_fx_begin(&list);
        printf("Full: ");
        c_foreach (i, clist_fx, list)
            printf(" %g", *i.ref);
        printf("\nSubs: ");
        c_foreach (i, clist_fx, clist_fx_advance(it, 4), clist_fx_end(&list))
            printf(" %g", *i.ref);
        puts("");
    }
}
