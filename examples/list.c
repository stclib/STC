#include <stdio.h>
#include <time.h>
#include <stc/clist.h>
#include <stc/crand.h>
declare_clist(fx, double);

int main() {
    clist_fx list = clist_init;
    crand_eng64_t eng = crand_eng64_init(time(NULL));
    crand_uniform_f64_t dist = crand_uniform_f64_init(1.0, 100.0);
    int n;
    for (int i = 0; i < 10000000; ++i) // ten million
        clist_fx_push_back(&list, crand_uniform_f64(&eng, dist));
    n = 100; 
    c_foreach (i, clist_fx, list)
        if (n--) printf("%8d: %10f\n", 100 - n, i.item->value); else break;
    // Sort them...
    clist_fx_sort(&list); // mergesort O(n*log n)
    n = 100;
    puts("sorted");
    c_foreach (i, clist_fx, list)
        if (n--) printf("%8d: %10f\n", 100 - n, i.item->value); else break;

    clist_fx_clear(&list);
    c_push(&list, clist_fx, c_items(10, 20, 30, 40, 50));
    c_foreach (i, clist_fx, list) printf("%f ", i.item->value);
    puts("");

    clist_fx_destroy(&list);
}