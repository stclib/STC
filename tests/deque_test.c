#include "ctest.h"

#define i_type IDeq,int
#define i_use_cmp
#include "stc/deque.h"


TEST(deque, basics) {
    IDeq d = c_make(IDeq, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12});
    EXPECT_EQ(12, IDeq_size(&d));

    for (c_range(5))
        IDeq_pop_front(&d);
    for (c_range32(i, 2, 9))
        IDeq_push_back(&d, i*10);

    IDeq res1 = c_make(IDeq, {6, 7, 8, 9, 10, 11, 12, 20, 30, 40, 50, 60, 70, 80});
    EXPECT_TRUE(IDeq_eq(&res1, &d));
    EXPECT_EQ(14, IDeq_size(&d));

    IDeq_erase_n(&d, 7, 4);

    IDeq res2 = c_make(IDeq, {6, 7, 8, 9, 10, 11, 12, 60, 70, 80});
    EXPECT_TRUE(IDeq_eq(&res2, &d));

    int nums[] = {200, 300, 400, 500};
    IDeq_insert_n(&d, 7, nums, 4);

    IDeq res3 = c_make(IDeq, {6, 7, 8, 9, 10, 11, 12, 200, 300, 400, 500, 60, 70, 80});
    EXPECT_TRUE(IDeq_eq(&res3, &d));

    EXPECT_EQ(14, IDeq_size(&d));
    EXPECT_EQ(200, *IDeq_at(&d, 7));

    c_drop(IDeq, &d, &res1, &res2, &res3);
}
