#include "ctest.h"

#define T IDeq, int, (c_use_eq)
#include <stc/deque.h>


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
    EXPECT_EQ(*IDeq_find(&res3, 400).ref, 400);
    EXPECT_NULL(IDeq_find(&res3, 401).ref);

    EXPECT_EQ(14, IDeq_size(&d));
    EXPECT_EQ(200, *IDeq_at(&d, 7));

    c_drop(IDeq, &d, &res1, &res2, &res3);
}

#include <stc/cstr.h>
#define T SDeq, cstr, (c_pro_key|c_use_eq)
#include <stc/deque.h>

TEST(deque, bulk) {

    const char* tokens[] = {"Hello", "cruel", "world"};
    SDeq d = {0};
    SDeq_put_n(&d, tokens, c_countof(tokens));
    EXPECT_EQ(3, SDeq_size(&d));

    const char* names[] = {"item1", "item2", "item3"};
    SDeq_put_n(&d, names, c_countof(names));
    SDeq_emplace_n(&d, 0, names, c_countof(names));
    SDeq_put_n(&d, tokens, c_countof(tokens));

    SDeq res1 = c_make(SDeq, {
        "item1", "item2", "item3",
        "Hello", "cruel", "world",
        "item1", "item2", "item3",
        "Hello", "cruel", "world",
    });
    EXPECT_TRUE(SDeq_eq(&d, &res1));

    SDeq_erase_n(&d, 3, 6);

    SDeq res2 = c_make(SDeq, {
        "item1", "item2", "item3",
        "Hello", "cruel", "world",
    });
    EXPECT_TRUE(SDeq_eq(&d, &res2));

    c_drop(SDeq, &d, &res1, &res2);
}
