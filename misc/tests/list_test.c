#include <stdio.h>
#include "ctest.h"

#define i_type IList, int
#define i_use_cmp
#include "stc/list.h"


TEST(list, splice)
{
    IList list1 = c_make(IList, {1, 2, 3, 4, 5});
    IList list2 = c_make(IList, {10, 20, 30, 40, 50});
    IList_iter pos = IList_advance(IList_begin(&list1), 2);

    // splice list1 into list2 after pos:
    pos = IList_splice(&list1, pos, &list2);

    IList res1 = c_make(IList, {1, 2, 10, 20, 30, 40, 50, 3, 4, 5});
    EXPECT_EQ(*pos.ref, 3);
    EXPECT_TRUE(IList_eq(&res1, &list1));
    EXPECT_TRUE(IList_is_empty(&list2));

    // splice items from pos to end of list1 into empty list2:
    IList_splice_range(&list2, IList_begin(&list2), &list1, pos, IList_end(&list1));

    IList res2 = c_make(IList, {1, 2, 10, 20, 30, 40, 50});
    EXPECT_TRUE(IList_eq(&res2, &list1));

    IList res3 = c_make(IList, {3, 4, 5});
    EXPECT_TRUE(IList_eq(&res3, &list2));
    EXPECT_FALSE(IList_eq(&list1, &list2));

    c_drop(IList, &list1, &list2, &res1, &res2, &res3);
}

TEST(list, erase)
{
    IList L = c_make(IList, {10, 20, 30, 40, 50});

    IList_iter it = IList_begin(&L);
    IList_next(&it);
    it = IList_erase_at(&L, it);

    IList res1 = c_make(IList, {10, 30, 40, 50});
    EXPECT_TRUE(IList_eq(&res1, &L));

    IList_next(&it);
    it = IList_erase_range(&L, it, IList_end(&L));

    IList res2 = c_make(IList, {10, 30});
    EXPECT_TRUE(IList_eq(&res2, &L));

    c_drop(IList, &L, &res1, &res2);
}


TEST(list, misc)
{
    IList nums = {0}, nums2 = {0};
    IList_clear(&nums);

    for (int i = 0; i < 10; ++i)
        IList_push_back(&nums, i);
    for (int i = 100; i < 110; ++i)
        IList_push_back(&nums2, i);

    /* splice nums2 to front of nums */
    IList_splice(&nums, IList_begin(&nums), &nums2);

    IList res1 = c_make(IList, {100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
    EXPECT_TRUE(IList_eq(&res1, &nums));

    *IList_find(&nums, 104).ref += 50;
    IList_remove(&nums, 103);
    IList res2 = c_make(IList, {100, 101, 102, 154, 105, 106, 107, 108, 109, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
    EXPECT_TRUE(IList_eq(&res2, &nums));

    IList_iter it = IList_begin(&nums);
    IList_erase_range(&nums, IList_advance(it, 5), IList_advance(it, 15));
    IList res3 = c_make(IList, {100, 101, 102, 154, 105, 6, 7, 8, 9});
    EXPECT_TRUE(IList_eq(&res3, &nums));

    IList_pop_front(&nums);
    IList_push_back(&nums, -99);
    IList_sort(&nums);
    IList res4 = c_make(IList, {-99, 6, 7, 8, 9, 101, 102, 105, 154});
    EXPECT_TRUE(IList_eq(&res4, &nums));

    c_drop(IList, &nums, &nums2, &res1, &res2, &res3, &res4);
}
