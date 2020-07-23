#include <stdio.h>
#include <time.h>
#include <stc/clist.h>
#include <stc/crandom.h>
declare_CList(ix, uint64_t);

int main() {
    CList_ix list = clist_ix_from((uint64_t[]) {10, 20, 30, 40}, 4);
    c_foreach (i, clist_ix, list) printf("%zu ", i.item->value);
    puts("");

    crandom32_t pcg = crandom32_init(time(NULL));
    int n;
    for (int i=0; i<10000000; ++i) // ten million
        clist_ix_pushBack(&list, crandom32(&pcg));
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