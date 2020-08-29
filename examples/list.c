#include <stdio.h>
#include <time.h>
#include <stc/clist.h>
#include <stc/crandom.h>
declare_clist(fx, double);

int main() {
    int k, n = 100000;
    clist_fx list = clist_init;
    crand_rng64_t eng = crand_rng64_init(time(NULL));
    crand_uniform_f64_t dist = crand_uniform_f64_init(eng, 0.0f, n);
    
    for (int i = 0; i < 100000; ++i)
        clist_fx_push_back(&list, crand_uniform_f64(&dist));
    k = 0; c_foreach (i, clist_fx, list)
        if (++k <= 10) printf("%8d: %10f\n", k, i.item->value); else break;

    clist_fx_sort(&list); // mergesort O(n*log n)
    puts("sorted");

    k = 0; c_foreach (i, clist_fx, list)
        if (++k <= 10) printf("%8d: %10f\n", k, i.item->value); else break;

    clist_fx_clear(&list);
    c_push(&list, clist_fx, c_items(10, 20, 30, 40, 50));
    c_foreach (i, clist_fx, list) printf("%.1f ", i.item->value);
    puts("");

    clist_fx_destroy(&list);
}