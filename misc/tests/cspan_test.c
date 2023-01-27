#include <stdio.h>
#include <stc/cspan.h>
#include "ctest.h"

using_cspan3(intspan, int);


CTEST(cspan, subdim) {
    int array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    intspan3 m = cspan_multidim(array, 2, 2, 3);
    
    for (size_t i = 0; i < m.dim[0]; ++i) {
        intspan2 sub_i = cspan_at3(&m, i);
        for (size_t j = 0; j < m.dim[1]; ++j) {
            intspan sub_i_j = cspan_at2(&sub_i, j);
            for (size_t k = 0; k < m.dim[2]; ++k) {
               ASSERT_EQ(*cspan_at(&sub_i_j, k), *cspan_at(&m, i, j, k));
            }
        }
    }
}
