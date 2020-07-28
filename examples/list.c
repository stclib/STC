#include <stdio.h>
#include <time.h>
#include <stc/clist.h>
#include <stc/crandom.h>
declare_clist(ix, uint64_t);

int main() {
    clist_ix list = clist_init;
    crandom32_t pcg = crandom32_uniform_engine(time(NULL));
    int n;
    for (int i=0; i<10000000; ++i) // ten million
        clist_ix_push_back(&list, crandom32_uniform_int(&pcg));
    n = 100; 
    c_foreach (i, clist_ix, list)
        if (n--) printf("%8d: %10zu\n", 100 - n, i.item->value); else break;
    // Sort them...
    clist_ix_sort(&list); // mergesort O(n*log n)
    n = 100;
    puts("sorted");
    c_foreach (i, clist_ix, list)
        if (n--) printf("%8d: %10zu\n", 100 - n, i.item->value); else break;

    clist_ix_clear(&list);
    c_push(&list, clist_ix, c_items(10, 20, 30, 40, 50));
    c_foreach (i, clist_ix, list) printf("%zu ", i.item->value);
    puts("");

    clist_ix_destroy(&list);
}