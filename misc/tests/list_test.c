#include <stdio.h>
#include "ctest.h"

#define i_TYPE IList,int
#define i_use_cmp
#include "stc/list.h"

TEST(list, splice)
{
    IList list1 = c_init(IList, {1, 2, 3, 4, 5});
    IList list2 = c_init(IList, {10, 20, 30, 40, 50});
    IList_iter pos = IList_advance(IList_begin(&list1), 2);

    // splice list1 into list2 after pos:
    pos = IList_splice(&list1, pos, &list2);

    IList res1 = c_init(IList, {1, 2, 10, 20, 30, 40, 50, 3, 4, 5});
    EXPECT_EQ(*pos.ref, 3);
    EXPECT_TRUE(IList_eq(&list1, &res1));
    EXPECT_TRUE(IList_is_empty(&list2));

    // splice items from pos to end of list1 into empty list2:
    IList_splice_range(&list2, IList_begin(&list2), &list1, pos, IList_end(&list1));

    IList res2 = c_init(IList, {1, 2, 10, 20, 30, 40, 50});
    EXPECT_TRUE(IList_eq(&list1, &res2));

    IList res3 = c_init(IList, {3, 4, 5});
    EXPECT_TRUE(IList_eq(&list2, &res3));

    EXPECT_FALSE(IList_eq(&list1, &list2));
    c_drop(IList, &list1, &list2, &res1, &res2, &res3);
}
