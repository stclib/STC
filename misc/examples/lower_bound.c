#include <stdio.h>

#define i_val int
#include <stc/cvec.h>

#define i_val int
#include <stc/csset.h>

int main()
{
    // TEST SORTED VECTOR
    c_AUTO (cvec_int, vec)
    {
        int key, *res;

        c_FORLIST (i, int, {40, 600, 1, 7000, 2, 500, 30})
            cvec_int_push(&vec, *i.ref);

        cvec_int_sort(&vec);

        key = 100;
        res = cvec_int_lower_bound(&vec, key).ref;
        if (res)
            printf("Sorted Vec %d: lower bound: %d\n", key, *res); // 500

        key = 10;
        cvec_int_iter it1 = cvec_int_lower_bound(&vec, key);
        if (res)
            printf("Sorted Vec %3d: lower_bound: %d\n", key, *it1.ref); // 30

        key = 600;
        cvec_int_iter it2 = cvec_int_binary_search(&vec, key);
        if (res)
            printf("Sorted Vec %d: bin. search: %d\n", key, *it2.ref); // 600

        c_FOREACH (i, cvec_int, it1, it2)
            printf("  %d\n", *i.ref);

        puts("");
    }

    // TEST SORTED SET
    c_AUTO (csset_int, set)
    {
        int key, *res;

        c_FORLIST (i, int, {40, 600, 1, 7000, 2, 500, 30})
            csset_int_push(&set, *i.ref);

        key = 100;
        res = csset_int_lower_bound(&set, key).ref;
        if (res)
            printf("Sorted Set %d: lower bound: %d\n", key, *res); // 500

        key = 10;
        csset_int_iter it1 = csset_int_lower_bound(&set, key);
        if (res)
            printf("Sorted Set %3d: lower bound: %d\n", key, *it1.ref); // 30

        key = 600;
        csset_int_iter it2 = csset_int_find(&set, key);
        if (res)
            printf("Sorted Set %d: find       : %d\n", key, *it2.ref); // 600

        c_FOREACH (i, csset_int, it1, it2)
            printf("  %d\n", *i.ref);
    }
}
