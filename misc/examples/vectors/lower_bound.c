#include <stdio.h>

#define i_TYPE IVec,int
#define i_opt c_use_cmp | c_more
#include "stc/vec.h"
#include "stc/algo/quicksort.h"

#define i_TYPE ISet,int
#include "stc/sset.h"

int main(void)
{
    // TEST SORTED VECTOR
    {
        int key;
        IVec vec = c_init(IVec, {40, 600, 1, 7000, 2, 500, 30});

        IVec_quicksort(&vec);

        key = 100;
        intptr_t res = IVec_lower_bound(&vec, key);
        if (res != -1)
            printf("Sorted Vec %d: lower bound: %d\n", key, vec.data[res]); // 500

        key = 10;
        intptr_t it1 = IVec_lower_bound(&vec, key);
        if (it1 != -1)
            printf("Sorted Vec %3d: lower_bound: %d\n", key, vec.data[it1]); // 30

        key = 600;
        intptr_t it2 = IVec_binary_search(&vec, key);
        if (it2 != -1)
            printf("Sorted Vec %d: bin. search: %d\n", key, vec.data[it2]); // 600

        for (intptr_t i = it1; i != it2; ++i)
            printf(" %d\n", vec.data[i]);

        puts("");
        IVec_drop(&vec);
    }

    // TEST SORTED SET
    {
        int key, *res;
        ISet set = c_init(ISet, {40, 600, 1, 7000, 2, 500, 30});

        key = 100;
        res = ISet_lower_bound(&set, key).ref;
        if (res)
            printf("Sorted Set %d: lower bound: %d\n", key, *res); // 500

        key = 10;
        ISet_iter it1 = ISet_lower_bound(&set, key);
        if (it1.ref)
            printf("Sorted Set %3d: lower bound: %d\n", key, *it1.ref); // 30

        key = 600;
        ISet_iter it2 = ISet_find(&set, key);
        if (it2.ref)
            printf("Sorted Set %d: find       : %d\n", key, *it2.ref); // 600

        c_foreach (i, ISet, it1, it2)
            printf("  %d\n", *i.ref);

        ISet_drop(&set);
    }
}
