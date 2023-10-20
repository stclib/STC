#include <stdio.h>

#define i_key int
#define i_use_cmp
#define i_more
#include "stc/cvec.h"
#include "stc/algo/quicksort.h"

#define i_key int
#include "stc/csset.h"

int main(void)
{
    // TEST SORTED VECTOR
    {
        int key;
        cvec_int vec = c_init(cvec_int, {40, 600, 1, 7000, 2, 500, 30});

        cvec_int_quicksort(&vec);

        key = 100;
        intptr_t res = cvec_int_lower_bound(&vec, key);
        if (res != -1)
            printf("Sorted Vec %d: lower bound: %d\n", key, vec.data[res]); // 500

        key = 10;
        intptr_t it1 = cvec_int_lower_bound(&vec, key);
        if (it1 != -1)
            printf("Sorted Vec %3d: lower_bound: %d\n", key, vec.data[it1]); // 30

        key = 600;
        intptr_t it2 = cvec_int_binary_search(&vec, key);
        if (it2 != -1)
            printf("Sorted Vec %d: bin. search: %d\n", key, vec.data[it2]); // 600

        for (intptr_t i = it1; i != it2; ++i)
            printf(" %d\n", vec.data[i]);

        puts("");
        cvec_int_drop(&vec);
    }
    
    // TEST SORTED SET
    {
        int key, *res;
        csset_int set = c_init(csset_int, {40, 600, 1, 7000, 2, 500, 30});

        key = 100;
        res = csset_int_lower_bound(&set, key).ref;
        if (res)
            printf("Sorted Set %d: lower bound: %d\n", key, *res); // 500

        key = 10;
        csset_int_iter it1 = csset_int_lower_bound(&set, key);
        if (it1.ref)
            printf("Sorted Set %3d: lower bound: %d\n", key, *it1.ref); // 30

        key = 600;
        csset_int_iter it2 = csset_int_find(&set, key);
        if (it2.ref)
            printf("Sorted Set %d: find       : %d\n", key, *it2.ref); // 600

        c_foreach (i, csset_int, it1, it2)
            printf("  %d\n", *i.ref);

        csset_int_drop(&set);
    }
}
