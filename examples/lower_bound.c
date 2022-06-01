#include <stdio.h>

#define i_val int
#include <stc/cvec.h>

#define i_val int
#include <stc/csset.h>

int main()
{
    // TEST SORTED VECTOR
    c_auto (cvec_int, vec)
    {
        int key, *res;

        c_apply(t, cvec_int_push(&vec, *t), int, { 
            40, 600, 1, 7000, 2, 500, 30, 
        });

        cvec_int_sort(&vec);

        key = 500;
        res = cvec_int_lower_bound(&vec, key).ref;
        if (res != cvec_int_end(&vec).ref)
            printf("Sorted Vec %d: lower bound: %d\n", key, *res); // 600

        key = 550;
        res = cvec_int_lower_bound(&vec, key).ref;
        if (res != cvec_int_end(&vec).ref)
            printf("Sorted Vec %d: lower_bound: %d\n", key, *res); // 500

        key = 500;
        res = cvec_int_binary_search(&vec, key).ref;
        if (res != cvec_int_end(&vec).ref)
            printf("Sorted Vec %d: bin. search: %d\n", key, *res); // 500
        puts("");
    }

    // TEST SORTED SET
    c_auto (csset_int, set)
    {
        int key, *res;

        c_apply(t, csset_int_push(&set, *t), int, { 
            40, 600, 1, 7000, 2, 500, 30,
        });

        key = 500;
        res = csset_int_lower_bound(&set, key).ref;
        if (res != csset_int_end(&set).ref)
            printf("Sorted Set %d: lower bound: %d\n", key, *res); // 600

        key = 550;
        res = csset_int_lower_bound(&set, key).ref;
        if (res != csset_int_end(&set).ref)
            printf("Sorted Set %d: lower bound: %d\n", key, *res); // 600

        key = 500;
        res = csset_int_find(&set, key).ref;
        if (res != csset_int_end(&set).ref)
            printf("Sorted Set %d: find       : %d\n", key, *res); // 600
    }
    return 0;
}
