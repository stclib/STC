#include <stdio.h>
#include <stc/cspan.h>
#include "ctest.h"

using_cspan3(intspan, int);


CTEST(cspan, subdim) {
    int array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    intspan3 m = cspan_md(array, 2, 2, 3);
    
    for (size_t i = 0; i < m.dim[0]; ++i) {
        intspan2 sub_i = cspan_submd3(&m, i);
        for (size_t j = 0; j < m.dim[1]; ++j) {
            intspan sub_i_j = cspan_submd2(&sub_i, j);
            for (size_t k = 0; k < m.dim[2]; ++k) {
               ASSERT_EQ(*cspan_at(&sub_i_j, k), *cspan_at(&m, i, j, k));
            }
        }
    }
}

CTEST(cspan, slice) {
    int array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    intspan2 m1 = cspan_md(array, 3, 4);

    size_t sum1 = 0;
    for (size_t i = 0; i < m1.dim[0]; ++i) {
        for (size_t j = 0; j < m1.dim[1]; ++j) {
            sum1 += *cspan_at(&m1, i, j);
        }
    }

    intspan2 m2 = m1;
    cspan_slice(&m2, {0}, {2,4});

    size_t sum2 = 0;
    for (size_t i = 0; i < m2.dim[0]; ++i) {
        for (size_t j = 0; j < m2.dim[1]; ++j) {
            sum2 += *cspan_at(&m2, i, j);
        }
    }
    ASSERT_EQ(78, sum1);
    ASSERT_EQ(45, sum2);
}

#define i_val int
#include <stc/cstack.h>

CTEST(cspan, slice2) {
    c_AUTO (cstack_int, stack)
    {
        c_FORRANGE (i, 10*20*30)
            cstack_int_push(&stack, i);

        intspan3 ms3 = cspan_md(stack.data, 10, 20, 30);
        cspan_slice(&ms3, {1,4}, {3,7}, {20,24});

        size_t sum = 0;
        for (size_t i = 0; i < ms3.dim[0]; ++i) {
            for (size_t j = 0; j < ms3.dim[1]; ++j) {
                for (size_t k = 0; k < ms3.dim[2]; ++k) {
                    sum += *cspan_at(&ms3, i, j, k);
                }
            }
        }
        ASSERT_EQ(65112, sum);

        sum = 0;
        c_FOREACH (i, intspan3, ms3)
            sum += *i.ref;
        ASSERT_EQ(65112, sum);
    }
}
