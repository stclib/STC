#include <stdio.h>

#define i_val int
#include <stc/cvec.h>

#define i_val int
#include <stc/csset.h>

 
int main()
{
    c_auto (csset_int, set)
    c_auto (cvec_int, vec)
    {
        int key, *res;

        // TEST SORTED VECTOR
        c_apply(t, cvec_int_push(&vec, t), int, { 
            40, 600, 1, 7000, 2, 500, 30, 
        });

        cvec_int_sort(&vec);

        key = 550;
        res = cvec_int_lower_bound(&vec, key).ref;

        if (res != cvec_int_end(&vec).ref)
            printf("Vec key %d: lower bound: %d\n", key, *res); // 600
        else
            printf("Vec element not found\n");


        // TEST SORTED SET
        c_apply(t, csset_int_push(&set, t), int, { 
            40, 600, 1, 7000, 2, 500, 30,
        });

        key = 550;
        res = csset_int_lower_bound(&set, key).ref;

        if (res != csset_int_end(&set).ref)
            printf("Set key %d: lower bound: %d\n", key, *res); // 600
        else
            printf("Set element not found\n");
    }
    return 0;
}
