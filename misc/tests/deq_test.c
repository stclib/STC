#include "ctest.h"
#include "stc/algorithm.h"

#define i_TYPE IDeq,int
#define i_use_cmp
#include "stc/deq.h"


CTEST(deq, basics) {
    IDeq d = c_init(IDeq, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12});
    ASSERT_EQ(12, IDeq_size(&d));

    c_forrange (5)
        IDeq_pop_front(&d);
    c_forrange (i, 2, 9)
        IDeq_push_back(&d, i*10);

    IDeq res1 = c_init(IDeq, {6, 7, 8, 9, 10, 11, 12, 20, 30, 40, 50 , 60, 70, 80});
    ASSERT_TRUE(IDeq_eq(&res1, &d));

    IDeq_erase_n(&d, 7, 4);

    IDeq res2 = c_init(IDeq, {6, 7, 8, 9, 10, 11, 12, 60, 70, 80});
    ASSERT_TRUE(IDeq_eq(&res2, &d));

    int nums[] = {200, 300, 400, 500};
    IDeq_insert_n(&d, 7, nums, 4);

    IDeq res3 = c_init(IDeq, {6, 7, 8, 9, 10, 11, 12, 200, 300, 400, 500, 60, 70, 80});
    ASSERT_TRUE(IDeq_eq(&res3, &d));

    ASSERT_EQ(14, IDeq_size(&d));
    ASSERT_EQ(200, *IDeq_at(&d, 7));

    c_drop(IDeq, &d, &res1, &res2, &res3);
}
