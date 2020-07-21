#include <stdio.h>
#include <time.h>
#include <stc/clist.h>
#include <stc/crandom.h>
declare_CList(ix, uint64_t);

int main() {
    CList_ix list = clist_ix_from((uint64_t[]) {10, 20, 30, 40}, 4);
    c_foreach (i, clist_ix, list) printf("%zu ", i.item->value);
    puts("");

    pcg32_random_t pcg = pcg32_seed(time(NULL), 0);
    int n;
    for (int i=0; i<10000000; ++i) // ten million
        clist_ix_pushBack(&list, pcg32_random(&pcg));
    n = 100; 
    c_foreach (i, clist_ix, list)
        if (n--) printf("%8d: %10zu\n", 100 - n, i.item->value); else break;
    // Sort them...
    clist_ix_sort(&list); // mergesort O(n*log n)
    n = 100;
    puts("sorted");
    c_foreach (i, clist_ix, list)
        if (n--) printf("%8d: %10zu\n", 100 - n, i.item->value); else break;
    clist_ix_destroy(&list);
}