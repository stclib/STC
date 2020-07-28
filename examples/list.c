#include <stdio.h>
#include <time.h>
#include <stc/clist.h>
#include <stc/crandom.h>
declare_CList(ix, uint64_t);

int main() {
    CList_ix list = clist_init;
    CRand32 pcg = crand32_init(time(NULL));
    int n;
    for (int i=0; i<10000000; ++i) // ten million
        clist_ix_pushBack(&list, crand32_gen(&pcg));
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