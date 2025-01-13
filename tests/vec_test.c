#include "ctest.h"

#define i_type IVec,int
#define i_use_cmp
#include "stc/vec.h"


TEST(vec, basics) {
    IVec d = c_make(IVec, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12});
    EXPECT_EQ(12, IVec_size(&d));

    IVec_erase_n(&d, 0, 5);
    for (c_range32(i, 2, 9))
        IVec_push_back(&d, i*10);

    IVec res = c_make(IVec, {6, 7, 8, 9, 10, 11, 12, 20, 30, 40, 50, 60, 70, 80});
    EXPECT_TRUE(IVec_eq(&res, &d));
    EXPECT_EQ(14, IVec_size(&d));

    IVec_erase_n(&d, 7, 4);

    IVec_take(&res, c_make(IVec, {6, 7, 8, 9, 10, 11, 12, 60, 70, 80}));
    EXPECT_TRUE(IVec_eq(&res, &d));

    int nums[] = {200, 300, 400, 500};
    IVec_insert_n(&d, 7, nums, 4);

    IVec_take(&res, c_make(IVec, {6, 7, 8, 9, 10, 11, 12, 200, 300, 400, 500, 60, 70, 80}));
    EXPECT_TRUE(IVec_eq(&res, &d));

    EXPECT_EQ(14, IVec_size(&d));
    EXPECT_EQ(200, *IVec_at(&d, 7));

    c_drop(IVec, &d, &res);
}
